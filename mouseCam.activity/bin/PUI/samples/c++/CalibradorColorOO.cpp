/*
 * CalibradorColorOO.cpp
 *
 * Es exactamente lo mismo que el CalibradorColor
 * pero usando la clase Pui
 *
 *  Created on: 7/03/2011
 *      Author: seba
 */
#include "Pui.h"
#include "UtilidadesSDL.h"
#include <AR/gsub.h>
#include <GL/gl.h>
#include <GL/glut.h>
			/********************************************************************************************************************************
			 * 								CONFIGURACION DE ARTOOLKIT Y MARCADORES
			 ********************************************************************************************************************************/

char			*vconf = "";
int             count = 0;

char                *config_nameCalibrador = "Data/calibradorColor/letras.dat";

RangosColorApuntador rangosEscalaColor;

int usarAlgoritmoRapido = 1;
int mostrarDebugAR = 0;
int aplicarMascara = 0;

int h_bins = 30;	//El histograma de HUE lo dividimos en 30 partes iguales
int s_bins = 20;	//El histograma de SATURATION lo dividimos en 20 partes iguales
int v_bins = 20;	//El histograma de BRIGHTNESS lo dividimos en 20 partes iguales



int id_marcador =0;
int tipoMascara = TIPO_MASCARA_BINARIA;

int err_deteccion;

//----------
/******************************************************************************************************************************************
				 * 												VARIABLES DEL CALIBRADOR
				 *****************************************************************************************************************************************/
int cantidad_muestras= 6;
int res_aplicar_mascara;
int muestra = 0;

int aplicar_mascara=1;
int aplicar_nueva_mascara=1;
int tomar_muestra;

int thresh = 100;

int factorProporcionMaximoH = 10;
int porcentajeMinimoRuido = 7;

int jugar = 0;
Pui* pui;
unsigned char * dataImagen;
IplImage* imageCv = NULL;
				/***************************************************************************************************************************************
				 * 												DECALRACIONES DE FUNCIONES AUXILIARES Y VARIABLES AUXILIAREAS
				 ****************************************************************************************************************************************/
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void proximoElemento();

void initRangosColor(){
	rangosEscalaColor.hmin = 1;		//Teoricamente seria correcto que hmin fuera 0. Lo ponemos en 1
									//Porque los negros totales y los blancos totales son convertidos
									//con H=0 y eso lleva a que se introduzca ruido en el histograma
	rangosEscalaColor.hmax = 360;
	rangosEscalaColor.smin = 0;
	rangosEscalaColor.smax = 100;
	rangosEscalaColor.vmin = 0;
	rangosEscalaColor.vmax = 100;
}


void desplegarDataImagen(unsigned char * data){
	//pui->desplegarImagenAR(mostrarDebugAR);
	if(data!=NULL){
		if(imageCv==NULL){
			imageCv =crearImagenCv2(data,320,240);
		}
		cvShowImage("imageCv",imageCv);
		cvWaitKey(33);
	}else{
		printf("data es null!!!!!!!!!!!!!!!!!!!\n");
	}

}

void calibrar(){
	if(tomar_muestra){
		printf("Tomo la muestra!!!!!\n");
		int err_tomarMuestra = pui->tomarMuestraApuntador();

		if(err_tomarMuestra==ERROR_TOMAR_MUESTRA_FORMA_NO_IMPLEMENTADA){
			fprintf(stderr, "Error, no se pudo tomar muestra. Forma no implementada\n");
		}else if(err_tomarMuestra==ERROR_TOMAR_MUESTRA_OK){
			printf("Muestra tomada OK\n");
		}
		tomar_muestra=0;
		id_marcador++; //FIXME no se debe incrementar asi porque se va a seleccionar de alguna manera mas inteligente, capaz aleatorio
		muestra++;
	}else{
		//printf("antes de capturar\n");
		dataImagen = pui->capturarImagenAR();
		//printf("luego de capturar\n");
		if(aplicar_mascara){
			if(aplicar_nueva_mascara){	//Calculo la nueva mascara y la aplico
				err_deteccion = pui->detectarYCalcularPosicionTodosMultiMarkers();
				if(err_deteccion==DETECCION_CORRECTA){
					res_aplicar_mascara = pui->aplicarNuevaMascaraCalibrador(id_marcador,tipoMascara);
					if(res_aplicar_mascara==MASCARA_APLICADA){
						printf("Se aplico la mascara correctamente\n");
						aplicar_nueva_mascara = 0;
					}
				}else{
					printf("err_deteccion= %d \n",err_deteccion);
				}
			}else{												//No calculo nueva mascara pero la aplico
				pui->aplicarMascaraCalibrador(tipoMascara);
			}
		}
		pui->desplegarImagenAR(false);

		arUtilSleep(90000); //FIXME dormir segun el fps
		arVideoCapNext();	//Si no pongo el capnext dsps del sleep a veces pisa la imagen
	}
}

