#include "Pui.h"
#include "nexo_event.h"
#include <unistd.h>
//#include <AR/gsub.h>
//#include <GL/gl.h>
//#include <GL/glut.h>
#define MAX_MARCADORES 30
#define CANT_HISTORIA 2

int mostrarVideo = 0;
int mostrarRadar = 0;
int mostrarImagenDebug = 1;
int debugImg = 0; //Parametro para hayObjetoColor. Si se le pasa 1 y se le pasa una ventana entonces va reproduciendo el ROI. Sino crea una ventana nueva pero detiene la aplicacion hasta tocar una tecla
int usarAlgoritmoRapido = 1;
char *vconf = "";
int xsize, ysize;
int thresh = 100;
int count = 0;
char *camara_calibracion = "source/Data/camera_para.dat";//Archivo de calibracion de la camara
ARParam cparam;
char *config_marcadoresAR = "source/Data/MouseCam/mouseCam.patterns"; //Ruta del archivo de configuracion donde se definen los marcadores que se van a utilizar. Este archivo consiste en un conjunto de rutas que apuntan a los .patt correspondientes a cada marcador. El orden en que se declaran los archivos .patt debe ser el mismo orden en el que se declaran los objetos PUI cargados por la funcion Pui::leerConfiguracionObjetosPUI
char *config_objetosPUI = "source/Data/MouseCam/mouseCam.objetosPUI";
Pui* pui;
int cantElementosCargados = 0;
int idObjetoPUIActual = -1; //id del objeto PUI que esta en pantalla, con este elemento deberia corresponderse la seleccion del usuario para obtener una respuesta exitosa
int idObjetoDetectado = -1; //id del objeto PUI que se ha seleccionado
int anchoPantalla = 30;
int altoPantalla = 24;
typedef struct {
	int idImagenSDL;
}ElementoEnPantalla;

/* Representacion de los elementosEnPantalla*/
ElementoEnPantalla elementosEnPantalla[MAX_MARCADORES];	//El indice el elemento coincide con el id del elemento que tambien coincide con el id del marcador artoolkit


static void init(void);
static void cleanup(void);
static void mainLoop(void);

static void generateEvent(int idObjetoPUIActual){
      
    switch(idObjetoPUIActual) {
        case 0: createRelativeMouseEvent(0,-10);break;
        case 1: createRelativeMouseEvent(0,10);break;
        case 2: createRelativeMouseEvent(10,0);break;
        case 3: createRelativeMouseEvent(-10,0);break;
        case 4: mouseClick(Button1);break;
        case 5: createKeyEvent(TRUE, XStringToKeysym("A"), 0);break;
        case 6: createKeyEvent(TRUE, XStringToKeysym("B"), 0);break;
        case 7: createKeyEvent(TRUE, XStringToKeysym("D"), 0);break;
        case 8: createKeyEvent(TRUE, XStringToKeysym("I"), 0);break;
        case 9: createKeyEvent(TRUE, XStringToKeysym("F1"), 0);break;
        case 10: createKeyEvent(TRUE, XStringToKeysym("F2"), 0);break;
        //case 10: createKeyEvent(TRUE, XK_F2, 0);break;
        case 11: createKeyEvent(TRUE, XStringToKeysym("F3"), 0);break;
        case 12: createKeyEvent(TRUE, XStringToKeysym("F4"), 0);break;
        default:printf("No event defined.\n");
    }
}

int main(int argc, char **argv){
    double frameInt = 0;
    pui = new Pui();
    init();
    printf("main loop\n");
    int err_deteccion;

    while (true) {
        //arVideoInqFramerateInterval(&frameInt);
        //usleep((unsigned long)frameInt);
        //printf("el frameInt es:%e", frameInt); 
        if(!pui->capturarImagenAR()){
        	
        }else{
        	err_deteccion = pui->detectarMarcadoresSimple();
            if(err_deteccion==DETECCION_CORRECTA){
	            idObjetoDetectado = pui->getIdMarcadorSimpleDetectado();
	            if(idObjetoDetectado!=-1){
		            printf("MARCADOR %s DETECTADO\n",pui->getNombreObjetoPUI(idObjetoDetectado)==NULL?"--":pui->getNombreObjetoPUI(idObjetoDetectado));
		            idObjetoPUIActual = idObjetoDetectado;
		            generateEvent(idObjetoPUIActual);
	            }
	        }else if(err_deteccion==ERROR_NINGUN_MARCADOR_DETECTADO){
		        //printf("No reconocio ningun marcador.\n");
	        }else if(err_deteccion==ERROR_DETECTANDO_MARCADORES){
		        printf("Error detectando marcadores\n");
	        }else if(err_deteccion==ERROR_IMAGEN_NULL){
		        printf("Error! imagen es NULL\n");
	        }else{
		        printf("err_deteccion= %d \n",err_deteccion);
	        }
        }
    }
    return (0);
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
		        printf("MARCADOR %s DETECTADO\n",pui->getNombreObjetoPUI(idObjetoDetectado)==NULL?"--":pui->getNombreObjetoPUI(idObjetoDetectado));
		        idObjetoPUIActual = idObjetoDetectado;
		        generateEvent(idObjetoPUIActual);
	        }
	    }else if(err_deteccion==ERROR_NINGUN_MARCADOR_DETECTADO){
		    //printf("No reconocio ningun marcador.\n");
	    }else if(err_deteccion==ERROR_DETECTANDO_MARCADORES){
		    printf("Error detectando marcadores\n");
	    }else if(err_deteccion==ERROR_IMAGEN_NULL){
		    printf("Error! imagen es NULL\n");
	    }else{
		    printf("err_deteccion= %d \n",err_deteccion);
	    }
    }
}


static void init( void ){

    //Inicializacion de PUI con soporte para capturar video.
    //Al indicarse modo de operacion MODO_SIMPLE este buscara detectar los marcadores que se definieron en el archivo de configuracion config_marcadoresAR 
    pui->initPuiConCapturaDeVideo(MODO_SIMPLE,NULL,config_marcadoresAR,camara_calibracion,vconf,usarAlgoritmoRapido,pui->NORMAL_SPEED_LOOP,2.0);

    pui->setARCf(0.5); //Valor entre 0.0 y 1.0 que indica el nivel de confianza al detectar un marcador. Si el nivel de confianza es menor al
					    //valor escogido entonces el marcador no se considera detectado
					    //Por defecto este valor esta definido en MIN_CF_MARKER_DETECT = 0.6 . Cuanto mas alto menos probabilidad de detecciones equivocadas
					    //pero tambien cuesta mas realizar una deteccion, especialmente trabajando con resolucion baja
    pui->capStart();

    cantElementosCargados = pui->leerConfiguracionObjetosPUI(config_objetosPUI,MAX_MARCADORES);
    printf("Se cargaron %d elementos\n",cantElementosCargados);
    if(cantElementosCargados <= 0){
    	fprintf(stderr, "Error cargando objetos PUI, finaliza la ejecucion del programa\n");
    	pui->finish();
    	exit(0);
    }
    //Inicializo los id de las imagenes en pantalla en -1
    for(int i=0;i<cantElementosCargados;i++){
    	elementosEnPantalla[i].idImagenSDL=-1;
    }
    pui->imprimirPropiedadesObjetosPUI();
	createKeyEvent(TRUE, XStringToKeysym("F3"), 0);
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    pui->finish();
}

