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

#define CANT_HISTORIA 2

int mostrarImagenDebug = 0;
int debugImg = 0;		//Parametro para hayObjetoColor. Si se le pasa 1 y se le pasa una ventana entonces va reproduciendo el ROI. Sino crea una ventana nueva pero detiene la aplicacion hasta tocar una tecla

int usarAlgoritmoRapido =1;

			/********************************************************************************************************************************
			 * 								CONFIGURACION DE ARTOOLKIT Y MARCADORES
			 ********************************************************************************************************************************/

#define MAX_MARCADORES 30

/* no se define el pipeline aqui, se usa la variable de ambiente ARTOOLKIT_CONFIG*/
char			*vconf = "";


int             xsize, ysize;
int             thresh = 100;
int             count = 0;

char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

char                *config_name = "Data/MostrarAnimales/mostrarAnimales_ModoSimple.patterns";

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
int idObjetoDetectado = -1; //id del objeto PUI que se ha seleccionado

int xNombre = 200;
int yNombre = 15;

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
    if (getenv ("ARTOOLKIT_CONFIG") == NULL)
        putenv("ARTOOLKIT_CONFIG=v4l2src device=/dev/video0 use-fixed-fps=false ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,width=640,height=480 ! identity name=artoolkit ! fakesink");

    pui = new Pui();
    printf("InitAR\n");

    init();
    printf("main loop\n");
    argMainLoop( NULL, keyEvent, mainLoop );
    printf("---------------------");
	return (0);
}

void actualizarPantalla(){
	if(!(elementosEnPantalla[idObjetoPUIActual].idImagenSDL == idSDLImagenPantalla && idSDLImagenPantalla >= 0)){//Si no sigue la misma imagen la actualizo
		if(idSDLImagenPantalla!=-1){
			OcultarZona(xNombre,yNombre,anchoPantalla - xNombre,yImagen - yNombre,0); //Borro la zona superior de la pantalla que es donde esta escrito el texto
			OcultarImage(idSDLImagenPantalla,0);
		}
		if( (elementosEnPantalla[idObjetoPUIActual].idImagenSDL == -1)  ){ //Todavia no se cargo la imagen para este elemento
			bool sinNombre = false;
			if(pui->getImagenObjetoPUI(idObjetoPUIActual)!=NULL){
				printf("voy a desplegar la imagen: %s\n",pui->getImagenObjetoPUI(idObjetoPUIActual));
				if(pui->getNombreObjetoPUI(idObjetoPUIActual)==NULL){
					printf("El nombre del objeto %d es NULL\n",idObjetoPUIActual);
					sinNombre = true;
				}else{
					printf("Nombre del objeto PUI: %s\n",pui->getNombreObjetoPUI(idObjetoPUIActual));
				}
				elementosEnPantalla[idObjetoPUIActual].idImagenSDL = PlayImageSinTitulo(pui->getImagenObjetoPUI(idObjetoPUIActual),xImagen,yImagen,0);
				if(!sinNombre){
					EscribirTexto(pui->getNombreObjetoPUI(idObjetoPUIActual),xNombre,yNombre,0);
				}
				idSDLImagenPantalla = elementosEnPantalla[idObjetoPUIActual].idImagenSDL; //Actualizo el id de la imagen mostrada actualmente
			}
		}else{
			mostrarImagen(elementosEnPantalla[idObjetoPUIActual].idImagenSDL,0);
			EscribirTexto(pui->getNombreObjetoPUI(idObjetoPUIActual),xNombre,yNombre,0);
			idSDLImagenPantalla = elementosEnPantalla[idObjetoPUIActual].idImagenSDL;
		}
	}
	//Siempre disparo el sonido del objeto PUI
	if(pui->getSonidoObjetoPUI(idObjetoPUIActual)!=NULL){
		PlaySound(pui->getSonidoObjetoPUI(idObjetoPUIActual));
	}else{
		printf("El sonido del objeto %d es NULL\n",idObjetoPUIActual);
	}
	//Actualizo la pantalla
	flip();
}


static void   keyEvent( unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
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

    /*Habilitar/Deshabilitar debug mode*/
	if( key == 'm' ) {

		pui->setDebugMode(1 - pui->getDebugMode());
		printf("Modo Debug = %d\n",pui->getDebugMode());
	}

    /* turn on and off the debug mode with right mouse */
    if( key == 'd' ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        mostrarImagenDebug = 1 - mostrarImagenDebug;
		pui->desplegarImagenAR(mostrarImagenDebug);
    }



}

/* main loop */
static void mainLoop(void)
{
    int err_deteccion;

    if(!pui->capturarImagenAR()){
    	return;
    }else{
    	err_deteccion = pui->detectarMarcadoresSimple();
		if(err_deteccion==DETECCION_CORRECTA){
			idObjetoDetectado = pui->getIdMarcadorSimpleDetectado();
			if(idObjetoDetectado!=-1){
				printf("&&&&&&&&&&& MARCADOR %s DETECTADO &&&&&&&&&&&&&&&\n",pui->getNombreObjetoPUI(idObjetoDetectado)==NULL?"--":pui->getNombreObjetoPUI(idObjetoDetectado));
				idObjetoPUIActual = idObjetoDetectado;
				actualizarPantalla();
			}
		}else if(err_deteccion==ERROR_NINGUN_MARCADOR_DETECTADO){
			//Equivalente a marker_num<=0
		}else if(err_deteccion==ERROR_DETECTANDO_MARCADORES){
			printf("Error detectando marcadores\n");
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
    }
}


static void init( void ){
	pui->initPuiConCapturaDeVideo(MODO_SIMPLE,NULL,config_name,"Data/camera_para.dat",vconf,usarAlgoritmoRapido,pui->NORMAL_SPEED_LOOP,2.0);

	pui->setARCf(0.5); //Valor entre 0.0 y 1.0 que indica el nivel de confianza al detectar un marcador. Si el nivel de confianza es menor al
						//valor escogido entonces el marcador no se considera detectado
						//Por defecto este valor esta definido en MIN_CF_MARKER_DETECT = 0.6 . Cuanto mas alto menos probabilidad de detecciones equivocadas
						//pero tambien cuesta mas realizar una deteccion, especialmente trabajando con resolucion baja
	pui->abrirVentanaGrafica();
	pui->capStart();

	cantElementosCargados = pui->leerConfiguracionObjetosPUI("Data/MostrarAnimales/mostrarAnimales_ModoSimple.objetosPUI",MAX_MARCADORES);
    printf("Se cargaron %d elementos\n",cantElementosCargados);
    if(cantElementosCargados<=0){
    	fprintf(stderr, "Error cargando objetos PUI, finaliza la ejecucion del programa\n");
    	pui->finish();
    	exit(0);
    }
    //Inicializo los id de las imagenes en pantalla en -1
    for(int i=0;i<cantElementosCargados;i++){
    	elementosEnPantalla[i].idImagenSDL=-1;
    }
    pui->imprimirPropiedadesObjetosPUI();


    //Inicializacion de sdl
	if(!initSDL(anchoPantalla,altoPantalla)){
		cleanup();
		exit(0);
	}
	CargarFondo("Data/mostrarAnimalesPUI/fondoPrototipo1.bmp",1);
	//Inicializo historial, CANT_HISTORIA es 2, menor que en el caso de tocar apuntadores.
	//Porque es menor? Porque es menos probable que se produzca un falso positivo en la deteccion de un marcador que
	//en la deteccion del apuntador
	pui->habilitarHistorico(true,CANT_HISTORIA,0);
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    pui->cerrarVentanaGrafica();
	pui->finish();
    cleanUpSDL();
}
