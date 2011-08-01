/*
 * Pui.cpp
 *
 *  Created on: 01/03/2011
 *      Author: seba
 */

#include "Pui.h"
#include <limits.h>
#include "TimeUtils.h"


int length = 0;
unsigned char *data;
binary_data result;

TimeUtils* timeUtils = new TimeUtils();
double elapsed = 0;
double sleepInterval = 0;

/**int Pui::insert(unsigned char *bytes, int len){
	data = new unsigned char[len];
	char * dataCasteado = reinterpret_cast<char *>(data);
	const char * bytesCasteado = reinterpret_cast<const char *>(bytes);
    strncpy(dataCasteado, bytesCasteado, len);
    length = len;
}

binary_data Pui::out(){
    result.size = length;
    result.data = capturarImagenAR();
    return result;
}*/


Pui::Pui() {
	this->h_ranges[0]=-1;
	this->h_ranges[1]=-1;

	this->s_ranges[0]=-1;
	this->s_ranges[1]=-1;

	this->v_ranges[0]=-1;
	this->v_ranges[1]=-1;

	this->resulCalc[0]=-1;
	this->resulCalc[1]=-1;
	this->zoomFactor = 1;

	this->cantidad_marcadores_calib = -1;
	this->historicoHabilitado = false;
	this->historico.historicoElementos=NULL;
	this->historico.tope = -1;

	this->debugMode=0;
	this->debugModeImagenes=0;

	this->count = 0;
	arUtilTimerReset();

	this->dataArtoolkit.patts_id = NULL;
	this->dataArtoolkit.coordenadas_marcadores = NULL;

	//Inicializo todas las imagenes en NULL
	this->imageColor =0;
	this->imageHsv=0;
	this->h_plane=0;
	this->s_plane=NULL;
	this->v_plane=NULL;
	this->mascaraBinaria=NULL;
	this->mascaraBinariaInvertida=NULL;
	this->imageColorApuntador=NULL;
	this->imageMonoApuntador=NULL;

	this->multimodalInfo.max_elementos =-1;
	this->multimodalInfo.objetosPUI=NULL;

	this->videoCapStarted = false;
	this->videoCapOpened = false;
	printf("Pui construido!\n");
}

Pui::~Pui() {}
/*Pui::~Pui() {
	printf("Destruyendo PUI...\n");
	delete data;
	if(this->multimodalInfo.max_elementos>0 && this->multimodalInfo.objetosPUI!=NULL){
		delete this->multimodalInfo.objetosPUI;
	}

	if(this->cantidad_marcadores_calib>0){
		for(int i=0;i<this->cantidad_marcadores_calib;i++){
			delete this->propiedades.medidas_apuntador[i];
			delete this->propiedades.medidas_marcador[i];
			delete this->propiedades.relaciones[i];
		}
		delete this->propiedades.medidas_apuntador;
		delete this->propiedades.medidas_marcador;
		delete this->propiedades.relaciones;
		delete this->propiedades.promedios;
		delete resultados.areas_maximas;
		delete this->resultados.areas_minimas;
		delete this->resultados.relaciones_promedio;
	}

	if(this->historicoHabilitado){
		if(this->historico.cantHistorico>0 && this->historico.historicoElementos!=NULL){
			delete this->historico.historicoElementos;
		}
	}

	if(this->dataArtoolkit.patts_id != NULL && this->multimodalInfo.cant_elementos_cargados>0){
		delete this->dataArtoolkit.patts_id;
	}

	if(this->dataArtoolkit.coordenadas_marcadores != NULL){
		delete this->dataArtoolkit.coordenadas_marcadores;
	}

	delete timeUtils;
	printf("Saliendo del destructor PUI...\n");
}
*/

/**
 * Actualmente obsoleta, no hay funciones dentro de PUI que permitan realizar la calibracion
 * de la proporcion del area del apuntador con el area del marcador.
 *
 * Ver programa de ejemplo Calibrador y Utilidades.c para migrar las funcionalidades necesarias
 * a la clase Pui
 */
void Pui::initPropiedadesCalibracion(int veces_por_marcador,int cant_marcadores, int metodo,int debug){
	this->debugMode = debug;
	printf("Inicializacion de propiedades para calibracion\n");
	printf("veces_por_marcador = %d\n",veces_por_marcador);
	printf("cantidad de marcadores = %d\n",cant_marcadores);
	printf("metodo = %d , ",metodo);
	if(metodo==1){
		printf("Promedio\n");
	}else{
		printf("Metodo desconocido\n");
	}
	int i;
	this->cantidad_marcadores_calib = cant_marcadores;
	this->propiedades.veces_por_marcador = veces_por_marcador;
	this->propiedades.metodo = metodo; //El metodo indica de que manera se calculan los valores guardados en el arreglo promedios. Por el momento
								  //Solo se soporta el valor 1 que indica que se calcula el promedio.

	this->propiedades.medidas_apuntador = new double*[cant_marcadores];
	this->propiedades.medidas_marcador = new double*[cant_marcadores];
	this->propiedades.relaciones = new double*[cant_marcadores];
	this->propiedades.promedios = new double[cant_marcadores];

	this->resultados.areas_maximas = new int[cant_marcadores];
	this->resultados.areas_minimas= new int[cant_marcadores];
	this->resultados.relaciones_promedio = new double[cant_marcadores];


	for(i=0;i<cant_marcadores;i++){
		this->propiedades.medidas_apuntador[i] = new double[veces_por_marcador];
		this->propiedades.medidas_marcador[i] = new double[veces_por_marcador];
		this->propiedades.relaciones[i] = new double[veces_por_marcador];
	}
}


void Pui::obtenerCoordenadasIdealesMarcadorNoDetectado(double markerToCameraMatrix[3][4],ARMultiEachMarkerInfoT marker,double hXi_hYi[3][4],double vertex[4][2]){
	double C_Por_Tcm[3][4];
	arUtilMatMul(arParam.mat,markerToCameraMatrix,C_Por_Tcm);
	arUtilMatMul(C_Por_Tcm,marker.trans,hXi_hYi);
	int i;

	/*
	 * 				PROCEDO A CALCULAR LAS COORDENADAS EN PANTALLA DE LOS VERTICES DEL MARCADOR
	 *
	 * 			  V0 ________ V1
	 * 				|		 |
	 * 				|		 |
	 * 			  V3|________| V2
	 */

	double auxiliar[3][4];
	for(i=0;i<4;i++){
		marker.trans[0][3] = marker.pos3d[i][0];		// Seteo la posicion del vertice Vi
		marker.trans[1][3] = marker.pos3d[i][1];
		arUtilMatMul(C_Por_Tcm,marker.trans,auxiliar);
		if(auxiliar[2][3]!=0){
			vertex[i][0] = auxiliar[0][3]/auxiliar[2][3];
			vertex[i][1] = auxiliar[1][3]/auxiliar[2][3];
		}else{
			vertex[i][0] = auxiliar[0][3]/auxiliar[2][3];
			vertex[i][1] = auxiliar[1][3]/auxiliar[2][3];
		}
	}
}

int Pui::leerConfiguracionObjetosPUI(const char* archivoConfiguracion,int max_elementos){
	this->multimodalInfo.objetosPUI = new Elemento[max_elementos];
	this->multimodalInfo.max_elementos = max_elementos;
	FILE *fich;
	char c = '#';
	char str[255];
	fich=fopen(archivoConfiguracion,"r");
	int res = 1;
	int largo = 0;
	int id=-1;
	if(fich != NULL) {
		int i=0;
		while((c!=EOF)&&(i<=max_elementos)){
			if( fscanf(fich,"%c",&c) < 0){
				break;
			}
			if(c!='\n'){
				if(c=='#'){ //consumo la linea del comentario
					while( (c!='\n') && (c!=EOF)){
						fscanf(fich,"%c",&c);
					}
				}else if(c=='&'){
					fscanf(fich,"%s\n",this->multimodalInfo.soundOK);
					fscanf(fich,"%s\n",this->multimodalInfo.soundError);
				}else{
					fseek(fich, -1, SEEK_CUR);
					fscanf(fich,"%d",&id);
					printf("id=%d\n",id);
					if( (id!=i)&&(id)!=-1 ){ //Si el id no es correlativo con los anteriores solo se permite el valor -1 para indicar que no tiene marcador asociado
						printf("c = %c\n",c);
						printf("c-48 = %d\n",c-48);
						printf("i = %d\n",i);
						printf("Error en el archivo de configuracion, los elementos deben aparecer en orden ascendente\n");
						res = 0;

						while( (c!='\n') && (c!=EOF)&&(c!='&')){
							printf("%c\n",c);
							fscanf(fich,"%c",&c);
						}
						break;
					}else{
						this->multimodalInfo.objetosPUI[i].idPatt = id;
						fgetc(fich);//consumo fin de linea

						fgets(str,255,fich);
						strcpy(this->multimodalInfo.objetosPUI[i].imagen,strtok(str,"\n"));

						fgets(str,255,fich);
						strcpy(this->multimodalInfo.objetosPUI[i].sonido,strtok(str,"\n"));

						fgets(str,255,fich);
						strcpy(this->multimodalInfo.objetosPUI[i].nombre,strtok(str,"\n"));

						fscanf(fich,"%d",&(this->multimodalInfo.objetosPUI[i].conjunto));
						fgetc(fich); //consumo fin de linea
						fscanf(fich,"%d",&(this->multimodalInfo.objetosPUI[i].idElementoRelacionado));
						fgetc(fich); //consumo fin de linea

						fgets(str,255,fich);
						strcpy(this->multimodalInfo.objetosPUI[i].nombreRelacion,strtok(str,"\n"));

						//get_buff(str, 255, fich);
						//sscanf(str,"%s",(this->multimodalInfo.objetosPUI[i].nombreRelacion));

						this->multimodalInfo.objetosPUI[i].idImagenSDL = -1;

						fscanf(fich,"%c",&c);
						if(c!='-'){ //El - indica que termina la configuracion de un elemento
							printf("Error en el archivo de configuracion, la configuracion del elemento %d no termina en -\n",i);
							res = 0;
							break;
						}
					}
					i++;
				}
			}
		}
		fclose(fich);
		if(res>0){
			res = i; //Retorna la CANTIDAD de elementos, por eso hay que sumar 1
		}
	}else{
		printf("No se puede leer el archivo de configuracion %s\n",archivoConfiguracion);
	}
	this->multimodalInfo.cant_elementos_cargados = res;
	return res;
}


char * Pui::getNombreObjetoPUI(int idObjeto){
	if(idObjeto< this->multimodalInfo.cant_elementos_cargados){
		return this->multimodalInfo.objetosPUI[idObjeto].nombre;
	}else{
		return NULL;
	}
}

char * Pui::getSonidoObjetoPUI(int idObjeto){
	if(idObjeto< this->multimodalInfo.cant_elementos_cargados){
		return this->multimodalInfo.objetosPUI[idObjeto].sonido;
	}else{
		return NULL;
	}
}


char * Pui::getImagenObjetoPUI(int idObjeto){
	if(idObjeto< this->multimodalInfo.cant_elementos_cargados){
		return this->multimodalInfo.objetosPUI[idObjeto].imagen;
	}else{
		return NULL;
	}
}

/**
 * Retorna true si y solo si el objeto1 esta relacionado con el objeto2
 * objeto1---->objeto2
 * No tiene porque cumplirse la coondicion inversa ( objeto2--->objeto1)
 */
bool Pui::esRelacionCorrecta(int idObjeto1,int idObjeto2){
	bool res = false;
	if(idObjeto1< this->multimodalInfo.cant_elementos_cargados && idObjeto2< this->multimodalInfo.cant_elementos_cargados){
		res =  this->multimodalInfo.objetosPUI[idObjeto1].idElementoRelacionado == idObjeto2;
	}
	return res;
}

int Pui::getConjuntoObjetoPui(int idObjeto){
	if(idObjeto< this->multimodalInfo.cant_elementos_cargados){
		return this->multimodalInfo.objetosPUI[idObjeto].conjunto;
	}else{
		return -1;
	}
}

int Pui::getIdElementoRelacionado(int idObjeto){
	if(idObjeto< this->multimodalInfo.cant_elementos_cargados){
		return this->multimodalInfo.objetosPUI[idObjeto].idElementoRelacionado;
	}else{
		return -1;
	}
}

