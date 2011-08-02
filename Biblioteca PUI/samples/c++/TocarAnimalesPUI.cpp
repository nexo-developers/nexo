/*
 * TocarAnimalesPui.cpp
 * Author: smarichal
 */
#include "Pui.h"
#include "UtilidadesSDL.h"
#include <AR/gsub.h>
#include <GL/gl.h>
#include <GL/glut.h>
			/*************************************************************************************************************************
			 * 									CONFIGURACIONES GENERALES DE LA APLICACION
			 *************************************************************************************************************************/
int mostrarVideo = 1;
int mostrarRadar = 1;

			/*************************************************************************************************************************
			 * 									CONFIGURACION DE RECONOCIMIENTO DE COLORES
			 *************************************************************************************************************************/

#define CANT_HISTORIA 4

int mostrarImagenDebug = 0;
int debugImg = 0;		//Parametro para hayObjetoColor. Si se le pasa 1 y se le pasa una ventana entonces va reproduciendo el ROI. Sino crea una ventana nueva pero detiene la aplicacion hasta tocar una tecla

int usarAlgoritmoRapido =1;

			/********************************************************************************************************************************
			 * 								CONFIGURACION DE ARTOOLKIT Y MARCADORES
			 ********************************************************************************************************************************/

#define MAX_MARCADORES 30

/* no se define el pipeline aqui, se usa la variable de ambiente ARTOOLKIT_CONFIG*/
char			*vconf = "";

int             thresh = 100;
int             count = 0;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

char                *config_name = "Data/tocarAnimalesPUI/letras.dat";
Pui* pui;

/**************************************************************************************************************************************
 * ************************************************************************************************************************************
 *  										VARIABLES DEL PROTOTIPO
 * ************************************************************************************************************************************
 * ************************************************************************************************************************************/

typedef struct {
	int idImagenSDL;
}ElementoEnPantalla;

/* Representacion de los elementosEnPantalla*/
ElementoEnPantalla elementosEnPantalla[MAX_MARCADORES];	//El indice el elemento coincide con el id del elemento que tambien coincide con el id del marcador artoolkit
int cantElementosCargados = 0;

int idObjetoPUIActual = -1; //id del objeto PUI que esta en pantalla, con este elemento deberia corresponderse la seleccion del usuario para obtener una respuesta exitosa
int idObjetoPUISeleccionado = -1; //id del objeto PUI que se ha seleccionado

int correspondenciaCorrecta = 0; //indica si el elemento del conjunto A y el del conjunto B
								 // que se seleccionaron se corresponden
int correspondenciaIncorrecta = 0;

int xNombreRelacion = 200;
int yNombreRelacion = 15;

int xImagen = 135;
int yImagen = 100;


int anchoPantalla = 655;
int altoPantalla = 700;

//Son los id de las imagenes actualmente desplegadas en la pantalla
int idSDLImagenPantalla = -1;


				/***************************************************************************************************************************************
				 * 												DECALRACIONES DE FUNCIONES AUXILIARES
				 ****************************************************************************************************************************************/
static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw();

int main(int argc, char **argv){
	pui = new Pui();
	printf("InitAR\n");

	init();
    printf("main loop\n");
    argMainLoop( NULL, keyEvent, mainLoop );
    printf("---------------------");
	return (0);
}


void proximoElemento(){
	if(idObjetoPUIActual == cantElementosCargados -1){
		idObjetoPUIActual = 0;
	}else{
		idObjetoPUIActual++;
	}
}

void actualizarPantalla(){
	if(idObjetoPUIActual!=-1){
		if(elementosEnPantalla[idObjetoPUIActual].idImagenSDL == idSDLImagenPantalla && idSDLImagenPantalla >= 0){
			printf("Se sigue con la misma imagen, no se hace nada");
		}else{
			if(idSDLImagenPantalla!=-1){
				OcultarImage(idSDLImagenPantalla,0);
			}
			if( (elementosEnPantalla[idObjetoPUIActual].idImagenSDL == -1)  ){ //Todavia no se cargo la imagen para este elemento
				printf("voy a desplegar la imagen: %s\n",pui->getImagenObjetoPUI(idObjetoPUIActual));
				elementosEnPantalla[idObjetoPUIActual].idImagenSDL = PlayImageConTitulo(pui->getImagenObjetoPUI(idObjetoPUIActual),pui->getNombreObjetoPUI(idObjetoPUIActual),xImagen,yImagen,0);
				idSDLImagenPantalla = elementosEnPantalla[idObjetoPUIActual].idImagenSDL; //Actualizo el id de la imagen mostrada actualmente
				printf("IdSDLA = %d\n",idSDLImagenPantalla);
			}else{
				mostrarImagen(elementosEnPantalla[idObjetoPUIActual].idImagenSDL,0);
				idSDLImagenPantalla = elementosEnPantalla[idObjetoPUIActual].idImagenSDL;
			}
		}

		if(correspondenciaCorrecta){
			OcultarZona(xNombreRelacion,yNombreRelacion,anchoPantalla - xNombreRelacion,yImagen - yNombreRelacion,0); //Borro la zona superior de la pantalla que es donde esta escrito el texto
			EscribirTexto(pui->getNombreRelacionObjetoPUI(idObjetoPUIActual),xNombreRelacion,yNombreRelacion,0);
			PlaySound(pui->getSonidoObjetoPUI(idObjetoPUIActual));
		}else if(correspondenciaIncorrecta){
			PlaySound(pui->getSonidoError());
		}

	}else{
		EscribirTexto("Prototipo - Estado Inicial",xNombreRelacion,yNombreRelacion,0);
	}
	//Actualizo la pantalla
	printf("Flip antes\n");
	flip();
	printf("Flip dsps\n");
}