void desplegarRangosColor(RangosColorApuntador rangos){
	printf("--------Rangos de Color--------\n");
	printf("Hue: 		[%d,%d]\n",rangos.hmin,rangos.hmax);
	printf("Saturation: [%d,%d]\n",rangos.smin,rangos.smax);
	printf("Brightness: [%d,%d]\n",rangos.vmin,rangos.vmax);
	printf("--------------------------------\n");
}

void mainL(void){
	//printf("Entra al mainL");
	if(muestra<cantidad_muestras){
		calibrar();
	}else if(!jugar){
		printf("Ya se tomaron todas las muestras, ahora se procesan los histogramas\n");
		pui->calcularResultadosCalibracionApuntador(factorProporcionMaximoH,porcentajeMinimoRuido);
		printf("Resultados:\n");
		desplegarRangosColor(pui->getResultadosRangosColor());

		printf("Guardo resultados en el archivo Data/calibracionColor.con\n");
		pui->guardarSoloRangosColorApuntador("Data/calibracionColor.con");
		pui->guardarCalibracionApuntador("Data/calibracionApuntadorCompleta.con",300/*area minima*/,1.0/*factor proporcion unico para todos los marcadores*/,
										1/*erode*/,1/*dilate*/,1/*smooth*/,1/*usar enclosing circe*/);

		printf("Fin de la calibracion\n");
		jugar=1;
	}else{ //estoy en el juego
		printf("Jugando\n");
		//mainLoopClicks();
	}
}

int main(int argc, char **argv)
{
	pui = new Pui();
	printf("InitAR\n");
	//pui->initARConVideo(config_nameCalibrador,"Data/camera_para.dat",vconf,usarAlgoritmoRapido,pui->NORMAL_SPEED_LOOP,2.0);
	pui->initPuiConCapturaDeVideo(MODO_MULTI_MARKERS_CALIBRADOR,""/*no interesa el archivo de propiedads para la deteccion del apuntador*/,
			config_nameCalibrador,"Data/camera_para.dat",vconf,usarAlgoritmoRapido,pui->NORMAL_SPEED_LOOP,2.0);
	printf("Init Rangos Color\n");
	initRangosColor();
	printf("Init prop calib \n");
	pui->initPropiedadesCalibracionSoloApuntador(FORMA_APUNTADOR_CIRCULO,rangosEscalaColor,h_bins,s_bins,v_bins,1);
	pui->setDebugCalibracion(false);


	aplicar_mascara=1;
	aplicar_nueva_mascara=1;
	tomar_muestra = 0;

	//argInit( &(this->dataArtoolkit.cparam), zoom, 0, 2, 1, 0 );
	//Usaremos la ventana grafica de glut manejada por la biblioteca gsub, requiere
	//haber compilado Pui.o con la siguiente bandera -DGLUT_HABILITADO

	pui->abrirVentanaGrafica();
	//argDrawMode     = AR_DRAW_BY_TEXTURE_MAPPING;
	//argTexmapMode   = AR_DRAW_TEXTURE_HALF_IMAGE;


	arVideoCapStart();
	printf("main loop\n");
	argMainLoop( NULL, keyEvent, mainL);
	printf("---------------------");
	return (0);
}


static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        pui->finish();
        exit(0);
    }

    if( key == SDLK_RETURN){
    	tomar_muestra = 1;
    	aplicar_nueva_mascara = 1 - aplicar_nueva_mascara;
		aplicar_mascara = 1;
    }

    if( key == 'a'){
    	aplicar_mascara = 1 - aplicar_mascara;
    }

    if( key == 'n'){
		aplicar_nueva_mascara = 1 - aplicar_nueva_mascara;
		aplicar_mascara = 1;
	}

    if( key == 'u'){
		if(tipoMascara==TIPO_MASCARA_BINARIA){
			tipoMascara=TIPO_MASCARA_ATENUANTE;
			printf("Usando mascara ATENUANTE!\n");
		}else{
			tipoMascara=TIPO_MASCARA_BINARIA;
			printf("Usando mascara BINARIA!\n");
		}
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

    if( key == 'd' ) {
        mostrarDebugAR = 1 - mostrarDebugAR;
        pui->desplegarImagenAR(mostrarDebugAR);
    }

    if( key == SDLK_1 ) {
    	pui->setAchicarRadioMascaraCircular( pui->getAchicarRadioMascaraCircular()+3 );
		aplicar_nueva_mascara = 1;
		aplicar_mascara = 1;
	}

	if( key == SDLK_2 ) {
		pui->setAchicarRadioMascaraCircular( pui->getAchicarRadioMascaraCircular()-3 );
		aplicar_nueva_mascara = 1;
		aplicar_mascara = 1;
	}


}