char* Pui::getNombreRelacionObjetoPUI(int idObjeto){
	if(idObjeto< this->multimodalInfo.cant_elementos_cargados){
		return this->multimodalInfo.objetosPUI[idObjeto].nombreRelacion;
	}else{
		return NULL;
	}
}

char* Pui::getSonidoError(){
	return this->multimodalInfo.soundError;
}

char* Pui::getSonidoOk(){
	return this->multimodalInfo.soundOK;
}

int Pui::leerConfiguracionCalibracion(const char* archivoConfiguracion, int cantMarcadores, char** imagenes){
		FILE *fich;
		char c = '#';
		char str[255];
		fich=fopen(archivoConfiguracion,"r");
		int res = 1;
		int largo = 0;
		if(fich != NULL) {
			int i=0;
			while((c!=EOF)&&(i<=cantMarcadores)){
				if( fscanf(fich,"%c",&c) < 0){
					break;
				}
				if(c!='\n'){
					if(c=='#'){ //consumo la linea del comentario
						while( (c!='\n') && (c!=EOF)){
							fscanf(fich,"%c",&c);
						}
					}else{
						//fgetc(fich);//consumo fin de linea
						fseek(fich, -1, SEEK_CUR);
						fscanf(fich,"%s\n",imagenes[i]);
						i++;
					}

				}
			}
		}else{
			printf("No se puede leer el archivo de configuracion %s\n",archivoConfiguracion);
			return -1;
		}
		return res;
}

void Pui::imprimirPropiedadesObjetosPUI(){
	int i;
	for(i=0;i<this->multimodalInfo.cant_elementos_cargados;i++){
		printf("------------------------------------------------------\n");
		printf("IdPatt=%d\n",this->multimodalInfo.objetosPUI[i].idPatt);
		printf("IdPattElementoRelacionado=%d\n",this->multimodalInfo.objetosPUI[i].idElementoRelacionado);
		printf("Nombre=%s\n",this->multimodalInfo.objetosPUI[i].nombre);
		printf("NombreRelacion=%s\n",this->multimodalInfo.objetosPUI[i].nombreRelacion);
		printf("Imagen=%s\n",this->multimodalInfo.objetosPUI[i].imagen);
		printf("IdImagenSDL=%d\n",this->multimodalInfo.objetosPUI[i].idImagenSDL);
		printf("Sonido=%s\n",this->multimodalInfo.objetosPUI[i].sonido);
		printf("Conjunto=%d\n",this->multimodalInfo.objetosPUI[i].conjunto);
		printf("------------------------------------------------------\n");
	}
	printf("Sound OK = %s\n",this->multimodalInfo.soundOK);
	printf("Sound Error = %s\n",this->multimodalInfo.soundError);
}

int Pui::detectarSeleccion(){
	int resultado = -1;
	if(this->dataArtoolkit.marker_num>0){
		for(int i = 0; i < this->dataArtoolkit.config->marker_num; i++ ) {
			if( (this->dataArtoolkit.coordenadas_marcadores[i].visible<=0)){

				double areaZonaEstudio = 0;
				float area=0;
				if(this->debugModeImagenes){
					area = hayApuntador(this->propiedades.resultado_rangos_color,this->dataArtoolkit.coordenadas_marcadores[i].vertex,
											  OFFSET_ROI,this->dataArtoolkit.dataPtr,this->imageColorApuntador,this->imageMonoApuntador,
											  MIN_AREA_WIDTH_HEIGHT,this->propiedadesDeteccion.min_area,this->propiedadesDeteccion.factorProporcionApuntadorMarcador,
											  this->propiedadesDeteccion.erode,this->propiedadesDeteccion.dilate,this->propiedadesDeteccion.smooth,
											  this->propiedadesDeteccion.enclosing_circle,&areaZonaEstudio,"Debug Image","ROI Image");
				}else{
					area = hayApuntador(this->propiedades.resultado_rangos_color,this->dataArtoolkit.coordenadas_marcadores[i].vertex,
																  OFFSET_ROI,this->dataArtoolkit.dataPtr,this->imageColorApuntador,this->imageMonoApuntador,
																  MIN_AREA_WIDTH_HEIGHT,this->propiedadesDeteccion.min_area,this->propiedadesDeteccion.factorProporcionApuntadorMarcador,
																  this->propiedadesDeteccion.erode,this->propiedadesDeteccion.dilate,this->propiedadesDeteccion.smooth,
																  this->propiedadesDeteccion.enclosing_circle,&areaZonaEstudio,NULL,NULL);
				}
				if(area>0){
					int cant =0;
					if(this->historicoHabilitado){
						cant = registrarColision(i);
					}
					if(cant>=this->historico.cantHistorico || !this->historicoHabilitado){
						resultado=i;
						break;				// Cuando se detecta la primera colision ya frenamos
					}else if(this->debugMode){
						printf("Colision nro %d con marcador %d\n",cant,i);
					}
				}
			}
		}
	}
	return resultado;
}


void Pui::setDebugConImagenes(int debugConImagenes){
	this->debugModeImagenes = debugConImagenes;
}

int Pui::getDebugConImagenes(){
	return this->debugModeImagenes;
}

void Pui::setDebugMode(int debugMode){
	this->debugMode = debugMode;
}

int Pui::getDebugMode(){
	return this->debugMode;
}


void Pui::habilitarHistorico(bool habilitar,int cantHistorico,int valorInicial){
	this->historicoHabilitado = habilitar;
	if(habilitar){
		printf("-->Inicializando estructura de almacenamiento de historico de colisiones\n");
		printf("   Tamaño del historico  %d\n",cantHistorico);
		printf("   Valor inicial         %d\n",valorInicial);

		this->historico.cantHistorico = cantHistorico;
		this->historico.tope = 0;
		this->historico.historicoElementos= new int[cantHistorico];
		int i;
		for(i=0;i<cantHistorico;i++){
			this->historico.historicoElementos[i] = valorInicial;
		}
	}else{
		printf("-->Historico deshabilitado\n");
	}
}

bool Pui::isHistoricoHabilitado(){
	return this->historicoHabilitado;
}

/*Registra la colision del marcador en el historico
 * Retorna la cantidad de colisiones Consecutivas desde la actual hacia atras que tiene el marcador en el historico */
int Pui::registrarColision(int idMarcador){
	this->historico.historicoElementos[this->historico.tope] = idMarcador;

	//Cuento la cantidad de colisiones consecutivas
	int i,cantidad = 0;
	int indice = this->historico.tope;
	for(i=0;i<this->historico.cantHistorico;i++){
		if(this->historico.historicoElementos[indice] == idMarcador){
			cantidad++;
		}else{
			break;
		}
		//printf("indice=%d\n",indice);
		indice = indice--; //Voy recorriendo hacia atras
		if(indice<0){
			indice +=this->historico.cantHistorico;
		}
	}
	//Actualizo el tope
	this->historico.tope = (this->historico.tope +1)%(this->historico.cantHistorico);		//Cuando llego al maximo vuelvo a comenzar en la posicion 0 del arreglo

	return cantidad;
}

void Pui::procesarDatosCalibracion(){
	for(int i=0;i < this->cantidad_marcadores_calib;i++){
		this->resultados.relaciones_promedio[i] = calcularPromedio(this->propiedades.relaciones[i], this->propiedades.veces_por_marcador);
		this->resultados.areas_minimas[i] = calcularMinimo(this->propiedades.medidas_apuntador[i],this->propiedades.veces_por_marcador);
		this->resultados.areas_maximas[i] = calcularMaximo(this->propiedades.medidas_apuntador[i],this->propiedades.veces_por_marcador);
	}
}


void Pui::guardarSoloRangosColorApuntador(char * nombreArchivo) {
	FILE *fich;
	fich=fopen(nombreArchivo,"w");
	if(fich != NULL) {
		fprintf(fich,"#\n%d %d\n%d %d\n%d %d\n-\n",this->propiedades.resultado_rangos_color.hmin,
												   this->propiedades.resultado_rangos_color.hmax,
												   this->propiedades.resultado_rangos_color.smin,
												   this->propiedades.resultado_rangos_color.smax,
												   this->propiedades.resultado_rangos_color.vmin,
												   this->propiedades.resultado_rangos_color.vmax);
		fclose(fich);
	}
}

void Pui::guardarCalibracionApuntador(char * nombreArchivo,int area_min,float factorProporcionUnico,int erode, int dilate,int smooth,int usarEnclosingCircle){
	FILE *fich;
	fich=fopen(nombreArchivo,"w");
	if(fich != NULL) {
		fprintf(fich,"#\n%d %d\n%d %d\n%d %d\n%d\n%f\n%d\n%d\n%d\n%d\n-\n",this->propiedades.resultado_rangos_color.hmin,
												   this->propiedades.resultado_rangos_color.hmax,
												   this->propiedades.resultado_rangos_color.smin,
												   this->propiedades.resultado_rangos_color.smax,
												   this->propiedades.resultado_rangos_color.vmin,
												   this->propiedades.resultado_rangos_color.vmax,
												   area_min,factorProporcionUnico,erode,dilate,smooth,usarEnclosingCircle);

		fclose(fich);
	}
}


void Pui::guardarDatosCalibracion(const char* nombreArchivo){
	FILE *arch = fopen(nombreArchivo,"w"); //Modo w: Si no existe el archivo lo creo, si ya existe lo sobrescribo.
	char aux[100];
	for(int i=0;i<this->cantidad_marcadores_calib;i++){
		sprintf(aux,"-\n",i);
		fputs(aux,arch);				//Escribo -
		sprintf(aux,"%d\n",i);
		fputs(aux,arch);				//Escribo id de marcador
		sprintf(aux,"%f\n",this->resultados.relaciones_promedio[i]); //Escribo relacion
		fputs(aux,arch);
		sprintf(aux,"%d\n",this->resultados.areas_minimas[i]);		//Escribo area minima
		fputs(aux,arch);
		sprintf(aux,"%d\n",this->resultados.areas_maximas[i]);		//Escribo area maxima
		fputs(aux,arch);
	}
	fputc('-',arch);
	fclose(arch);//Cierro archivo
}

void calibrarThresh(ArtoolkitData* artoolkitData,int intervaloDescanso, int cant_iteraciones,double *promedioDetectados, double *cfMarcadores,int thresh,int imprimirDebug){

	ARUint8         *dataPtr = artoolkitData->dataPtr;
	int marker_num = 0;
	ARMarkerInfo    *marker_info;
	int detectados[cant_iteraciones];
	double cf[artoolkitData->config->marker_num][cant_iteraciones];


	//Inicializo cf en 0
	for(int i=0;i<artoolkitData->config->marker_num;i++){
		for(int j=0;j<cant_iteraciones;j++){
			cf[i][j] = 0;
		}
	}

	for (int i=0;i<cant_iteraciones;i++){
		arUtilSleep(90000);
		if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) { //Si no obtengo la imagen entonces no avanzo en la i
			i--;
		}else{
			int res = -1;
			if(artoolkitData->usarAlgoritmoLite){
				res = arDetectMarkerLite(dataPtr, thresh, &marker_info, &marker_num);
			}else{
				res = arDetectMarker(dataPtr, thresh, &marker_info, &marker_num);
			}
			if( res < 0 ) {
				if(imprimirDebug){
					printf("iteracion=%d, error arDetectMarker[Lite]\n",i);
				}
				i--;
			}else{

				detectados[i]=0;
				for(int k=0;k<artoolkitData->config->marker_num;k++){ //Para cada marcador voy a ver si lo encontre
					for(int j=0;j<marker_num;j++){		//Recorro todos los marcadores detectados y guardo el cf si el id coincide con artoolkitData->config->marker[k].patt_id (es necesario verificar que el id coincida porque a veces ar devuelve -1)
						if(marker_info[j].id==artoolkitData->config->marker[k].patt_id){ //Detecto el marcador k
							cf[marker_info[j].id][i] = marker_info[j].cf;
							if(imprimirDebug){
								printf("Iteracion %d, Marcador %d detectado, cf=%f\n",i,artoolkitData->config->marker[k].patt_id,marker_info[j].cf);
							}
							detectados[i]++;
							break;
						}

						/*Comprueba de que si elemento de marker_info tiene id -1 entonces su cf es menor que 0.5
						 * if(marker_info[j].id==-1){
							printf("Marcador de indice %d con id -1. Su cf es: %f\n",j,marker_info[j].cf);
							if(marker_info[j].cf>0.5){
								printf("&&&&ERROR!!!!!! El id es -1 pero el cf es mayor a 0.5\n");
							}
						}*/
					}
				}

				//Busco visibles Para comparar con los detectados comentar esto!
			/*	int err = 0;
				if( (err=arMultiGetTransMat(marker_info, marker_num,artoolkitData->config)) < 0 ) {
						if(imprimirDebug){
							printf("iteracion=%d, error arMultiGetTransMat\n",i);
						}
						i--;
				}else if(err > 100.0){
					if(imprimirDebug){
						printf("iteracion=%d, Error elevado: %d\n",i,err);
					}
					i--;
				}else{
					int visibles = 0;
					for( int k = 0; k < artoolkitData->config->marker_num; k++ ) {
						if (artoolkitData->config->marker[k].visible >= 0) {
							visibles++;
							printf("Iteracion %d, Marcador %d Visible\n",i,artoolkitData->config->marker[k].patt_id);
						}
					}
					if(visibles!=detectados[i]){
						printf("ERRORRRRRRRRRRR!!! visibles=%d, detectados=%d\n",visibles,detectados[i]);
					}
				}
				*/

			}
		}
	}


	//Calculo de promedios
	*promedioDetectados = calcularPromedio(detectados,cant_iteraciones);
	for(int i=0;i<artoolkitData->config->marker_num;i++){
		cfMarcadores[i] = calcularPromedio(cf[i],cant_iteraciones);
	}

	if(imprimirDebug){
		printf("--> Thresh= %d, Iteraciones = %d, promedio de marcadores detectados = %f \n",thresh,cant_iteraciones,*promedioDetectados);
		printf("    Cf promedio para cada marcador: \n");
		for(int i=0;i<artoolkitData->config->marker_num;i++){
			printf("    -cf marcador %d = %f \n",i,cfMarcadores[i]);
		}
	}
}

