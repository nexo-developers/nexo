#include "Pui.h"
#include "nexo_event.h"
#include <unistd.h>
#include <pthread.h>
#include <gconf/gconf-client.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <X11/keysymdef.h>
//#include <AR/gsub.h>
//#include <GL/gl.h>
//#include <GL/glut.h>
#define MAX_MARCADORES 30
#define CANT_HISTORIA 2

int usarAlgoritmoRapido = 1;
char *vconf = "";
int xsize, ysize;
int threshold = 100;
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
bool keypad_events = false;
bool return_key_event = false;

unsigned long postMouseEventWait = 0;// wait after mouse's event

// Propiedades para gconf
char *key_adj = "/apps/mousecam/adj";
char *key_threshold = "/apps/mousecam/threshold";
char *key_keypad_events = "/apps/mousecam/keypad_events";
char *key_returnkey_event = "/apps/mousecam/return_key_instead_click";
char *monitor = "/apps/mousecam";

typedef struct {
	int idImagenSDL;
}ElementoEnPantalla;

/* Representacion de los elementosEnPantalla*/
ElementoEnPantalla elementosEnPantalla[MAX_MARCADORES];	//El indice el elemento coincide con el id del elemento que tambien coincide con el id del marcador artoolkit


static void init(void);
static void cleanup(void);
static void mainLoop(void);
void setKeys(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data);
void setKeypadMode(GConfClient *client, guint cnxn_id, GConfEntry *entry, gpointer user_data);
void* threadPUI(void *ptr);
void* listenerGconf(void *ptr);

// TODO acá se introdujo a prepo unos sleeps para solucionar problemas de rebotes que se daban en tortuga, el tema es que MouseCam envía rafagas de eventos
// que tortuga los almacena en el buffer del teclado, lo que es muy difícil de filtrar desde tortuga. Sería bueno buscar la forma de que esto se pueda
// configurar desde la pantalla de configuración.
// Otra cosa que descubrí es el hecho de los falsos positivos, dos por tres ocurren y son muy molestos, hay que implementar acá algún mecanismo de 
// eliminación de rebotes
static void generateEvent(int idObjetoPUIActual){


    switch(idObjetoPUIActual) {
	case 0:
		if (keypad_events) {
			createKeyEvent(TRUE, XK_Up, 0);
		} else {
			createRelativeMouseEvent(0,-10);
		}
        usleep(postMouseEventWait);
		break;
	case 1:
		if (keypad_events) {
			createKeyEvent(TRUE, XK_Down, 0);
		} else {
			createRelativeMouseEvent(0,10);
		}
        usleep(postMouseEventWait);
		break;
	case 2:
		if (keypad_events) {
			createKeyEvent(TRUE, XK_Right, 0);
		} else {
			createRelativeMouseEvent(10,0);
		}
        usleep(postMouseEventWait);
		break;
	case 3:
		if (keypad_events) {
			createKeyEvent(TRUE, XK_Left, 0);
		} else {
			createRelativeMouseEvent(-10,0);
		}
        usleep(postMouseEventWait);
		break;
	case 4:
		if (return_key_event) {
			createKeyEvent(TRUE, XK_Return, 0);
		} else {
			mouseClick(Button1);
		}
		usleep(postMouseEventWait);
		break;
	case 5:
		createKeyEvent(TRUE, XStringToKeysym("A"), 0);
        usleep(postMouseEventWait);
        break;
	case 6:
		createKeyEvent(TRUE, XStringToKeysym("B"), 0);
        usleep(postMouseEventWait);
        break;
	case 7:
		createKeyEvent(TRUE, XStringToKeysym("D"), 0);
        usleep(postMouseEventWait);
        break;
	case 8:
		createKeyEvent(TRUE, XStringToKeysym("I"), 0);
        usleep(postMouseEventWait);
        break;
	case 9:
		createKeyEvent(TRUE, XStringToKeysym("E"), 0);
        usleep(postMouseEventWait);
        break;
	case 10:
		createKeyEvent(TRUE, XStringToKeysym("M"), 0);
		usleep(postMouseEventWait);
		break;
	default:printf("No event defined.\n");
    }
}