static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanUpSDL();
		pui->finish();
		delete pui;

        exit(0);
    }


    if( key == 'v' ) {
		mostrarVideo = 1 - mostrarVideo;
		if(!mostrarVideo){
			argCleanWindow(0,0);
		}
    }

    if( key == 'r' ) {
    	printf("Mostrar Radar\n");
		mostrarRadar = 1 - mostrarRadar;
		if(!mostrarRadar){
			argCleanWindow(2,1);
		}
	}

    if( key == SDLK_RETURN ) {
		printf("Proximo\n");
		proximoElemento();
		actualizarPantalla();

	}

    if( key == 't' ) {
        printf("count = %d\n",count);
        printf("timer = %f\n",arUtilTimer());
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        printf("Enter new threshold value (current = %d): ", thresh);
        scanf("%d",&thresh); while( getchar()!='\n' );
        pui->setARThreshold(thresh);
        printf("\n");
        count = 0;
    }

    /* turn on and off the debug mode with right mouse */
    if( key == 'd' ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        mostrarImagenDebug = 1 - mostrarImagenDebug;
		pui->desplegarImagenAR(mostrarImagenDebug);
    }

    if( key == '+' ) {
            pui->setHminApuntador(pui->getHminApuntador()+5);
            pui->setHmaxApuntador(pui->getHmaxApuntador()+5);
            printf("*** Aumento hue: [%d,%d]\n", pui->getHminApuntador(),pui->getHmaxApuntador());
	}

    if( key == '-' ) {
			pui->setHminApuntador(pui->getHminApuntador()-5);
			pui->setHmaxApuntador(pui->getHmaxApuntador()-5);
			printf("*** Disminuyo hue: [%d,%d]\n", pui->getHminApuntador(),pui->getHmaxApuntador());
	}

}

/* main loop */
static void mainLoop(void)
{
    int err_deteccion;

    if(!pui->capturarImagenAR()){
    	return;
    }else{

    	err_deteccion = pui->detectarYCalcularPosicionTodosMultiMarkers();
		if(err_deteccion==DETECCION_CORRECTA){
			idObjetoPUISeleccionado = pui->detectarSeleccion();
			if(idObjetoPUISeleccionado!=-1){
				printf("&&&&&&&&&&& COLISION CON MARCADOR %d &&&&&&&&&&&&&&&\n",idObjetoPUISeleccionado);
				correspondenciaCorrecta = false;
				correspondenciaIncorrecta = false;
				if(pui->esRelacionCorrecta(idObjetoPUIActual,idObjetoPUISeleccionado)){//FIXME no preguntar por relacion, compara ids
					correspondenciaCorrecta = true;
					actualizarPantalla();
					correspondenciaCorrecta= false;
				}else{
					correspondenciaIncorrecta=false; //FIXME tendriamos que poner correspondencia incorrecta en true
					actualizarPantalla();
				}
			}
		}else if(err_deteccion==ERROR_NINGUN_MARCADOR_DETECTADO){
			//Equivalente a marker_num<=0
		}else if(err_deteccion==ERROR_DETECTANDO_MARCADORES){
			printf("Error detectando marcadores\n");
		}else if(err_deteccion==ERROR_CALCULANDO_MATRIZ_TRANSFORMACION){
			printf("Error calculando matriz de trasnformacion\n");
		}else if(err_deteccion==ERROR_IMAGEN_NULL){
			printf("Error! imagen es NULL\n");
		}else{
			printf("err_deteccion= %d \n",err_deteccion);
		}

			if(mostrarVideo){
				if(err_deteccion!=ERROR_IMAGEN_NULL){
					pui->desplegarImagenAR(mostrarImagenDebug);
				}
    		}

			//TODO como mostramos el radar con Pui? - ver versiones anteriores donde se muestra Radar con los marcadores detectados
    	/*	if(mostrarRadar){
    			argDrawMode2D();
    			argCleanWindow(2,1);
    			dibujarLimitesPantalla(xsize, ysize);
    			for( i = 0; i < marker_num; i++ ) {
    				dibujarMiniMarcador(0,0,marker_info[i].vertex,1);
    				//printf()
    				//argDrawSquare( marker_info[i].vertex, 2, 1 );
    			}
    		}
		*/

    }
}


static void init( void ){
	printf("cargo propiedades de calibracion del apuntador desde el archivo Data/calibracionApuntadorCompleta.con\n");
	pui->initPuiConCapturaDeVideo(MODO_MULTI_MARKERS,"Data/calibracionApuntadorCompleta.con",config_name,"Data/camera_para.dat",vconf,usarAlgoritmoRapido,pui->NORMAL_SPEED_LOOP,2.0);
	pui->abrirVentanaGrafica();
	pui->capStart();



	cantElementosCargados = pui->leerConfiguracionObjetosPUI("Data/tocarAnimalesPUI/objetosPUI.config",MAX_MARCADORES);
    printf("Se cargaron %d elementos\n",cantElementosCargados);

    //Inicializo los id de las imagenes en pantalla en -1
    for(int i=0;i<cantElementosCargados;i++){
    	elementosEnPantalla[i].idImagenSDL=-1;
    }
    pui->imprimirPropiedadesObjetosPUI();


    //Inicializacion de sdl
	if(!initSDL(anchoPantalla,altoPantalla)){
		cleanUpSDL();
		pui->finish();
		delete pui;
		exit(0);
	}

	CargarFondo("Data/tocarAnimalesPUI/fondoPrototipo1.bmp",1);
	//Inicializo historial
	pui->habilitarHistorico(true,CANT_HISTORIA,-1);
	idObjetoPUIActual = 0;
	actualizarPantalla();
}