//TODO Sin terminar. No habia grandes diferencias entre el marker_num del detectMarker y la cantidad de visibles asi que no es
//importante tener esta funcion
void calibrarThreshPorVisibilidad(ArtoolkitData* artoolkitData,int intervaloDescanso, int cant_iteraciones,double *promedioDetectados, double *cfMarcadores,int thresh,int imprimirDebug){

	ARUint8         *dataPtr = artoolkitData->dataPtr;
	int marker_num = 0;
	ARMarkerInfo    *marker_info;
	int detectados[cant_iteraciones];
	double cf[artoolkitData->config->marker_num][cant_iteraciones];


	//Inicializo cf en 0
	for(int i=0;i<artoolkitData->config->marker_num;i++){
		for(int j=0;j<cant_iteraciones;j++){
			cf[i][j] = 0;
		}
	}

	for (int i=0;i<cant_iteraciones;i++){
		arUtilSleep(90000);
		if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) { //Si no obtengo la imagen entonces no avanzo en la i
			i--;
		}else{
			int res = -1;
			if(artoolkitData->usarAlgoritmoLite){
				res = arDetectMarkerLite(dataPtr, thresh, &marker_info, &marker_num);
			}else{
				res = arDetectMarker(dataPtr, thresh, &marker_info, &marker_num);
			}
			if( res < 0 ) {
				if(imprimirDebug){
					if(artoolkitData->usarAlgoritmoLite){
						printf("iteracion=%d, error arDetectMarkerLite\n",i);
					}else{
						printf("iteracion=%d, error arDetectMarker\n",i);
					}
				}
				i--;
			}else{
				int err = 0;
				if( (err=arMultiGetTransMat(marker_info, marker_num,artoolkitData->config)) < 0 ) {
						if(imprimirDebug){
							printf("iteracion=%d, error arMultiGetTransMat\n",i);
						}
						i--;
				}else if(err > 100.0){
					if(imprimirDebug){
						printf("iteracion=%d, Error elevado: %d\n",i,err);
					}
					i--;
				}else{

				}

				detectados[i]=0;
				for(int k=0;k<artoolkitData->config->marker_num;k++){ //Para cada marcador voy a preguntar si esta visible
					if(artoolkitData->config->marker[k].visible){
						//TODO es visible, lo puedo contar como detectado pero existe la posibilidad de que no pueda acceder al cf

					/*	codigo de calibrar thresh comun
							 * for(int j=0;j<marker_num;j++){		//Recorro todos los marcadores detectados y guardo el cf si el id coincide con artoolkitData->config->marker[k].patt_id (es necesario verificar que el id coincida porque a veces ar devuelve -1)
								if(marker_info[j].id==artoolkitData->config->marker[k].patt_id){ //Detecto el marcador k
									cf[marker_info[j].id][i] = marker_info[j].cf;
									if(imprimirDebug){
										printf("Iteracion %d --> pattId=%d, cf=%f\n",i,marker_info[j].id,marker_info[j].cf);
									}
									detectados[i]++;
									break;
								}
							}
					 * */
					}

				}

			}
		}
	}


	//Calculo de promedios
	*promedioDetectados = calcularPromedio(detectados,cant_iteraciones);
	for(int i=0;i<artoolkitData->config->marker_num;i++){
		cfMarcadores[i] = calcularPromedio(cf[i],cant_iteraciones);
	}

	if(imprimirDebug){
		printf("--> Thresh= %d, Iteraciones = %d, promedio de marcadores detectados = %f \n",thresh,cant_iteraciones,*promedioDetectados);
		printf("    Cf promedio para cada marcador: \n");
		for(int i=0;i<artoolkitData->config->marker_num;i++){
			printf("    -cf marcador %d = %f \n",i,cfMarcadores[i]);
		}
	}
}



/**
 * Retorna 1 si la configuracion 1 es mejor que la 2
 * Retorna 2 si la configuracion 2 es mejor que la 1
 * Retorna 0 en caso de que sean iguales
 */
int arEsMejorConfiguracion(ARMultiMarkerInfoT *config,double promedioDetectados1, double * cfMarcadores1,double promedioDetectados2, double *cfMarcadores2,int largoCfs, double minimaDiferenciaCf,double minimaDiferenciaDetectados){

	double promedioCf1 = calcularPromedio(cfMarcadores1,largoCfs);
	double promedioCf2 = calcularPromedio(cfMarcadores2,largoCfs);

	if( (config->marker_num != round(promedioDetectados1) && config->marker_num != round(promedioDetectados2)) || //Ninguna configuracion detecta todos los marcadores O
		(config->marker_num == round(promedioDetectados1) && config->marker_num == round(promedioDetectados2))){  //Ambas configuraciones detectan todos los marcadores

		//Primero pregunto cual detecta mas marcadores
		if(promedioDetectados1 -promedioDetectados2>minimaDiferenciaDetectados){
			return 1;
		}else if(promedioDetectados2 -promedioDetectados1>minimaDiferenciaDetectados){
			return 2;
		}
		//En caso de que los dos detecten los mismos marcadores preguntos por los Cf
		if(promedioCf1 -promedioCf2 > minimaDiferenciaCf){
			return 1;
		}else if(promedioCf2 -promedioCf1 > minimaDiferenciaCf){
			return 2;
		}else{
			return 0;
		}
	}else if(config->marker_num == round(promedioDetectados1)){
		return 1;
	}else if(config->marker_num == round(promedioDetectados2)){
		return 2;
	}else{
		return -1; //Error, no puede alcanzar este resulatado
	}
}

void Pui::arCalibrarThresholdMultiMarker(ArtoolkitData* artoolkitData,int intervaloDescanso, int cant_iteraciones,int imprimirDebug){

	int thresh = artoolkitData->thresh;
	int deltaThreshDer = 15;
	int deltaThreshIzq = 15;
	int finCalibracion = 0;
	int nroCalibracion = 1;
	double promedioDetectados;
	double* cfMarcadores = new double[artoolkitData->config->marker_num];

	double promedioDetectados1;
	int thresh1;
	double* cfMarcadores1 = new double[artoolkitData->config->marker_num];

	if(imprimirDebug){
		printf("Comienza calibracion\n");
		printf("-->Calibracion nro %d\n",nroCalibracion);
		nroCalibracion++;
	}


	calibrarThresh(artoolkitData, intervaloDescanso, cant_iteraciones, &promedioDetectados,cfMarcadores, thresh,imprimirDebug);

	while(!finCalibracion){
		thresh1 = thresh + deltaThreshDer;

		if(imprimirDebug){
			printf("-->Calibracion nro %d\n",nroCalibracion);
			nroCalibracion++;
		}
		calibrarThresh(artoolkitData, intervaloDescanso, cant_iteraciones, &promedioDetectados1,cfMarcadores1, thresh1,imprimirDebug);
		int mejorConfiguracion = arEsMejorConfiguracion(artoolkitData->config,promedioDetectados, cfMarcadores, promedioDetectados1, cfMarcadores1,cant_iteraciones,0.5,0.5);

		if(mejorConfiguracion==1 || mejorConfiguracion==0){ //Prueba con el valor thresh - deltaTresh
			thresh1 = thresh - deltaThreshIzq;

			if(imprimirDebug){
				printf("-->Calibracion nro %d\n",nroCalibracion);
				nroCalibracion++;
			}
			calibrarThresh(artoolkitData, intervaloDescanso, cant_iteraciones, &promedioDetectados1,cfMarcadores1, thresh1,imprimirDebug);
			mejorConfiguracion = arEsMejorConfiguracion(artoolkitData->config,promedioDetectados, cfMarcadores, promedioDetectados1, cfMarcadores1,cant_iteraciones,0.5,0.5);

			if(mejorConfiguracion==1 || mejorConfiguracion==0){ //Termine la calibracion. Ni (thresh + deltaThresh) ni (thresh - deltaThresh) es mejor que el thresh.
				this->dataCalibracionArtoolkit.threshCalculado = thresh;
				this->dataCalibracionArtoolkit.cf = cfMarcadores;
				finCalibracion = 1;
			}else if(mejorConfiguracion==2){	//Me quedo con thresh - deltaThresh
				printf("El thresh %d es mejor que el thresh %d\n",thresh1,thresh);
				thresh = thresh1;
				deltaThreshDer = round(deltaThreshDer/2);
				promedioDetectados = promedioDetectados1;
				cfMarcadores = cfMarcadores1;
			}else{ //Error
				fprintf(stderr, "[arCalibrarThresholdMultiMarker]: Error calibrando!!!\n");
				finCalibracion=1;
			}
		}else if(mejorConfiguracion==2){		//Me quedo con thresh + deltaThresh
			printf("El thresh %d es mejor que el thresh %d\n",thresh1,thresh);
			thresh = thresh1;
			deltaThreshIzq = round(deltaThreshIzq/2);
			promedioDetectados = promedioDetectados1;
			cfMarcadores = cfMarcadores1;
		}else{ //Error
			fprintf(stderr, "[arCalibrarThresholdMultiMarker]: Error calibrando!!!\n");
			finCalibracion=1;
		}
	}
	this->dataCalibracionArtoolkit.threshCalculado = thresh;
	this->dataCalibracionArtoolkit.cf = cfMarcadores;

	if(imprimirDebug){
		printf("Fin de la calibracion\n");
	}
}

void Pui::calibrarThreshold(int cantidad_iteraciones){
	printf("Antes de calibrar threshold\n");
	arCalibrarThresholdMultiMarker(&this->dataArtoolkit,1000,cantidad_iteraciones,1);
	printf("Nuevo threshold calculado = %d\n",this->dataArtoolkit.thresh);
	this->dataArtoolkit.thresh = this->dataCalibracionArtoolkit.threshCalculado;
	printf("Resultados de la calibracion, nuevo t = %d\n",this->dataCalibracionArtoolkit.threshCalculado);
}


void Pui::setHorizontalFlip(int habilitado, char * device){
	int fd = open(device, O_RDWR);
	int ret;
	struct v4l2_control ctrl;

	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.id = V4L2_CID_HFLIP;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
	printf("Get H flip, ret %d value %d\n", ret, ctrl.value);
	ctrl.value = habilitado;
	ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
	printf("Set to %d, ret %d\n", ctrl.value, ret);
}