int main(int argc, char **argv){
    double frameInt = 0;
    pui = new Pui();
    init();

    pthread_t hilo0, hilo1;
	int id0=0, id1=1;

	pthread_create(&hilo0, NULL, &threadPUI, (void *) &id0);
	pthread_create(&hilo1, NULL, &listenerGconf, (void *) &id1);

	pthread_join(hilo0, NULL);
	pthread_join(hilo1, NULL);
    return (0);
}

void* threadPUI(void *ptr) {
	while (true) {
		mainLoop();
	}
}
/* main loop */
static void mainLoop(void)
{
	int err_deteccion;
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

    pui->habilitarHistorico(true,CANT_HISTORIA,0);
}

/* cleanup function called when program exits */
static void cleanup(void)
{
    pui->finish();
}

void* listenerGconf(void *ptr)
{
    GConfClient *client;
    GConfValue *value;

	client = gconf_client_get_default();

	value = gconf_client_get_without_default(client, key_adj, NULL);
	if (NULL == value) {
		printf("valor de postMouseEventWait por defecto");
		postMouseEventWait = 5000000.0;
		//gconf_client_set_float(client, key_adj, 2.0, NULL);
	} else {
		printf("valor de postMouseEventWait calculado 10.000.000 / ");
		postMouseEventWait = (unsigned long) (10000000.0 / gconf_value_get_float(value));
	}

	value = gconf_client_get_without_default(client, key_threshold, NULL);
	if (NULL == value) {
		printf("valor de threshold por defecto");
		threshold = 100;
	} else {
		printf("obteniendo valor de threshold");
		threshold = gconf_value_get_int(value);
	}

	value = gconf_client_get_without_default(client, key_keypad_events, NULL);
	if (NULL == value) {
		printf("valor de keypad_events por defecto: 0 (false)");
		keypad_events = false;
	} else {
		printf("obteniendo valor de keypad_events");
		keypad_events = gconf_value_get_bool(value);
	}
	value = gconf_client_get_without_default(client, key_returnkey_event, NULL);
	if (NULL == value) {
		printf("valor de return_key_event por defecto: 0 (false)");
		return_key_event = false;
	} else {
		printf("obteniendo valor de return_key_event");
		return_key_event = gconf_value_get_bool(value);
	}

	gconf_client_add_dir(client, monitor, GCONF_CLIENT_PRELOAD_NONE, NULL);
	gconf_client_notify_add(client,
							  key_adj,
							  setKeys,
							  NULL,
							  NULL,
							  NULL);
	gconf_client_notify_add(client,
							  key_threshold,
							  setKeys,
							  NULL,
							  NULL,
							  NULL);

	gconf_client_notify_add(client,
			key_keypad_events,
			setKeypadMode,
			NULL,
			NULL,
			NULL);
	gconf_client_notify_add(client,
			key_returnkey_event,
			setKeypadMode,
			NULL,
			NULL,
			NULL);
//printf("pase 1: %d, %u\n", postMouseEventWait, postMouseEventWait);
//sleep(postMouseEventWait);
//printf("pase 2\n");
	gtk_main();
}

void setKeys(GConfClient *client, guint cnxn_id, GConfEntry *entry,
		gpointer user_data) {
	if (0 == strcmp(gconf_entry_get_key(entry), key_adj)) {
		postMouseEventWait = (unsigned long) (10000000.0 / gconf_client_get_float(client, key_adj, NULL));
		printf("Callback, nuevo valor de postMouseEventWait: %d\n", postMouseEventWait);
	} else if (0 == strcmp(gconf_entry_get_key(entry), key_threshold)) {
		threshold = gconf_client_get_int(client, key_threshold, NULL);
		pui->setARThreshold(threshold);
		printf("Callback, nuevo valor de threshold: %d\n", threshold);
	}
}

void setKeypadMode(GConfClient *client, guint cnxn_id, GConfEntry *entry,
		gpointer user_data) {
	if (0 == strcmp(gconf_entry_get_key(entry), key_keypad_events)) {
		keypad_events = gconf_client_get_bool(client, key_keypad_events, NULL);
		printf("Callback, nuevo valor de keypad_events: %d\n", keypad_events);
	} else if (0 == strcmp(gconf_entry_get_key(entry), key_returnkey_event)) {
		return_key_event = gconf_client_get_bool(client, key_returnkey_event, NULL);
		printf("Callback, nuevo valor de return_key_event: %d\n", return_key_event);
	}
}