void Pui::setBrillo(int brillo, char * device){
	int fd = open(device, O_RDWR);
	int ret;
	struct v4l2_control ctrl;

	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.id = V4L2_CID_BRIGHTNESS;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
	printf("Get bright, ret %d value %d\n", ret, ctrl.value);
	ctrl.value = brillo;
	ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
	printf("Set to %d, ret %d\n", ctrl.value, ret);

}

void Pui::setAutoGain(int habilitado, char * device){
	int fd = open(device, O_RDWR);
	int ret;
	struct v4l2_control ctrl;

	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.id = V4L2_CID_AUTOGAIN;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
	printf("Get Auto white balance, ret %d value %d\n", ret, ctrl.value);

	ctrl.value = habilitado;
	ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
	printf("Set to %d, ret %d\n", ctrl.value, ret);

}


void Pui::setAutoWhiteBalance(int habilitado, char * device){
	int fd = open(device, O_RDWR);
	int ret;
	struct v4l2_control ctrl;

	memset(&ctrl, 0, sizeof(ctrl));
	ctrl.id = V4L2_CID_AUTO_WHITE_BALANCE;
	ret = ioctl(fd, VIDIOC_G_CTRL, &ctrl);
	printf("Get Auto white balance, ret %d value %d\n", ret, ctrl.value);

	ctrl.value = habilitado;
	ret = ioctl(fd, VIDIOC_S_CTRL, &ctrl);
	printf("Set to %d, ret %d\n", ctrl.value, ret);

}


void Pui::setearTodosLosMarcadoresNoVisibles(){
	for( int i = 0; i < this->dataArtoolkit.config->marker_num; i++ ) {
		this->dataArtoolkit.coordenadas_marcadores[i].area = 0;
		//Seteo centro
		this->dataArtoolkit.coordenadas_marcadores[i].centro[0]= -1;
		this->dataArtoolkit.coordenadas_marcadores[i].centro[1]= -1;
		//Seteo vertices
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[0][0] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[0][1] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[1][0] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[1][1] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[2][0] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[2][1] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[3][0] = -1;
		this->dataArtoolkit.coordenadas_marcadores[i].vertex[3][1] = -1;

		this->dataArtoolkit.coordenadas_marcadores[i].cf = 0;
		this->dataArtoolkit.coordenadas_marcadores[i].visible = 0;
	}
}
/*Devuelve la informacion de todos los marcadores
 * Tanto los marcadores  que no figuran en la estructura de dectados pero estan como visible o aquellos
 * que directamente estan como no visibles tendran su area y cf en -1 */
int Pui::detectarYCalcularPosicionTodosMultiMarkers(){
	int res;
	double err;
	if(count<INT_MAX)
		count++;
	else{
		count=0;
		arUtilTimerReset();
	}
	if(this->dataArtoolkit.dataPtr==NULL){
		if(debugMode){
			printf("data->dataPtr es NULL\n");
		}
		this->setearTodosLosMarcadoresNoVisibles();
		return ERROR_IMAGEN_NULL;
	}

	/*------------------- DETECCION DE LOS MARCADORES -----------------------------*/
	if(this->dataArtoolkit.usarAlgoritmoLite){
		res = arDetectMarkerLite(this->dataArtoolkit.dataPtr, this->dataArtoolkit.thresh, &this->dataArtoolkit.marker_info, &this->dataArtoolkit.marker_num);
	}else{
		res = arDetectMarker(this->dataArtoolkit.dataPtr, this->dataArtoolkit.thresh, &this->dataArtoolkit.marker_info, &this->dataArtoolkit.marker_num);
	}

	if( res < 0 ) {
		this->setearTodosLosMarcadoresNoVisibles();
		return ERROR_DETECTANDO_MARCADORES;
	}else{

		/*----------------------------CALCULO POSICIONES RELATIVAS DE LOS MARCADORES------------------------------*/
		if( (err=arMultiGetTransMat(this->dataArtoolkit.marker_info, this->dataArtoolkit.marker_num,this->dataArtoolkit.config)) < 0 ) {
			if(debugMode){
				printf("detectarMultiMarkers: Error obteniendo matriz de transformacion\n");
			}
			this->setearTodosLosMarcadoresNoVisibles();
			return ERROR_CALCULANDO_MATRIZ_TRANSFORMACION;
		}else if(err > 100.0){
			if(debugMode){
				printf("detectarMultiMarkers: Error elevado al calcular matriz de transformacion\n");
			}
			this->setearTodosLosMarcadoresNoVisibles();
			return ERROR_CALCULANDO_MATRIZ_TRANSFORMACION;
		}else if(this->dataArtoolkit.marker_num>0){

			/*for( int i = 0; i < data->config->marker_num; i++ ) {
				if(data->config->marker[i].visible>=0){
					printf("&&&Marcador %d visible (data->config->marker[%d]) . bandera=%d\n",data->config->marker[i].patt_id,i,data->config->marker[i].visible);
				}else{
					printf("&&&Marcador %d NO visible (data->config->marker[%d]) . bandera=%d\n",data->config->marker[i].patt_id,i,data->config->marker[i].visible);
				}
			}*/

			for( int i = 0; i < this->dataArtoolkit.config->marker_num; i++ ) {
				this->dataArtoolkit.coordenadas_marcadores[i].id = this->dataArtoolkit.config->marker[i].patt_id;
				if( (this->dataArtoolkit.config->marker[i].visible >= 0) ){			//Esta como visible pero muchas veces el id en el marker_info es -1. Por lo tanto lo que hago es buscarlo en el
																		//marker_info. Si lo encuentro entonces uso las coordenadas alli indicadas, sino las calculo como si no estuviera visible
					this->dataArtoolkit.coordenadas_marcadores[i].visible =1;
					//data->coordenadas_marcadores[i].id = data->config->marker[i].patt_id;
					int encontre=0;
					for(int j=0;(j<this->dataArtoolkit.marker_num)&&(!encontre);j++){
						if(this->dataArtoolkit.marker_info[j].id == this->dataArtoolkit.config->marker[i].patt_id){ //lo encontre en marker_info
							this->dataArtoolkit.coordenadas_marcadores[i].area = this->dataArtoolkit.marker_info[j].area;
							//Seteo centro
							this->dataArtoolkit.coordenadas_marcadores[i].centro[0]= this->dataArtoolkit.marker_info[j].pos[0];
							this->dataArtoolkit.coordenadas_marcadores[i].centro[1]= this->dataArtoolkit.marker_info[j].pos[1];
							//Seteo vertices
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[0][0] = this->dataArtoolkit.marker_info[j].vertex[0][0];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[0][1] = this->dataArtoolkit.marker_info[j].vertex[0][1];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[1][0] = this->dataArtoolkit.marker_info[j].vertex[1][0];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[1][1] = this->dataArtoolkit.marker_info[j].vertex[1][1];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[2][0] = this->dataArtoolkit.marker_info[j].vertex[2][0];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[2][1] = this->dataArtoolkit.marker_info[j].vertex[2][1];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[3][0] = this->dataArtoolkit.marker_info[j].vertex[3][0];
							this->dataArtoolkit.coordenadas_marcadores[i].vertex[3][1] = this->dataArtoolkit.marker_info[j].vertex[3][1];

							this->dataArtoolkit.coordenadas_marcadores[i].cf = this->dataArtoolkit.marker_info[j].cf;
							encontre =1;
						}

					}
					if(!encontre){ //No lo encontre en marker_info, lo calculo como si fuera no visible
						double hXi_hYi[3][4];
						obtenerCoordenadasIdealesMarcadorNoDetectado(this->dataArtoolkit.config->trans,this->dataArtoolkit.config->marker[i],hXi_hYi,this->dataArtoolkit.coordenadas_marcadores[i].vertex);

						double Xi,Yi;
						if(hXi_hYi[2][3]!=0){ //TODO que pasa si h==0?
							this->dataArtoolkit.coordenadas_marcadores[i].centro[0] = hXi_hYi[0][3]/hXi_hYi[2][3];
							this->dataArtoolkit.coordenadas_marcadores[i].centro[1] = hXi_hYi[1][3]/hXi_hYi[2][3];
						}else{
							this->dataArtoolkit.coordenadas_marcadores[i].centro[0] =  hXi_hYi[0][3];
							this->dataArtoolkit.coordenadas_marcadores[i].centro[1] =  hXi_hYi[1][3];
						}
						this->dataArtoolkit.coordenadas_marcadores[i].cf = -1;
						this->dataArtoolkit.coordenadas_marcadores[i].area = -1;

					}
				}else {
					this->dataArtoolkit.coordenadas_marcadores[i].visible =0;
					/*
					 *		MARCADOR NO DETECTADO.
					 *									* TENEMOS SUS COORDENADAS
					 *														- Marker Coordinates (Xm,Ym)
					 *														- Camera Coordinates (Xc,Yc)
					 *									* NECESITAMOS SUS COORDENADAS
					 *														- Ideal Screen Coordinates  (xc,yc)
					 *
					 */
					double hXi_hYi[3][4];
					obtenerCoordenadasIdealesMarcadorNoDetectado(this->dataArtoolkit.config->trans,this->dataArtoolkit.config->marker[i],hXi_hYi,this->dataArtoolkit.coordenadas_marcadores[i].vertex);

					double Xi,Yi;
					if(hXi_hYi[2][3]!=0){ //TODO que pasa si h==0?
						this->dataArtoolkit.coordenadas_marcadores[i].centro[0] = hXi_hYi[0][3]/hXi_hYi[2][3];
						this->dataArtoolkit.coordenadas_marcadores[i].centro[1] = hXi_hYi[1][3]/hXi_hYi[2][3];
					}else{
						this->dataArtoolkit.coordenadas_marcadores[i].centro[0] =  hXi_hYi[0][3];
						this->dataArtoolkit.coordenadas_marcadores[i].centro[1] =  hXi_hYi[1][3];
					}
					this->dataArtoolkit.coordenadas_marcadores[i].cf = -1;
					this->dataArtoolkit.coordenadas_marcadores[i].area = -1;
					this->dataArtoolkit.coordenadas_marcadores[i].id = this->dataArtoolkit.config->marker[i].patt_id;

				}
			}
		}else{ //Ningun marcador detectado
			this->setearTodosLosMarcadoresNoVisibles();
			return ERROR_NINGUN_MARCADOR_DETECTADO;
		}
	}

	return DETECCION_CORRECTA;
}


int Pui::detectarMarcadoresSimple(){
	int res;
	double err;
	if(count<INT_MAX)
		count++;
	else{
		count=0;
		arUtilTimerReset();
	}
	this->multimodalInfo.idSimpleMarcadorDetectado=-1;
	this->dataArtoolkit.coordenadas_marcadores[0].area = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].centro[0] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].centro[1] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].cf = 0;
	this->dataArtoolkit.coordenadas_marcadores[0].id = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[0][0] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[0][1] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[1][0] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[1][1] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[2][0] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[2][1] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[3][0] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].vertex[4][1] = -1;
	this->dataArtoolkit.coordenadas_marcadores[0].visible =0;
	if(this->dataArtoolkit.dataPtr==NULL){
		if(debugMode){
			printf("data->dataPtr es NULL\n");
		}
		return ERROR_IMAGEN_NULL;
	}
	/*------------------- DETECCION DE LOS MARCADORES -----------------------------*/
	if(this->dataArtoolkit.usarAlgoritmoLite){
		res = arDetectMarkerLite(this->dataArtoolkit.dataPtr, this->dataArtoolkit.thresh, &this->dataArtoolkit.marker_info, &this->dataArtoolkit.marker_num);
	}else{
		res = arDetectMarker(this->dataArtoolkit.dataPtr, this->dataArtoolkit.thresh, &this->dataArtoolkit.marker_info, &this->dataArtoolkit.marker_num);
	}

	if( res < 0 ) {
		return ERROR_DETECTANDO_MARCADORES;
	}else{
		if(this->dataArtoolkit.marker_num>0){
			//idMarcadorDetectado
			double max_cf =0;
			int idDetectado = -1;
			int index = -1;
			for( int i = 0; i < this->dataArtoolkit.marker_num; i++ ) {
				if(this->dataArtoolkit.marker_info[i].id>=0 && this->dataArtoolkit.marker_info[i].cf>max_cf
					&& this->dataArtoolkit.marker_info[i].cf>=this->dataArtoolkit.cf ){
					max_cf = this->dataArtoolkit.marker_info[i].cf;
					idDetectado =this->dataArtoolkit.marker_info[i].id;
					index=i;
				}else if(this->debugMode){
					if(this->dataArtoolkit.marker_info[i].id>=0){//Imprimo el marcador descartado
						printf("marcador %d descartado. CF = %f maxCF=%f\n ",this->dataArtoolkit.marker_info[i].id,this->dataArtoolkit.marker_info[i].cf,max_cf);
					}
				}
				if(idDetectado>=0 ){
					int cant =0;
					if(this->historicoHabilitado){
						cant = registrarColision(i);
					}
					if(cant>=this->historico.cantHistorico || !this->historicoHabilitado){
						this->multimodalInfo.idSimpleMarcadorDetectado = idDetectado;
						this->dataArtoolkit.coordenadas_marcadores[0].area = this->dataArtoolkit.marker_info[index].area;
						this->dataArtoolkit.coordenadas_marcadores[0].centro[0] = this->dataArtoolkit.marker_info[index].pos[0];
						this->dataArtoolkit.coordenadas_marcadores[0].centro[1] = this->dataArtoolkit.marker_info[index].pos[1];
						this->dataArtoolkit.coordenadas_marcadores[0].cf = this->dataArtoolkit.marker_info[index].cf;
						this->dataArtoolkit.coordenadas_marcadores[0].id = this->multimodalInfo.idSimpleMarcadorDetectado;
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[0][0] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[0][1] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[1][0] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[1][1] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[2][0] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[2][1] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[3][0] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].vertex[4][1] = this->dataArtoolkit.marker_info[index].vertex[0][0];
						this->dataArtoolkit.coordenadas_marcadores[0].visible = 1; //En modo MODO_SIMPLE la informacion siempre corresponde a un marcador visible
																					//porque fue detectado
					}else if(this->debugMode){
						printf("Deteccion nro %d con marcador %d\n",cant,i);
					}
				}
			}
		}else{ //Ningun marcador detectado
			return ERROR_NINGUN_MARCADOR_DETECTADO;
		}
	}
	return DETECCION_CORRECTA;
}

/**
 * Retorna el id del marcador y objeto PUI detectado en la ultima invocacion
 * a la funcion Pui::detectarMarcadoresSimple
 */
int Pui::getIdMarcadorSimpleDetectado(){
	if(this->multimodalInfo.modo==MODO_SIMPLE){
		return this->multimodalInfo.idSimpleMarcadorDetectado;
	}else{
		fprintf(stderr, "Error, el operacion soporta solo en modo MODO_SIMPLE\n");
		return -1;
	}
}

/**
 * Retorna la informacion correspondiente al marcador detectado en la ultima invocacion
 * a la funcion Pui::detectarMarcadoresSimple.
 * El id del marcador es el mismo que el id del objeto PUI asociado.
 */
ARCoordMarcador Pui::getMarcadorSimpleDetectado(){
	if(this->multimodalInfo.modo==MODO_SIMPLE){
		return this->dataArtoolkit.coordenadas_marcadores[0];
	}else{
		fprintf(stderr, "Error, el operacion soporta solo en modo MODO_SIMPLE\n");
		ARCoordMarcador resVacio;
		resVacio.id=-1;
		resVacio.area=-1;
		resVacio.cf=0;
		resVacio.visible=0;
		return resVacio;
	}
}

/**
	 * Retorna la información contenida en la estructura ARCoordMarcador para el marcador de id idMarcador
	 * Solo se utiliza en el caso de modo MODO_MULTI_MARKERS.
	 * Siempre se dispone de cada propiedad de la estructura ARCoordMarcador aunque el marcador no
	 * haya sido detectado (su area, coordenadas y verticas son calculados a partir de la informacion
	 * de aquellos marcadores detectados ) a excepcion de la propiedad cf que no tiene sentido en caso
	 * de que el marcador no haya sido detectado.
	 *
	 * @param idMarcador Identificador del marcador
	 */
ARCoordMarcador Pui::getInfoMultiMarcador(int idMarcador){
	if(this->multimodalInfo.modo==MODO_MULTI_MARKERS){
		if(idMarcador<=this->dataArtoolkit.config->marker_num){
			return this->dataArtoolkit.coordenadas_marcadores[idMarcador];
		}else{
			fprintf(stderr, "Error, id %d del marcador fuera de rango\n",idMarcador);
			ARCoordMarcador resVacio;
			resVacio.id=-1;
			resVacio.area=-1;
			resVacio.cf=0;
			resVacio.visible=0;
			return resVacio;
		}
	}else{
		fprintf(stderr, "Error, el operacion soporta solo en modo MODO_MULTI_MARKERS\n");
		ARCoordMarcador resVacio;
		resVacio.id=-1;
		resVacio.area=-1;
		resVacio.cf=0;
		resVacio.visible=0;
		return resVacio;
	}
}

void Pui::setARCf(double cf){
	this->dataArtoolkit.cf = cf;
}

double Pui::getARCf(){
	return this->dataArtoolkit.cf;
}

int Pui::detectarMarcadores(){
	if(this->multimodalInfo.modo == MODO_MULTI_MARKERS){
		return this->detectarYCalcularPosicionTodosMultiMarkers();
	}else if(this->multimodalInfo.modo == MODO_SIMPLE){
		return this->detectarMarcadoresSimple();
	}else{ //Modo incorrecto?
		return ERROR_DETECTANDO_MARCADORES;
	}
}

bool Pui::hayImagen(){
	/*current_time = time.time()
    time_elapsed = (float)(current_time - self.video_time_prev)

    #  Don't update video more often than 30 frame per second.
    if not (time_elapsed < 1./30.) and self.playOn:
        self.video_time_prev = current_time
        self.lock.acquire()
        # get a new frame from video device
        self.arVideoGetImage()
        # increase number of frame grab
        self.frame_counter = self.frame_counter +1
        # set image has been updated
        self.image_has_update = True
        self.lock.release()

    else:
        time.sleep(0.01)  # we pause the thread to let other thread runs
	 */
	elapsed =timeUtils->utilTimerGetElapsedTimeFromLast() + elapsed;
	if(elapsed>this->dataArtoolkit.frameRateInterval){
		elapsed=0;
		return true;
	}else{
		return false;
	}
	//return timeUtils->utilTimerGetElapsedTimeFromLast() < this->dataArtoolkit.frameRateInterval;
}
binary_data Pui::capturarPyImageAR(){
	unsigned char* dataImg = capturarImagenAR();
	if (dataImg!=NULL){
		printf("antes de copiar. length=%d\n",length);
		memcpy(data,dataImg,length);
		result.data=data;
		printf("luego de copiar\n");
	}else{
		result.data = NULL;
	}
	result.size = result.data==NULL?0:length;
	return result;
}

unsigned char* Pui::capturarImagenAR(){
	if(!hayImagen()){
		//arUtilSleep(20);
		usleep(sleepInterval);
		//sleep(9);
		return NULL;
	}else if( (this->dataArtoolkit.dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
		arUtilSleep(2);
		return NULL;
	}
	this->count++;
	return (unsigned char*)this->dataArtoolkit.dataPtr;
}


unsigned char* Pui::obtenerImagenDebugAR(){
	return arImage;
}


//Funcion de uso interno del calibrador
void Pui::initImagenesCalibrador(ARUint8* dataPtr,int width, int height){

	//imageColor = crearImagenCv2((uchar*)dataPtr,width,height);
	imageColor = crearImagenCv3(width,height);
	imageHsv = cvCreateImage( cvGetSize(imageColor), 8, 3 );//convertImageRGBtoHSV(imageColor);

	//Inicializacion de los planos h,s,v
	h_plane = cvCreateImage( cvGetSize(imageColor), 8, 1 );
	s_plane = cvCreateImage( cvGetSize(imageColor), 8, 1 );
	v_plane = cvCreateImage( cvGetSize(imageColor), 8, 1 );

	cvZero(h_plane);
	cvZero(s_plane);
	cvZero(v_plane);

	mascaraBinaria = cvCreateImage(cvGetSize(imageColor),imageColor->depth,1);
	cvZero(mascaraBinaria);

	mascaraBinariaInvertida = cvCreateImage(cvGetSize(imageColor),imageColor->depth,1);
	cvZero(mascaraBinariaInvertida);

}

void Pui::initPropiedadesCalibracionSoloApuntador(int forma,RangosColorApuntador rangos_escala,int h_bins, int s_bins, int v_bins,int debug){
		this->debugMode =debug;
		this->propiedades.forma = forma;
		//this->propiedades.rangos = rangos_escala;
		this->propiedades.resultado_rangos_color= rangos_escala;

			/*Inicializacion de histogramas*/
		//Seteo de los rangos de la escala hsv utilizada
		this->propiedades.data_histogramas.rangos = rangos_escala;

		//Seteo la cantidad de intervalos en los que se van a dividir los histogramas
		this->propiedades.data_histogramas.h_bins = h_bins;
		this->propiedades.data_histogramas.s_bins = s_bins;
		this->propiedades.data_histogramas.v_bins = v_bins;

			/*Inicializo los histogramas*/
		h_ranges[0] = this->propiedades.data_histogramas.rangos.hmin;
		h_ranges[1] = this->propiedades.data_histogramas.rangos.hmax;
		this->propiedades.data_histogramas.histograma_h = crearHistograma1D(h_ranges,h_bins);

		s_ranges[0] = this->propiedades.data_histogramas.rangos.smin;
		s_ranges[1] = this->propiedades.data_histogramas.rangos.smax;
		this->propiedades.data_histogramas.histograma_s = crearHistograma1D(s_ranges,s_bins);

		v_ranges[0] = this->propiedades.data_histogramas.rangos.vmin;
		v_ranges[1] = this->propiedades.data_histogramas.rangos.vmax;
		this->propiedades.data_histogramas.histograma_v = crearHistograma1D(v_ranges,v_bins);

		initImagenesCalibrador(this->dataArtoolkit.dataPtr,this->dataArtoolkit.xsize,this->dataArtoolkit.ysize);

		//this->propiedades.tomar_muestra = 0;
		//this->propiedades.aplicar_mascara = 0;
		this->propiedades.debug_imagenes = 0;
		//this->propiedades.aplicar_nueva_mascara= 0;
		this->propiedades.restarAlRadio =0;
}


void Pui::initPropiedadesCalibracionSoloApuntador(int forma,int hmin,int hmax, int smin, int smax, int vmin, int vmax,int h_bins, int s_bins, int v_bins,int debug ){
	RangosColorApuntador rangosEscalaColor;
	rangosEscalaColor.hmin = hmin;
	rangosEscalaColor.hmax = hmax;
	rangosEscalaColor.smin = smin;
	rangosEscalaColor.smax = smax;
	rangosEscalaColor.vmin = vmin;
	rangosEscalaColor.vmax = vmax;
	this->initPropiedadesCalibracionSoloApuntador(forma,rangosEscalaColor,h_bins,s_bins,v_bins,debug);
}



/*A partir de los marcadores detectados selecciona uno y aplica la mascara segun la forma seleccionada
Como resultado se deja en data->dataPtr la imagen con la mascara aplicada
En data se debe encontrar el centro y los vertices del marcador de identificador idMarcador*/
int Pui::aplicarNuevaMascaraCalibrador(int idMarcador,int tipoMascara){
	if(this->dataArtoolkit.marker_num<1){
			if(debugMode){
				printf("aplicarNuevaMascaraCalibrador: Error, no se detecto ningun marcador\n");
			}
			return MASCARA_NO_APLICADA;
		}

		if(this->dataArtoolkit.marker_num < ((this->dataArtoolkit.config->marker_num)/2 +1)){ //Se pide que al menos se hayan detectado la mitad mas un marcador
			if(debugMode){
				printf("aplicarNuevaMascaraCalibrador: Solamente hay %d en pantalla, como minimo debe haber %d\n",this->dataArtoolkit.marker_num ,(this->dataArtoolkit.config->marker_num)/2 +1);
			}
			return MASCARA_NO_APLICADA;
		}

		int indice = -1;
		for(int i=0;i<this->dataArtoolkit.config->marker_num;i++){
			if(this->dataArtoolkit.coordenadas_marcadores[i].id==idMarcador){
				indice =i;
				break;
			}
		}

		if(indice<0){
			if(debugMode){
				printf("aplicarNuevaMascaraCalibrador: Error, no se encontro el marcador con id = %d\n",idMarcador);
			}
			return MASCARA_NO_APLICADA;
		}

		if(this->propiedades.forma==FORMA_APUNTADOR_CIRCULO){
			int area = 0;
			double radio =0;
			printf("indice = %d\n",indice);
			if(this->dataArtoolkit.coordenadas_marcadores[indice].area>0){//Si el area ya esta calculada entonces no vuelvo a calcularla
				area = this->dataArtoolkit.coordenadas_marcadores[indice].area;
				printf("El area ya esta calculada y es  %d\n",area);
			}else{											//Cuando el marcador es no visible entonces el area no esta precalculada por ARtoolkit y es necesario calcularla
				area = aproximarArea(this->dataArtoolkit.coordenadas_marcadores[indice].vertex);
				printf("El area NO esta calculada. Su aproximacino es %d\n",area);
				if(area==0){
					printf("ERROR AREA 0! --> indice=%d --- visible=%d ---- v0=(%f,%f) - v1=(%f,%f) -v2=(%f,%f) -v3=(%f,%f) \n",indice,this->dataArtoolkit.coordenadas_marcadores[indice].visible,
							this->dataArtoolkit.coordenadas_marcadores[indice].vertex[0][0],this->dataArtoolkit.coordenadas_marcadores[indice].vertex[0][1],
							this->dataArtoolkit.coordenadas_marcadores[indice].vertex[1][0],this->dataArtoolkit.coordenadas_marcadores[indice].vertex[1][1],
							this->dataArtoolkit.coordenadas_marcadores[indice].vertex[2][0],this->dataArtoolkit.coordenadas_marcadores[indice].vertex[2][1],
							this->dataArtoolkit.coordenadas_marcadores[indice].vertex[3][0],this->dataArtoolkit.coordenadas_marcadores[indice].vertex[3][1]);
				}
			}

			//area = (radio)*(radio)*PI ==> radio = SQRT(area/PI)
			radio = sqrt(area/CV_PI) - this->propiedades.restarAlRadio;
			if(radio<=0){
				radio = 5;		//Tomamos 5 como el radio mas pequeno que podemos tener
			}

			//Dibujo el circulo sobre la mascara negra
			cvSet(mascaraBinaria,cvScalar(255),NULL);
			cvCircle(mascaraBinaria,
					cvPoint(this->dataArtoolkit.coordenadas_marcadores[indice].centro[0],this->dataArtoolkit.coordenadas_marcadores[indice].centro[1]),
					radio,CV_RGB(0,0,0),-1);

								/*******Guardo informacion de los circulos y el rectangulo que lo circunscribe*******/
			circle.centro.x = this->dataArtoolkit.coordenadas_marcadores[indice].centro[0];
			circle.centro.y = this->dataArtoolkit.coordenadas_marcadores[indice].centro[1];
			circle.radio = radio;

			int centro_x = cvRound(circle.centro.x - radio);
			int centro_y = cvRound(circle.centro.y - radio);

			rect.x = (centro_x)>0 ? (centro_x) : 0;
			rect.y = (centro_y)>0 ? (centro_y) : 0;
			rect.height = 2*radio;
			rect.width = rect.height;

			cvSetImageData( imageColor, (uchar*)(this->dataArtoolkit.dataPtr), imageColor->width* imageColor->nChannels);
			imageColor->origin = IPL_ORIGIN_TL;


			if(tipoMascara==TIPO_MASCARA_ATENUANTE){
				cvSubS(imageColor,cvScalar(120,120,120),imageColor,mascaraBinaria);  //--> Oscurece la zona del circulo pero no la deja negra
			}else if(tipoMascara==TIPO_MASCARA_BINARIA){
				cvXor(imageColor,imageColor,imageColor,mascaraBinaria); //->>El xor deja la imagen toda negra excepto el circulo
			}else{
				if(debugMode){
					printf("aplicarNuevaMascaraCalibrador: Error en el tipo de mascara especificada\n");
					return MASCARA_NO_APLICADA;
				}
			}
			//Dibujo rectangulo alrededor del circulo
			cvRectangle(imageColor,cvPoint(rect.x,rect.y),cvPoint(rect.x + rect.width,rect.y + rect.height),cvScalar(100,0,0,0),2,1,0);

			if(this->propiedades.debug_imagenes){
				cvNamedWindow("Mascara");
				cvShowImage("Mascara",mascaraBinaria);
				cvNamedWindow("Imagen enmascarada");
				cvShowImage("Imagen enmascarada",imageColor);
				cvWaitKey(20);
			}
			if(debugMode){
				printf("Nueva Mascara aplicada. Circle: centro> (%f,%f) radio> %f\n",this->dataArtoolkit.coordenadas_marcadores[indice].centro[0],
																			   this->dataArtoolkit.coordenadas_marcadores[indice].centro[1],radio);
			}

			return MASCARA_APLICADA;
		}else if(this->propiedades.forma==FORMA_APUNTADOR_MANO){
			fprintf(stderr, "aplicarNuevaMascaraCalibrador: NO existe implementacion de la mascara para la forma de la mano\n ");
			return MASCARA_NO_APLICADA;
		}else if(this->propiedades.forma==FORMA_APUNTADOR_RECTANGULAR){
			fprintf(stderr, "aplicarNuevaMascaraCalibrador: NO existe implementacion de la mascara para la forma rectangular\n ");
			return MASCARA_NO_APLICADA;
		}else{
			return MASCARA_NO_APLICADA;
		}
}

/*Solamente aplica una mascara previamente calculada por la funcion aplicarNuevaMascaraCalibrador
 * si antes no se llama al menos una vez a aplicarNuevaMascaraCalibrador entonces no se aplicara la mascara*/
int Pui::aplicarMascaraCalibrador(int tipoMascara){
	if(this->mascaraBinaria==NULL){
		if(this->debugMode){
			printf("aplicarMascaraCalibrador: La mascara es NULL, primero debe invocarse aplicarNuevaMascaraCalibrador \n");
		}
		return MASCARA_NO_APLICADA;
	}
	cvSetImageData( this->imageColor, (uchar*)(this->dataArtoolkit.dataPtr), this->imageColor->width* this->imageColor->nChannels);
	this->imageColor->origin = IPL_ORIGIN_TL;


	if(tipoMascara==TIPO_MASCARA_ATENUANTE){
		cvSubS(this->imageColor,cvScalar(120,120,120),this->imageColor,this->mascaraBinaria);  //--> Oscurece la zona del circulo pero no la deja negra
	}else if(tipoMascara==TIPO_MASCARA_BINARIA){
		cvXor(this->imageColor,this->imageColor,this->imageColor,this->mascaraBinaria); //->>El xor deja la imagen toda negra excepto el circulo
	}else{
		if(this->debugMode){
			printf("aplicarMascaraCalibrador: Error en el tipo de mascara especificada\n");
			return MASCARA_NO_APLICADA;
		}
	}

	//Dibujo rectangulo alrededor del circulo
	cvRectangle(this->imageColor,cvPoint(rect.x,rect.y),cvPoint(rect.x + rect.width,rect.y + rect.height),cvScalar(100,0,0,0),2,1,0);
	if(this->propiedades.debug_imagenes){
		cvNamedWindow("Mascara");
		cvShowImage("Mascara",this->mascaraBinaria);

		cvNot(mascaraBinaria,this->mascaraBinariaInvertida);
		cvNamedWindow("Mascara Invertida");
		cvShowImage("Mascara Invertida",this->mascaraBinariaInvertida);

		cvNamedWindow("Imagen enmascarada");
		cvShowImage("Imagen enmascarada",this->imageColor);
		cvWaitKey(5);
	}

	return MASCARA_APLICADA;
}


void Pui::setAchicarRadioMascaraCircular(int restarAlRadio){
	this->propiedades.restarAlRadio = restarAlRadio;
}

int Pui::getAchicarRadioMascaraCircular(){
	return this->propiedades.restarAlRadio;
}


unsigned char* Pui::getCalibrationImage(){
	if(this->imageColor!=NULL){
		return (unsigned char*)this->imageColor->imageData;
	}
}

/*Calcula el histograma filtrado por la mascara lo acumula al histograma calculado con las muestras
anteriores*/
int Pui::tomarMuestraApuntador(){
	cvSetImageData( this->imageColor, (uchar*)(this->dataArtoolkit.dataPtr), this->imageColor->width* this->imageColor->nChannels);
	this->imageColor->origin = IPL_ORIGIN_TL;

	if(this->propiedades.forma==FORMA_APUNTADOR_CIRCULO || this->propiedades.forma==FORMA_APUNTADOR_RECTANGULAR){
		rgb2hsv(this->imageColor,this->imageHsv,this->rect);

		//Invierto la mascara binaria. Originalmente esta toda blanca con el circulo negro. Para calcular el histograma la necesitamos exactamente al reves
		cvNot(this->mascaraBinaria,this->mascaraBinariaInvertida);
		cvSetImageROI(this->mascaraBinariaInvertida,this->rect);
		cvSetImageROI(this->imageHsv,this->rect);
		cvSetImageROI(this->h_plane,this->rect);
		cvSetImageROI(this->s_plane,this->rect);
		cvSetImageROI(this->v_plane,this->rect);
		cvSetImageROI(this->mascaraBinaria,this->rect);
		cvSetImageROI(this->imageColor,this->rect);



		if(this->propiedades.debug_imagenes){
			cvNamedWindow("ROI");
			cvShowImage("ROI",this->imageColor);

			cvNot(this->mascaraBinaria,this->mascaraBinariaInvertida);
			cvNamedWindow("HSV A Procesar");
			cvShowImage("HSV A Procesar",this->imageHsv);

			cvNamedWindow("Imagen enmascarada");
			cvShowImage("Imagen enmascarada",this->imageColor);
			cvWaitKey(5);
		}

		separarImagenEnPlanos(this->imageHsv, this->h_plane,this->s_plane,this->v_plane);

		calcularHistograma(this->propiedades.data_histogramas.histograma_h,this->h_plane,this->mascaraBinariaInvertida,this->propiedades.data_histogramas.h_bins,this->h_ranges,1,this->debugMode);
		calcularHistograma(this->propiedades.data_histogramas.histograma_s,this->s_plane,this->mascaraBinariaInvertida,this->propiedades.data_histogramas.s_bins,this->s_ranges,1,this->debugMode);
		calcularHistograma(this->propiedades.data_histogramas.histograma_v,this->v_plane,this->mascaraBinariaInvertida,this->propiedades.data_histogramas.v_bins,this->v_ranges,1,this->debugMode);
		if(this->debugMode){
			printf("Bins de histograma H\n");
			imprimirInfoHist(this->propiedades.data_histogramas.histograma_h,this->propiedades.data_histogramas.h_bins);
		}
		cvResetImageROI(this->imageColor);
		cvResetImageROI(this->imageHsv);
		cvResetImageROI(this->h_plane);
		cvResetImageROI(this->s_plane);
		cvResetImageROI(this->v_plane);
		cvResetImageROI(this->mascaraBinaria);
		cvResetImageROI(this->mascaraBinariaInvertida);

	}else{
		fprintf(stderr, "tomarMuestraApuntador: Error, forma no implementada\n");
		return ERROR_TOMAR_MUESTRA_FORMA_NO_IMPLEMENTADA;
	}
	return ERROR_TOMAR_MUESTRA_OK;

}

int thresholdHistograms(PropiedadesCalibracion* propiedades,int factor_proporcion_valor_maximo,int minPorcentajeRuido ){

	/*
	 * En el caso de los histogramas de Hue queremos buscar un pico que nos indique la tonalidad de
	 * color del objeto procesado. Buscando solamente donde se da el maximo del histograma tenemos una aproximacion
	 * demasiado grosera, probablemente alrededor del pico maximo haya algunos picos interesantes que tengamos que tener en cuenta.
	 * Para no descartarlos usamos un threshold proporcional al valor maximo del histograma.
	 * El threshold escodigo es max_val/factor_proporcion_valor_maximo , por lo tanto vamos a considerar hasta los picos que sean la factor_proporcion_valor_maximo-esima parte del maximo, el resto
	 * lo ponemos en 0.
	 * Como la representacion de la tonalidad H es continua es de esperarse que solamente queden algunos intervalos consecutivos en el histograma
	 * y el resto se vaya a 0. Estos intervalos son los que usaremos para calcular hmin y hmax, mas precisamente :
	 * hmin es el borde izq del primer intervalo distinto de 0
	 * hmax es el borde derecho del ultimo intervalo distinto de 0
	 * Si luego de hacer el threshold los intervalos distintos de 0 no son consecutivos entonces no podemos utilizar este histograma y probablemente
	 * se este procesando un objeto de varios colores.
	 * */

	if(propiedades->data_histogramas.histograma_h!=NULL){
		printf("***Threshold de Histograma H\n ***");
		float min_val_h,max_val_h;
		cvGetMinMaxHistValue(propiedades->data_histogramas.histograma_h, &min_val_h, &max_val_h, NULL, NULL);
		printf("   max_val_h = %f\n",max_val_h);
		double thresh_h = max_val_h/factor_proporcion_valor_maximo;
		printf("   Threshold H= %f\n",thresh_h);
		cvThreshHist(propiedades->data_histogramas.histograma_h,thresh_h);
	}else{
		return ERROR_HISTOGRAMA_NULL;
	}


	/*El total de pixeles computados en el histograma no tiene porque coincidir con el area de la imagen estudiada porque
	 * los pixeles negros NO son computados en el histograma.
	 * */
	int pixeles_h = getTotalBins1D(propiedades->data_histogramas.histograma_h,propiedades->data_histogramas.h_bins);
	int pixeles_s = getTotalBins1D(propiedades->data_histogramas.histograma_s,propiedades->data_histogramas.s_bins);
	int pixeles_v = getTotalBins1D(propiedades->data_histogramas.histograma_v,propiedades->data_histogramas.v_bins);

	printf("Total pixeles procesados hist_h = %d\n",pixeles_h);
	printf("Total pixeles procesados hist_s = %d\n",pixeles_s);
	printf("Total pixeles procesados hist_v = %d\n",pixeles_v);

	/* Calculo del threshold para eliminar ruido.
	 * Consideramos ruido a aquellos bins del histograma que tengan menos del 3% del total del mismo
	 * pixelesProcesados ---- 100%
	 * thresh			 ----   minPorcentajeRuido%
	 *
	 * thresh = 3*pixelesProcesados/100;
	 * */
	double thresh_ruido_s = minPorcentajeRuido*pixeles_s/100;
	printf("   Threshold S= %f\n",thresh_ruido_s);

	if(propiedades->data_histogramas.histograma_s!=NULL){
		printf("***Threshold de Histograma S\n ***");
		float min_val_s,max_val_s;
		cvGetMinMaxHistValue(propiedades->data_histogramas.histograma_s, &min_val_s, &max_val_s, NULL, NULL);
		printf("   max_val_s = %f\n",max_val_s);
		cvThreshHist(propiedades->data_histogramas.histograma_s,thresh_ruido_s);
	}




	/* Calculo del threshold para eliminar ruido.
	 * Consideramos ruido a aquellos bins del histograma que tengan menos del 3% del total del mismo
	 * pixelesProcesados ---- 100%
	 * thresh			 ----   minPorcentajeRuido%
	 *
	 * thresh = 3*pixelesProcesados/100;
	 * */
	double thresh_ruido_v = minPorcentajeRuido*pixeles_v/100;
	printf("   Threshold V= %f\n",thresh_ruido_v);

	if(propiedades->data_histogramas.histograma_v!=NULL){
		printf("***Threshold de Histograma V\n ***");
		float min_val_v,max_val_v;
		cvGetMinMaxHistValue(propiedades->data_histogramas.histograma_v, &min_val_v, &max_val_v, NULL, NULL);
		printf("   max_val_v = %f\n",max_val_v);
		cvThreshHist(propiedades->data_histogramas.histograma_v,thresh_ruido_v);
	}
}

int Pui::calcularResultadosCalibracionApuntador(int factorThreshH,int minPorcentajeRuido){
	if(this->debugMode){
		printf("****Bins de histograma H ANTES del threshold****\n");
		imprimirInfoHist(this->propiedades.data_histogramas.histograma_h,this->propiedades.data_histogramas.h_bins);

		printf("****Bins de histograma S ANTES del threshold****\n");
		imprimirInfoHist(this->propiedades.data_histogramas.histograma_s,this->propiedades.data_histogramas.s_bins);

		printf("****Bins de histograma V ANTES del threshold****\n");
		imprimirInfoHist(this->propiedades.data_histogramas.histograma_v,this->propiedades.data_histogramas.v_bins);
	}
	thresholdHistograms(&this->propiedades,factorThreshH,minPorcentajeRuido);
	if(debugMode){
		printf("Bins de histograma H LUEGO del threshold\n");
		imprimirInfoHist(this->propiedades.data_histogramas.histograma_h,this->propiedades.data_histogramas.h_bins);

		printf("****Bins de histograma S LUEGO del threshold****\n");
		imprimirInfoHist(this->propiedades.data_histogramas.histograma_s,this->propiedades.data_histogramas.s_bins);

		printf("****Bins de histograma V LUEGO del threshold****\n");
		imprimirInfoHist(this->propiedades.data_histogramas.histograma_v,this->propiedades.data_histogramas.v_bins);
	}


	double resultado_h[2],resultado_s[2],resultado_v[2];
	int res_h,res_s,res_v;


	//Calculo resultado para histograma H
	res_h = calcRangosHist(this->propiedades.data_histogramas.histograma_h,this->propiedades.data_histogramas.h_bins,this->h_ranges,1,resultado_h);

	//Calculo resultado para histograma S
	res_s =calcRangosHist(this->propiedades.data_histogramas.histograma_s,this->propiedades.data_histogramas.s_bins,this->s_ranges,0,resultado_s);

	//Calculo resultado para histograma V
	res_v =calcRangosHist(this->propiedades.data_histogramas.histograma_v,this->propiedades.data_histogramas.v_bins,this->v_ranges,0,resultado_v);

	if(res_h==ERROR_RANGOS_HISTOGRAMA_NO_CONTINUO){
		return res_h;
	}else if(res_s==ERROR_RANGOS_HISTOGRAMA_NO_CONTINUO){
		return res_s;
	}else if(res_v==ERROR_RANGOS_HISTOGRAMA_NO_CONTINUO){
		return res_v;
	}else{
		this->propiedades.resultado_rangos_color.hmin = resultado_h[0];
		this->propiedades.resultado_rangos_color.hmax = resultado_h[1];

		this->propiedades.resultado_rangos_color.smin = resultado_s[0];
		this->propiedades.resultado_rangos_color.smax = resultado_s[1];

		this->propiedades.resultado_rangos_color.vmin = resultado_v[0];
		this->propiedades.resultado_rangos_color.vmax = resultado_v[1];

		return ERROR_RANGOS_CALCULADOS_OK;
	}
}

int Pui::leerPatternsModoSimple(const char* archivoConfiguracion){
	FILE *fich;
	char c = '#';
	char str[500];
	int patts_id[200];
	fich=fopen(archivoConfiguracion,"r");
	int res = 1;
	int largo = 0;
	int id=-1;
	if(fich != NULL) {
		int i=0;
		while((c!=EOF)){
			if( fscanf(fich,"%c",&c) < 0){
				break;
			}
			if(c!='\n'){
				if(c=='#'){ //consumo la linea del comentario
					while( (c!='\n') && (c!=EOF)){
						fscanf(fich,"%c",&c);
					}
				}else{
					fseek(fich, -1, SEEK_CUR);
					fscanf(fich,"%s\n",str);
					printf("ruta .patt =%s\n",str);

					if ((patts_id[i] = arLoadPatt(str)) < 0) {
						fprintf(stderr, "error carcando .patt  %s en modo MODO_SIMPLE !!\n",str);
						return (0);
					}
					i++;
				}
			}
		}
		dataArtoolkit.patts_id = new int[i];
		for(int k=0;k<i;k++){
			this->dataArtoolkit.patts_id[k]=patts_id[k];  //Se suponen que los id siempre arrancan en 0 y van incrementando de a uno,
															//pero por las dudas copiamos los id que devuelve artoolkit al cargar los .patt
		}
		this->multimodalInfo.cant_elementos_cargados = i;
		printf("Se cargaron %d marcadores en MODO_SIMPLE\n",this->multimodalInfo.cant_elementos_cargados);
		fclose(fich);
		if(res>0){
			res = i; //Retorna la CANTIDAD de elementos, por eso hay que sumar 1
		}
	}else{
		printf("No se puede leer el archivo de configuracion %s\n",archivoConfiguracion);
	}
	this->multimodalInfo.cant_elementos_cargados = res;
	return res;
}

int Pui::cargarConfiguracionDeteccionApuntadorDesdeArchivo(char * fileName) {
	FILE *fich;
	char c;
	fich=fopen(fileName,"r");
	if(fich != NULL) {
		fscanf(fich," %c",&c);
		if(c=='#'){ //si el separador es correcto y no es fin de fichero carga valores;
			fscanf(fich,"%d",&(this->propiedades.resultado_rangos_color.hmin));
			fscanf(fich,"%d",&(this->propiedades.resultado_rangos_color.hmax));
			fscanf(fich,"%d",&(this->propiedades.resultado_rangos_color.smin));
			fscanf(fich,"%d",&(this->propiedades.resultado_rangos_color.smax));
			fscanf(fich,"%d",&(this->propiedades.resultado_rangos_color.vmin));
			fscanf(fich,"%d",&(this->propiedades.resultado_rangos_color.vmax));
			fscanf(fich,"%d",&(this->propiedadesDeteccion.min_area));
			fscanf(fich,"%f",&((this->propiedadesDeteccion.factorProporcionApuntadorMarcador)));
			fscanf(fich,"%d",&(this->propiedadesDeteccion.erode));
			fscanf(fich,"%d",&(this->propiedadesDeteccion.dilate));
			fscanf(fich,"%d",&(this->propiedadesDeteccion.smooth));
			fscanf(fich,"%d",&(this->propiedadesDeteccion.enclosing_circle));
		}
		fclose(fich);
		printf("*** Configuración HSV cargada ***\n");
		printf("minH = %d , maxH = %d\n",this->propiedades.resultado_rangos_color.hmin,this->propiedades.resultado_rangos_color.hmax);
		printf("minS = %d , maxS = %d\n",this->propiedades.resultado_rangos_color.smin,this->propiedades.resultado_rangos_color.smax);
		printf("minB = %d , maxB = %d\n",this->propiedades.resultado_rangos_color.vmin,this->propiedades.resultado_rangos_color.vmax);
		printf("*** Configuración del algoritmo de detección del apuntador ***\n");
		printf("Area mínima del aputnador = %d\n",this->propiedadesDeteccion.min_area);
		printf("Proporción Máxima (area marcador/area apuntador) = %f\n",this->propiedadesDeteccion.factorProporcionApuntadorMarcador);
		printf("Erode = %d\n",this->propiedadesDeteccion.erode);
		printf("Dilate = %d\n",this->propiedadesDeteccion.dilate);
		printf("Smooth = %d\n",this->propiedadesDeteccion.smooth);
		if(this->propiedadesDeteccion.enclosing_circle){
			printf("Se utiliza algoritmo con Enclosing Circle\n ");
		}else{
			printf("NO se utiliza algoritmo con Enclosing Circle\n ");
		}

		return 1;
	}else{
		printf("Se usaran los valores por defecto, el motivo es: No se puede leer el archivo de configuracion\n");
	}
	return 0;
}


void Pui::initImagenesOpenCVApuntador(){
	/*Estas imagenes seran procesadas por funciones de opencv para detectar el apuntador*/
	printf("Inicialización de imagenes OpenCV para detección de apuntador (%d,%d)\n",this->dataArtoolkit.xsize,this->dataArtoolkit.ysize);

	//this->imageColorApuntador =  crearImagenCv2((uchar*)this->dataArtoolkit.dataPtr,this->dataArtoolkit.xsize,this->dataArtoolkit.ysize);
	this->imageColorApuntador =  crearImagenCv3(this->dataArtoolkit.xsize,this->dataArtoolkit.ysize);
	this->imageMonoApuntador = cvCreateImage( cvGetSize(this->imageColorApuntador), 8, 1);
}

int Pui::initDeteccionApuntador(char* configuracionFileName){
	int res = cargarConfiguracionDeteccionApuntadorDesdeArchivo(configuracionFileName);
	if(res){
		initImagenesOpenCVApuntador();
	}else{
		fprintf(stderr, "No se pudo cargar configuracion para la detección del apuntador\n");
	}
	return res;
}

void Pui::initDeteccionApuntador(PropiedadesDeteccionApuntador propiedadesDeteccion,RangosColorApuntador rangosColorApuntador){
	this->propiedadesDeteccion = propiedadesDeteccion;
	this->propiedades.resultado_rangos_color = rangosColorApuntador;

	initImagenesOpenCVApuntador();
}

void Pui::initARConVideo(char * config_name,char * camera_para,char *vconf,int usarAlgoritmoRapido,int loopSpeed,double zoom){
	printf("initARConVideo!\n");
	ARParam  wparam;

    printf("initARConVideo\n vconf= %s \n" , vconf);
      /* open the video path */
    if( arVideoOpen( vconf) < 0 ) exit(0);
    printf("Video abierto....\n");
    this->videoCapOpened = true;
    /* find the size of the window */
    if( arVideoInqSize(&(this->dataArtoolkit.xsize), &(this->dataArtoolkit.ysize) ) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", this->dataArtoolkit.xsize, this->dataArtoolkit.ysize);

    double frameRateInterval;
    if(arVideoInqFramerateInterval(&frameRateInterval) ==0){
    	printf("****Framerate Interval = %f \n",frameRateInterval);
    }else{
    	printf("****Framerate no especificado, se asumen 30fps (Framerate Interval = %f) \n",1./30.);
    	frameRateInterval=1./30.;
    }
    this->dataArtoolkit.frameRateInterval = frameRateInterval;
    //sleepInterval es el tiempo que se suspende la aplicacion cuando
    //se pide una imagen antes de que haya pasado el frameRateInterval.
    //Como usamos usleep tenemos que multplicar el intervalo en segundos por un millon


   if(loopSpeed==FAST_SPEED_LOOP){
	   sleepInterval = frameRateInterval*FAST_SPEED_LOOP_MULTIPLIER* 1000000;
   }else{//se asume NORMAL_SPEED_LOOP
	   sleepInterval = frameRateInterval*NORMAL_SPEED_LOOP_MULTIPLIER* 1000000;
   }

    printf("Intervalo que se duerme = %f microsegundos ( %f segundos)\n",sleepInterval,sleepInterval/1000000);
    this->dataArtoolkit.cparam_name = camera_para;
    /* set the initial camera parameters */
    if( arParamLoad(this->dataArtoolkit.cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, this->dataArtoolkit.xsize, this->dataArtoolkit.ysize, &(this->dataArtoolkit.cparam) );
    arInitCparam( &(this->dataArtoolkit.cparam) );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &(this->dataArtoolkit.cparam) );

    if(config_name!=NULL){
    	if(this->multimodalInfo.modo==MODO_MULTI_MARKERS || this->multimodalInfo.modo==MODO_MULTI_MARKERS_CALIBRADOR){
			if( (this->dataArtoolkit.config = arMultiReadConfigFile(config_name)) == NULL ) {
				fprintf(stderr, "Error cargando archivo multi markers Artoolkit !!\n");
				exit(0);
			}
			printf("this->dataArtoolkit.config->marker_num=%d\n",this->dataArtoolkit.config->marker_num);
			this->dataArtoolkit.coordenadas_marcadores = new ARCoordMarcador[this->dataArtoolkit.config->marker_num];
    	}else if(this->multimodalInfo.modo==MODO_SIMPLE){
    		this->dataArtoolkit.coordenadas_marcadores = new ARCoordMarcador[1]; //En el modo MODO_SIMPLE no se almacena informacion de
																				 //mas de un marcador por vez. Siempre se el marcador detectado.
    		if(this->leerPatternsModoSimple(config_name)<=0){
    			fprintf(stderr,"Error cargando archivo simple markers Artoolkit!!\n");
    			exit(0);
    		}
    	}else{
    		fprintf(stderr, "Error, modo desconocido (%d). Termina la ejecucion\n",this->multimodalInfo.modo);
    		this->finish();
    		exit(0);
    	}
    }

    zoomFactor = zoom;

    arFittingMode   = AR_FITTING_TO_IDEAL;
	arImageProcMode = AR_IMAGE_PROC_IN_FULL;

    this->dataArtoolkit.usarAlgoritmoLite = usarAlgoritmoRapido;
    this->dataArtoolkit.thresh = 100;
    this->dataArtoolkit.cf = MIN_CF_MARKER_DETECT;

    data = new unsigned char [this->dataArtoolkit.xsize*this->dataArtoolkit.ysize*3];
    length = this->dataArtoolkit.xsize*this->dataArtoolkit.ysize*3;

    timeUtils->utilTimerReset();
}


/****Init Principal****/
void Pui::initPuiConCapturaDeVideo(int modo,PropiedadesDeteccionApuntador propiedadesDeteccion,RangosColorApuntador rangosColorApuntador,char * config_name,char * camera_para,char *vconf,int usarAlgoritmoRapido,int loopSpeed,double zoom){
	this->multimodalInfo.modo = modo;
	this->initARConVideo(config_name,camera_para,vconf,usarAlgoritmoRapido,loopSpeed,zoom);//no modificar el orden de las invocaciones!
	if(modo==MODO_MULTI_MARKERS){ //En modo MODO_SIMPLE O MODO_MULTI_MARKERS_CALIBRADOR no hago deteccion de apuntador
		this->initDeteccionApuntador(propiedadesDeteccion,rangosColorApuntador);
	}
}

/****Init Principal desde archivo de Configuracion****/
void Pui::initPuiConCapturaDeVideo(int modo,char* configuracionFileName,char * config_name,char * camera_para,char *vconf,int usarAlgoritmoRapido,int loopSpeed,double zoom){
	this->multimodalInfo.modo = modo;
	this->initARConVideo(config_name,camera_para,vconf,usarAlgoritmoRapido,loopSpeed,zoom);//no modificar el orden de las invocaciones!
	if(modo==MODO_MULTI_MARKERS){
		this->initDeteccionApuntador(configuracionFileName);
	}

}


PropiedadesDeteccionApuntador Pui::getPropiedadesDeteccionApuntador(){
	return this->propiedadesDeteccion;
}

RangosColorApuntador Pui::getPropiedadesColorApuntador(){
	return this->propiedades.resultado_rangos_color;
}

void Pui::setPropiedadesDeteccionApuntador(PropiedadesDeteccionApuntador propiedadesDeteccion){
	this->propiedadesDeteccion = propiedadesDeteccion;
}

void Pui::setPropiedadesColorApuntador(RangosColorApuntador rangosColorApuntador){
	this->propiedades.resultado_rangos_color = rangosColorApuntador;
}


void Pui::cerrarVentanaGrafica(){
#ifdef GLUT_HABILITADO
	argCleanup();
#else
	fprintf(stderr, "Pui no compilada con soporte para manejo de ventanas.\n Es necesario recompilar Pui con la siguiente bandera: \n -DGLUT_HABILITADO=1");
#endif
}

void Pui::abrirVentanaGrafica(){
#ifdef GLUT_HABILITADO
	int cero = 0;
	glutInit(&cero, NULL);
	argInit( &(this->dataArtoolkit.cparam), this->zoomFactor, 0, 2, 1, 0 );
#else
	fprintf(stderr, "Pui no compilada con soporte para manejo de ventanas.\n Es necesario recompilar Pui con la siguiente bandera: \n -DGLUT_HABILITADO=1");
#endif
}

void Pui::desplegarImagenFromData(unsigned char * imageData){
#ifdef GLUT_HABILITADO

	if(imageData!=NULL){
		argDrawMode2D();
		argDispImage( imageData, 0,0 );
		argSwapBuffers();
	}

#else
	fprintf(stderr, "Pui no compilada con soporte para manejo de ventanas.\n Es necesario recompilar Pui con la siguiente bandera: \n -DGLUT_HABILITADO=1");
#endif
}

void Pui::desplegarImagenAR(bool debug){
#ifdef GLUT_HABILITADO

		if( debug != arDebug) {//Si estaba debagueando y ahora ya no debagueo, entonces limpio la pantalla para que no quede la imagen chica congelada
			glClearColor( 0.0, 0.0, 0.0, 0.0 );
			glClear(GL_COLOR_BUFFER_BIT);
			argSwapBuffers();
		}
		arDebug = debug;

		argDrawMode2D();
		if( !debug) {
			argDispImage( this->dataArtoolkit.dataPtr, 0,0 );
		}
		else {
			argDispImage( this->dataArtoolkit.dataPtr, 1, 1 );
			if(arImage!=NULL){	//arImage es una imagen interna de artoolkit y se accede solo para debaguear. Si no se llama a arDetectMarker o no se activa la bandera arDebug entonces sera NULL
				if( arImageProcMode == AR_IMAGE_PROC_IN_HALF )
					argDispHalfImage( arImage, 0, 0 );
				else
					argDispImage( arImage, 0, 0);

				glColor3f( 1.0, 0.0, 0.0 );
				glLineWidth(6.0);
				for( int i = 0; i < this->dataArtoolkit.marker_num; i++ ) {
					argDrawSquare( this->dataArtoolkit.marker_info[i].vertex, 0, 0 );
				}
				glLineWidth( 1.0 );
			}else{
				if( arImageProcMode == AR_IMAGE_PROC_IN_HALF )
					argDispHalfImage( this->dataArtoolkit.dataPtr, 0, 0 );
				else
					argDispImage( this->dataArtoolkit.dataPtr, 0, 0);

				glColor3f( 1.0, 0.0, 0.0 );
				glLineWidth(6.0);
				for( int i = 0; i < this->dataArtoolkit.marker_num; i++ ) {
					argDrawSquare( this->dataArtoolkit.marker_info[i].vertex, 0, 0 );
				}
				glLineWidth( 1.0 );
			}
		}
		argSwapBuffers();

#else
	fprintf(stderr, "Pui no compilada con soporte para manejo de ventanas.\n Es necesario recompilar Pui con la siguiente bandera: \n -DGLUT_HABILITADO=1");
#endif
}

int Pui::getARThreshold(){
	return this->dataArtoolkit.thresh ;
}

void Pui::setARThreshold(int threshold){
	this->dataArtoolkit.thresh = threshold;
}

void Pui::finish(){
	if(this->videoCapStarted){
		capStop();
		capClose();
	}else if(this->videoCapOpened){
		capClose();
	}
	//cleanupAR();
}

void Pui::setDebugCalibracion(bool debug){
	this->propiedades.debug_imagenes = debug;
}

bool Pui::isDebugCalibracion(){
	return this->propiedades.debug_imagenes;
}

RangosColorApuntador Pui::getResultadosRangosColor(){
	return this->propiedades.resultado_rangos_color;
}


void Pui::setHminApuntador(int hMin){

}

int Pui::getHminApuntador(){

}

void Pui::setHmaxApuntador(int hMax){

}

int Pui::getHmaxApuntador(){

}

void Pui::setSminApuntador(int vMin){

}

int Pui::getSminApuntador(){

}

void Pui::setSmaxApuntador(int vMax){

}

int Pui::getSmaxApuntador(){

}

void Pui::setVminApuntador(int vMin){

}

int Pui::getVminApuntador(){

}

void Pui::setVmaxApuntador(int vMax){

}

int Pui::getVmaxApuntador(){

}

/**
 * Con la implementacion actual de video.c que trae artoolkit,
 * esta operacion NO HACE NADA, siempre retorna TRUE
 */
void Pui::capNext(){
	arVideoCapNext();
}

void Pui::capStart(){
	this->videoCapStarted = true;
	arVideoCapStart();
}
void Pui::capStop(){
	if(this->videoCapStarted ){
		this->videoCapStarted = false;
		arVideoCapStop();
	}
}

void Pui::capClose(){
	arVideoClose();
}

double Pui::getFPS(){
	return (double)this->count/arUtilTimer();
}

double Pui::getFPSAndResetCount(){
	double res = (double)this->count/arUtilTimer();
	this->count=0;
	arUtilTimerReset();
	return res;
}

char* Pui::getModuleInfo(){
	return "Modulo PUI para python creado con SWIG - 7.02.2011";
}
