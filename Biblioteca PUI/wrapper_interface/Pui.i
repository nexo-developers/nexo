/* @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */
%module PuiModule
%include "EstructurasPublicas.i"
%{
#include "Pui.h"
//#include "EstructurasPublicas.h"
#include <numpy/arrayobject.h>
/*Incluyo Artoolkit*/
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/arMulti.h>
#include <AR/video.h>

#include <sys/ioctl.h>
#include <linux/videodev.h>
#include <fcntl.h>

%}


%init %{
        import_array(); /* load the Numeric PyCObjects */
%}


%typemap(python, in) char* typemapString {
    printf("En typemapString\n");
    if ($input == Py_None) {
    	fprintf(stderr,"typemapString recibio Py_None, retorna NULL...\n");
        $1 = NULL;
    } else if (!PyString_Check($input)) {
    	fprintf(stderr,"typemapString no recibio un char*, retorna error...\n");
	    PyErr_SetString(PyExc_TypeError, "Se esperaba un String");
	    return NULL;
    } else {
    	$1 = PyString_AsString($input);
	}
}

%apply (char* typemapString) {char* propiedadesDeteccion_RangosColor_Filename,char * configMarcadoresAR,char * camera_para,	char *vconf,const char* archivoConfiguracion};

%typemap(out,optimal="1") binary_data {
  	binary_data bin_data = $1;
  	int size = bin_data.size;
  	if(size==0){
  		return Py_BuildValue("");
  	}
  	/*	para crear el array en un espacio de memoria compartida ya creado y pasado por parametro
  		array = (PyArrayObject *) PyArray_ContiguousFromObject(input,PyArray_UBYTE,1,1);
  	*/
  	
  	
  	PyArrayObject *array = (PyArrayObject *) PyArray_FromDims(1, &size,PyArray_UBYTE);
  	if (array == NULL){
    	PyErr_SetString(PyExc_ValueError,"Error creando PyArray");
    	return Py_BuildValue("");
    }
    
    //Una vez creado el array copio la info 
    memcpy(array->data,(unsigned char *)(bin_data.data),size);
  	
  	$result = (PyObject *)array;
}





#define FORMA_APUNTADOR_CIRCULO	1
#define FORMA_APUNTADOR_RECTANGULAR	2
#define FORMA_APUNTADOR_MANO	3

#define ERROR_DETECTANDO_MARCADORES 4
#define ERROR_CALCULANDO_MATRIZ_TRANSFORMACION 5
#define ERROR_NINGUN_MARCADOR_DETECTADO 6
#define ERROR_IMAGEN_NULL 7
#define DETECCION_CORRECTA 8

#define MASCARA_APLICADA 9
#define MASCARA_NO_APLICADA 10

#define TIPO_MASCARA_BINARIA 11
#define TIPO_MASCARA_ATENUANTE 12

#define ERROR_TOMAR_MUESTRA_FORMA_NO_IMPLEMENTADA 13
#define ERROR_TOMAR_MUESTRA_OK 14

#define OFFSET_ROI 5			//Cuanto mas grande mas grande sera el ROI donde busco el apuntador
#define MIN_AREA_WIDTH_HEIGHT 5	//Este valor sirve para indicar cual es el area minima en la cual se busca un apuntador.
								//Por ejemplo si el marcador se encuentra apenas dentro de la pantalla y el height o el width no superan este valor entonces
								//el area no sera analizada en busca de un apuntador

#define MIN_CF_MARKER_DETECT 0.6
#define MODO_MULTI_MARKERS 15
#define MODO_SIMPLE		   16
#define MODO_MULTI_MARKERS_CALIBRADOR 17

#include "cvUtilities.h"
#include "Estructuras.h"
#include "Funciones.h"
#include "Estructuras_wrapp.h"

class Pui {

public:

	static int const FAST_SPEED_LOOP = 15;
	static int const NORMAL_SPEED_LOOP = 16;
	static double const FAST_SPEED_LOOP_MULTIPLIER = 1./8.;
	static double const NORMAL_SPEED_LOOP_MULTIPLIER = 1./2.;

	/*int insert(unsigned char *bytes, int len);
	binary_data out();
	*/


	Pui();
	virtual ~Pui();

		/************************************** Manejo de Objetos PUI **************************************/
	/*
	 * Inicializa los objetos PUI desde un archivo de texto.
	 * Como máximo se cargan la cantidad indicada por max_elementos
	 *
	 * Además de las propiedades de cada objeto PUI también se carga un sonido
	 * de error y uno de éxito
	 *
	 * @param archivoConfiguracion Ruta completa del archivo de configuración
	 * @param max_elementos Candidad máxima de elementos que serán cargados
	 */
	int leerConfiguracionObjetosPUI(const char* archivoConfiguracion,int max_elementos);

	/**
	 * Lee un archivo de configuracion conteniendo una lista de ruta de archivos .patt
	 * que seran cargados para utilizar en el modo MODO_SIMPLE
	 *
	 * @param config_name Ruta completa del archivo conteniendo la lista de .patt
	 *
	 * @return Retorna la cantidad de marcadores que fueron cargados
	 */
	int leerPatternsModoSimple(const char* config_name);
	/**
	 * Imprime en consola la información de los objetos PUI cargados
	 */
	void imprimirPropiedadesObjetosPUI();


			/*Funciones para acceder a propiedades de los objetos PUI*/

	/**
	 * Retorna el nombre del Objeto PUI correspondiente al identificador idObjeto
	 *
	 * @param idObjeto Identificador del Objeto PUI
	 */
	char * getNombreObjetoPUI(int idObjeto);

	/**
	 * Retorna la ruta del archivo de sonido asociado al Objeto PUI correspondiente al identificador idObjeto
	 *
	 * @param idObjeto Identificador del Objeto PUI
	 */
	char * getSonidoObjetoPUI(int idObjeto);

	/**
	 * Retorna la ruta del archivo de imagen asociado al Objeto PUI correspondiente al identificador idObjeto
	 *
	 * @param idObjeto Identificador del Objeto PUI
	 */
	char * getImagenObjetoPUI(int idObjeto);

	/**
	 * Retorna la ruta del archivo de sonido de error.
	 */
	char* getSonidoError();

	/**
	 * Retorna la ruta del archivo de sonido de exito.
	 */
	char* getSonidoOk();

	/**
	 * Retorna true si y solo si el objeto1 esta relacionado con el objeto2
	 * objeto1---->objeto2
	 * No tiene porque cumplirse la coondicion inversa ( objeto2--->objeto1)
	 */
	bool esRelacionCorrecta(int idObjeto1,int idObjeto2);

	int getConjuntoObjetoPui(int idObjeto);

	int getIdElementoRelacionado(int idObjeto);

	char * getNombreRelacionObjetoPUI(int idObjeto);

	/* Si hay algun apuntador tocando un marcador entonces devuelve el indice del marcador,
	 * de lo contrario retorna -1. Como máximo retorna el id de un sólo marcador aunque
	 * puedan existir multiples selecciones
	 *
	 * Algoritmo de detección:
	 *
	 * 	1- Para cada marcador M no detectado:
	 * 	  	1.1 - hay apuntador en el área correspondiente a M?
	 * 			1.1.1 - Si ==> MarcadorSeleccionado = M
	 * 				   		   break //termina el loop
	 *
	 *  2- Está habilitada la funciónalidad de historico?
	 *  	2.1 - cantDeteccionesMarcador = Registrar detección (M)
	 *  	2.2 - cantDeteccionesMarcador >= CANT_HISTORICO?
	 *  		2.1 - Si ==> retorno M.id
	 *  		2.2 - No ==> retonro -1	(aun no se llegó a detectar la selección sobre el mismo marcador la cantidad mínima
	 *  								de veces seguidas para decidir que realmente se haya producido una selección. Esta funcionalidad
	 *  								se denomina histórico y agrega robustez al algoritmo de detección evitando falsos positivos en
	 *  								los casos en que el apuntador no se detenga encima del marcador o que simplemente se haya introducido
	 *  								ruido en la imágen)
	 *
	 *  Desarrollo del punto 1.1:
	 *  	Función hayApuntador:
	 *  		1- Transformar la imagen original img a su respectiva representación en HSV(H:0-360,S:0-100,V:0-100) : img_hsv
	 *  		2- Binarización de la imágen:
	 *  			2.1: Para cada píxel P perteneciente a img_hsv:
	 *  				2.1.1 : P está dentro de los rangos de color (definidos en la estructura RangosColorApuntador) ?
	 *  					2.1.1.1 Si ==> img_mono[indice(P)] = 1
	 *  					2.1.1.2 No ==> img_mono[indice(P)] = 0
	 *
	 *  		3 - Aplicar cvSmooth (openCV) para eliminar ruido
	 *  		   Si PropiedadesDeteccionApuntador.smooth		 		El smooth o también conocido como blur (desenfoque) genera un borroneo en la
	 *  			==>	img_mono = cvSmooth(img_mono)			 		imagen y se utiliza para reducir el ruido de la imagen. Se utiliza un vecindario de 7x7 píxeles para este filtro
	 *  																------------------------------------------------------
	 *  		4 - Aplicar cvErode (openCV) para eliminar ruido.
	 *  			Si PropiedadesDeteccionApuntador.erode > 0								Esta operación se utiliza para eliminar las pequeñas manchas aisladas en
	 *  			==> img_mono = cvErode(img_mono,PropiedadesDeteccionApuntador.erode)	la imagen. La idea es que las pequeñas manchas desaparezcan pero que
	 *  														 							las áreas mas grandes y significativas queden visibles. Luego de aplicar
	 *  														 							este filtro deberían permanecer en la imágen binaria solamente aquellos
	 *  														 							objetos de tamaño significativo cuyo color estaba comprendido dentro de los
	 *  														 							rangos aplicados en la binarización.
	 *  																------------------------------------------------------
	 *  		5- Aplicar cvDilate (openCV).
	 *  			Si PropiedadesDeteccionApuntador.dilate > 0								Se aplica dilate para conectar regiones cercanas y asi encontrar componentes
	 *  			==>img_mono = cvDilate(img_mono,PropiedadesDeteccionApuntador.dilate)	conectados. Muchas veces algunas regiones quedan con vacios que pueden ser producidos
	 *						  							 							 		por ejemplo por un punto de luz u otro tipo de ruido. Una pequeña dilatación alrededor
	 *  																			 		de estas regiones vacias puede terminar rellenandolas y produciendo un componente conexo sin
	 *  																			 		agujeros. (luego se aproximan los componentes a polígonos y se hayan sus contornos)
	 *			6- Deteccion de contornos y calculo del area
	 *				6.1- contornos = calcularContonosExteriores(img_mono)					Calcula solamente los contornos exteriores, no interesan aquellos contornos que estén anidados
	 *					Si: PropiedadesDeteccionApuntador.enclosing_circle
	 *						==> area = area del circulo que circunscribe al contorno		-- Se utiliza cvMinEnclosingCircle
	 *					No:
	 *						==> area = area de la aproximacion por poligono del contorno	-- Se utiliza poly = cvApproxPoly y area = cvContourArea(poly)
	 *				6.2  (area>=PropiedadesDeteccionApuntador.min_area &&
	 *				      area<=(areaMarcador/PropiedadesDeteccionApuntador.factorProporcionApuntadorMarcador))?	--Actualmente los chequeos de proporcion entre el area del
	 *				      Si:==>retorna area																		apuntador y el marcador no son de mucha utilidad porque no se
	 *				      No:==>retorna -1																			logró una precisión tan exacta en los calculos de ambas áreas y
	 *																												los resultados son muy variables.
	 *																												En cambio es importante establecer un area minima y chequear que el
	 *																												area encontrada al menos la iguale, esto evita falsos positivos por ruido
	 *																												en la imagen u objetos con tonalidades similares al apuntador
	 *
	 *
	 *
	 * */
	int detectarSeleccion();

	/**
	 * Se inicializan los parámetros para realizar la detección de los marcadores.
	 * También se inicializan las estructuras e imágenes auxiliares que se utilizan para la detección del apuntador.
	 *
	 * @param propiedadesDeteccion estructura con las propiedades utilizadas para realizar la detección del apuntador.
	 * Ver documentación de la estructura PropiedadesDeteccionApuntador y de la función Pui::detectarSeleccion
	 *
	 * @param rangosColorApuntador estructura que indica el rango de colores en representación HSV utilizado para la
	 * detección del apuntador
	 * Ver documentación de la estructura RangosColorApuntador y de la función Pui::detectarSeleccion
	 */
	void initDeteccionApuntador(PropiedadesDeteccionApuntador propiedadesDeteccion,RangosColorApuntador rangosColorApuntador);

	/**
	 * Se inicializan los parámetros para realizar la detección de los marcadores a partir de un archivo de configuración
	 * También se inicializan las estructuras e imágenes auxiliares que se utilizan para la detección del apuntador.

	 * Ver documentación de archivos de configuración
	 */
	int initDeteccionApuntador(char* configuracionFileName);

	/**
	 * Devuelve la configuración utilizada para la deteccion del apuntador.
	 * area minima
	 * proporcion entre el area del apuntador y los marcadores
	 * Parámetros para la eliminación de ruido: erode, dilate y smooth
	 * Flag indicando si el algorimo de detección aproxima el contorno del apuntador y
	 * por lo tanto su área a la de un círculo
	 */
	PropiedadesDeteccionApuntador getPropiedadesDeteccionApuntador();

	/**
	 * Establece la configuración utilizada para la deteccion del apuntador.
	 *
	 * propiedadesDeteccion.min_area : area minima
	 * propiedadesDeteccion.factorProporcionApuntadorMarcador : proporcion entre el area del apuntador y los marcadores
	 * propiedadesDeteccion.erode/dilate/smooth : Parámetros para la eliminación de ruido: erode, dilate y smooth (1,1,1 en casos normales)
	 * propiedadesDeteccion.enclosing_circle: Flag indicando si el algorimo de detección aproxima el contorno del apuntador y
	 * por lo tanto su área a la de un círculo. Se recomienda setearla en 1 cuando se utilizan apuntadores
	 * con forma aproximada a una esfera
	 */
	void setPropiedadesDeteccionApuntador(PropiedadesDeteccionApuntador propiedadesDeteccion);

	/**
	 * Devuelve los rangos HSV en los que se está intentando detectar el apuntador.
	 * La escala HSV utilizada en todos los casos es : H:0-360 S:0-100 V:0-100
	 */
	RangosColorApuntador getPropiedadesColorApuntador();

	/**
	 * Establece los rangos HSV en los que se está intentando detectar el apuntador.
	 * La escala HSV utilizada en todos los casos es : H:0-360 S:0-100 V:0-100
	 */
	void setPropiedadesColorApuntador(RangosColorApuntador rangosColorApuntador);

	/**
	 * Establece solamente el Hue minimo para la binarización de
	 * la imagen en el algoritmo de detección del apuntador. (Setea RangosColorApuntador.hmin)
	 */
	void setHminApuntador(int hMin);
	/**
	 * Devuelve el Hue minimo para la binarización de
	 * la imagen en el algoritmo de detección del apuntador. (Retorna RangosColorApuntador.hmin)
	 */
	int getHminApuntador();

	/**
	 * Establece solamente el Hue maximo para la binarización de
	 * la imagen en el algoritmo de detección del apuntador. (Setea RangosColorApuntador.hmax)
	 */
	void setHmaxApuntador(int hMax);

	/**
	 * Devuelve el Hue maximo para la binarización de
	 * la imagen en el algoritmo de detección del apuntador. (Retorna RangosColorApuntador.hmax)
	 */
	int getHmaxApuntador();

	/*Análoga a setHmin*/
	void setSminApuntador(int vMin);

	/*Análoga a getHmin*/
	int getSminApuntador();

	/*Analoga a setHmax*/
	void setSmaxApuntador(int vMax);

	/*Análoga a getHmax*/
	int getSmaxApuntador();

	/*Analoga a setHmin*/
	void setVminApuntador(int vMin);

	/*Análoga a getHmin*/
	int getVminApuntador();

	/*Analoga a setHmax*/
	void setVmaxApuntador(int vMax);

	/*Análoga a getHmax*/
	int getVmaxApuntador();

			/***************************************************************************************************/


		   /***************************  Calibracion de Proporcion apuntador-marcador  ***********************/

	/**
	 * Calcula los resultados de la calibración que establece la proporción
	 * del área del apuntador y el marcador.
	 *
	 * Actualmente obsoleta, para que funcione de forma óptima la comparación del área entre el
	 * marcador y el apuntador se requiere mayor precisión en los algoritmos que calculan ambas áreas.
	 *
	 * Para mayor información ver el programa Calibrador adjunto con la distribución PUI
	 */
	void procesarDatosCalibracion();

	/**
	 * Guarda los resultados de la calibración que establece la proporción
	 * del área del apuntador y el marcador.
	 *
	 * Actualmente obsoleta, para que funcione de forma óptima la comparación del área entre el
	 * marcador y el apuntador se requiere mayor precisión en los algoritmos que calculan ambas áreas.
	 *
	 * @param nombreArchivo Ruta completa del archivo donde se guarda la configuración
	 */
	void guardarDatosCalibracion(const char* nombreArchivo);


	/**
	 * Inicializa las propiedades para el calibrador de la proporcion de las areas del apuntador
	 * y los marcadores
	 *
	 * Actualmente obsoleta, se necesita migrar mas funcionalidades a la clase Pui para
	 * que soporte este tipo de calibracion
	 */
	void initPropiedadesCalibracion(int veces_por_marcador, int cant_marcadores, int metodo,int debug);

	/**
	 * Inicializa las imagenes que se utilizan en el calibrador de la proporcion de las areas del apuntador
	 * y los marcadores
	 *
	 * Actualmente obsoleta, se necesita migrar mas funcionalidades a la clase Pui para
	 * que soporte este tipo de calibracion
	 */
	int leerConfiguracionCalibracion(const char* archivoConfiguracion, int cantMarcadores, char** imagenes);

		/***************************************************************************************************/



		/*********************************** Calibracion Threshold ARToolkit **************************************/

	/**
	 * Calcula el Threshold utilizado por artoolkit (por mas info ver documentación de Artoolkit) que
	 * mejor funciona con los marcadores que se esten utilizando.
	 *
	 * La definición de que un valor t1 de threshold es mejor que otro valor t2 está dada por:
	 * 	t1 detecta mas marcadores que t2?
	 * 		Si: ==> t1 es mejor
	 *      No: ==> detectan la misma cantidad de marcadores? (luego de cantidad_iteraciones se realiza un
	 *      												   promedio de marcadores detectados)
	 *      		Si: Si el promedio de CF (valor de confianza de que un marcador detectado realmente sea ese marcador, ver
	 *      								  documentacion de Artoolkit) para todos los marcadores utilizando t1 es mayor al
	 *      								 de t2 ==> t1 es mejor, sino t2 es mejor.
	 *      	    No: Entonces t2 detecta mas marcadores, por lo tanto t2 es mejor.
	 *
	 *      En resumen, t1 es mejor que t2 si detecta mas marcadores, en caso de empate se decide cual es mejor
	 *      comprando los promedios de CF obtenidos
	 *
	 *  El valor calculado se almacena en Pui.dataArtoolkit.thresh
	 *
	 *  @param cantidad_iteraciones Cantidad de veces que se detectan los marcadores
	 */
	void calibrarThreshold(int cantidad_iteraciones);

		/***************************************************************************************************/




	    /**************************************** Propiedades y manejo de video ************************************/
		/*	El dispositivo utilizado debe permitir estas opciones, de lo contrario el resultado es impredecible*/

	/**
	 * Setea el brillo
	 */
	void setBrillo(int brillo, char * device);

	/**
	 * Setea en true o false el Balance de blanco automatico
	 */
	void setAutoWhiteBalance(int habilitado,char * device);

	/**
	 * Setea en true o false el AGC (Auto Gain Control)
	 */
	void setAutoGain(int habilitado,char * device);

	/**
	 * Setea flip horizontal en la imagen de video
	 */
	void setHorizontalFlip(int habilitado,char * device);

	/**
	 * Abre una ventana para desplegar el video.
	 * Pui debe ser compilado con la bandera GLUT_HABILITADO , de lo contrario no hace nada
	 */
	void abrirVentanaGrafica();

	/**
	 * Cierra la ventana abierta mediante la funcion Pui::abrirVentanaGrafica
	 * Invocar esta funcion al finalizar el programa
	 * Pui debe ser compilado con la bandera GLUT_HABILITADO , de lo contrario no hace nada
	 */
	void cerrarVentanaGrafica();

	/**
	 * Despliega video en la ventana abierta por la funcion Pui::abrirVentanaGrafica()
	 * Pui debe ser compilado con la bandera GLUT_HABILITADO , de lo contrario no hace nada
	 *
	 * @param debug Indica si se debe mostrar la imagen en modo debug.
	 */
	void desplegarImagenAR(bool debug);

	/**
	 * Despliega video en la ventana abierta por la funcion Pui::abrirVentanaGrafica()
	 * Pui debe ser compilado con la opcion GLUT_HABILITADO , de lo contrario no hace nada
	 */
	void desplegarImagenFromData(unsigned char * imageData);

	/**
	 * Capturar siguiente Frame
	 *
	 * Actualmente obsoleta, en la implementacion de Artoolkit (video.c) para captura de video esta
	 * funcion no hace nada y siempre retorna TRUE
	 */
	void capNext();

	/**
	 * Comienza la captura de video
	 */
	void capStart();

	/**
	 * Detiene la captura de video
	 */
	void capStop();

	/**
	 * Finaliza la captura de video.
	 * Previo se debe llamar a capStop
	 */
	void capClose();
		/***************************************************************************************************/


			/*********************************** Captura de video *********************************/

	/**
	 * Si hay un frame disponible entonces lo retorna.
	 * De lo contrario duerme y retorna NULL
	 */
	unsigned char* capturarImagenAR();

	/**
	 * Si hay un frame disponible entonces lo retorna.
	 * De lo contrario duerme y retorna NULL
	 *
	 * Solamente para pasaje de unsigned char* a Python mediante el
	 * typemap de binary_data. Ver archivo Pui.i
	 */
	binary_data capturarPyImageAR();

	binary_data obtenerPyImagenDebugAR();


		/*******************************************************************************************/


		/******************************** Calibracion de color el apuntador ********************************/

	/*
	 * Inicializa los histogramas en cero
	 * Inicializa los resultados en los rangos maximos (H:0-360,S:0-100,V:0-100)
	 * Registra la forma de la mascara que se aplica a los frames obtenidos de la camara (circulo, rectangulo, mano)
	 * Actualmente solo soporta FORMA_APUNTADOR_CIRCULO
	 *
	 * @param forma Los posibles valores son FORMA_APUNTADOR_CIRCULO, FORMA_APUNTADOR_MANO y FORMA_APUNTADOR_RECTANGULAR.
	 * 				Actualmente solo funciona con FORMA_APUNTADOR_CIRCULO.
	 *
	 * @param rangos_escala Rangos de la escala HSV que se utilizara. Se recomienda excluir los valores 0 en H para evitar que se introduzca ruido
	 * 																  por el negro y blanco de los marcadores. Utilizar Hmin=1 y Hmax=360
	 *
	 * @param h_bins  Cantidad de Intervalos utilizados en el histograma calculado para H. Valor recomendado = 30
	 *
	 * @param s_bins  Cantidad de Intervalos utilizados en el histograma calculado para S. Valor recomendado = 20
	 *
	 * @param v_bins  Cantidad de Intervalos utilizados en el histograma calculado para V. Valor recomendado = 20
	 * */
	void initPropiedadesCalibracionSoloApuntador(int forma,RangosColorApuntador rangos_escala,int h_bins, int s_bins, int v_bins,int debug);


	/*
	 * Inicializa los histogramas en cero
	 * Inicializa los resultados en los rangos maximos (H:0-360,S:0-100,V:0-100)
	 * Registra la forma de la mascara que se aplica a los frames obtenidos de la camara (circulo, rectangulo, mano)
	 * Actualmente solo soporta FORMA_APUNTADOR_CIRCULO
	 *
	 * @param forma Los posibles valores son FORMA_APUNTADOR_CIRCULO, FORMA_APUNTADOR_MANO y FORMA_APUNTADOR_RECTANGULAR.
	 * 				Actualmente solo funciona con FORMA_APUNTADOR_CIRCULO.
	 *
	 * @param rangos_escala Rangos de la escala HSV que se utilizara. Se recomienda excluir los valores 0 en H para evitar que se introduzca ruido
	 * 																  por el negro y blanco de los marcadores. Utilizar Hmin=1 y Hmax=360
	 *
	 * @param h_bins  Cantidad de Intervalos utilizados en el histograma calculado para H. Valor recomendado = 30
	 *
	 * @param s_bins  Cantidad de Intervalos utilizados en el histograma calculado para S. Valor recomendado = 20
	 *
	 * @param v_bins  Cantidad de Intervalos utilizados en el histograma calculado para V. Valor recomendado = 20
	 * */
	void initPropiedadesCalibracionSoloApuntador(int forma,int hmin,int hmax, int smin, int smax, int vmin, int vmax,int h_bins, int s_bins, int v_bins,int debug );


	/*
	 * A partir de los marcadores detectados selecciona uno y aplica la mascara segun la forma seleccionada
	 * Como resultado se deja el frame con la mascara aplicada (this->dataArtoolkit.dataPtr)
	 * En caso de exito retorna TIPO_MASCARA_BINARIA, de lo contrario retorna MASCARA_NO_APLICADA
	 *
	 * @param idMarcador  Se aplica la mascara dejando visible el marcador indicado
	 * @param tipoMascara Los posibles valores son TIPO_MASCARA_ATENUANTE y TIPO_MASCARA_BINARIA
	*/
	int aplicarNuevaMascaraCalibrador(int idMarcador,int tipoMascara);

	/*Solamente aplica una mascara previamente calculada por la funcion aplicarNuevaMascaraCalibrador
	 * si antes no se llama al menos una vez a aplicarNuevaMascaraCalibrador entonces no se aplicara la mascara
	 * En caso de exito retorna TIPO_MASCARA_BINARIA, de lo contrario retorna MASCARA_NO_APLICADA
	 *
	 * @param tipoMascara Los posibles valores son TIPO_MASCARA_ATENUANTE y TIPO_MASCARA_BINARIA
	 * */
	int aplicarMascaraCalibrador(int tipoMascara);

	/**
	 * Las mascaras para el tipo de apuntador FORMA_APUNTADOR_CIRCULO son circunferencias aproximadamente
	 * del tamaño del marcador.
	 *
	 * Esta operacion permite achicar o agrandar la circunferencia
	 *
	 * @param restarAlRadio Los valores positivos achican la circunferencia, los valores negativos la agrandan
	 */
	void setAchicarRadioMascaraCircular(int restarAlRadio);

	/**
	 * Devuelve el valor que se esta restando al radio de la circunferencia usada
	 * en la calibracion de apuntadores de tipo FORMA_APUNTADOR_CIRCULO.
	 *
	 * Inicialmente siempre es 0
	 */
	int getAchicarRadioMascaraCircular();

	/**
	 * Retorna imagen usada en la calibracion.
	 * Puede ser util para debaguear
	 */
	unsigned char* getCalibrationImage();

	//TODO Incluir la funcionalidad que retorna la imagen de debug para Python
	//binary_data getCalibrationImage();

	/*Calcula el histograma filtrado por la mascara lo acumula al histograma calculado con las muestras	anteriores*/
	int tomarMuestraApuntador();

	/**
	 * Calcula los resultados de la calibracion del color del apuntador.
	 *
	 * @param factorThreshH Indica que nivel debe superar intervalo de H para ser incluido en los resultados.
	 * 						Valores alto producen una calibracion de H menos exigente y mas amplia, valores muy pequeños
	 * 						producen una calibracion muy precisa y puntual.
	 * 						Si el maximo en el histograma H se da con un valor de N pixeles (por ejemplo N pixeles que
	 * 						caen en el rango 20-50 del histograma H) entonces todos los intervalos (hay hbins intervalos en el hist H)
	 * 						que tengan un valor menor a N/H seran considerados nulos.
	 * 						Valor Recomendado = 10. En caso de que la calibracion falle porque los histogramas queden discontinuos
	 * 						(cuando esta funcion devuelve ERROR_RANGOS_HISTOGRAMA_NO_CONTINUO) se recomienda probar con valores mas altos
	 * 						de factorThreshH.
	 * 						En caso que la calibracion genere posteriores falsos positivos en la deteccion del apuntador se recomienda
	 * 						disminuir este valor.
	 *
	 * @param minPorcentajeRuido Indica el porcentaje de muestras para los intervalos de S y V que se consideran ruido y son puestos en 0.
	 * 							 En caso de que un intervalo de S o V tenga menos pixeles que minPorcentajeRuido % sera puesto en 0.
	 * 							 Ejemplo  . Intervalo Saturation[0-30] = 1000 px. En un total de 320*240=76800, 20px corresponde a un 1,3% del total,
	 * 										si minPorcentajeRuido=3 entonces el intervalo Saturation[0-30] se pone en 0 y no se incluye en los resultados
	 * 							Valor Recomendado = 3
	 * 							En caso que la calibracion genere posteriores falsos positivos en la deteccion del apuntador se recomienda
	 * 							aumentar este valor.
	 *
	 */
	int calcularResultadosCalibracionApuntador(int factorThreshH,int minPorcentajeRuido);

	/**
	 * Guarda solamente los rangos HSV calculados para el apuntador.
	 *
	 * @param nombreArchivo Ruta completa del archivo donde se guardar los resultados
	 */
	void guardarSoloRangosColorApuntador(char * nombreArchivo);

	/**
	 * Guarda los rangos HSV calculados y ademas los valores pasados por parametro.
	 *
	 * @param area_min (Recomendado 300, lo mas recomendable es probar segun el apuntador que se este usando.
	 * 					Valores muy pequeños producen falsos positivos en la deteccion)
	 * @param erode (ver documentacion de Pui::detectarSeleccion(). Valor recomendado = 1)
	 * @param dilate (ver documentacion de Pui::detectarSeleccion(). Valor recomendado = 1)
	 * @param smooth (ver documentacion de Pui::detectarSeleccion(). Valor recomendado = 1)
	 * @param usarEnclosingCircle (ver documentacion de Pui::detectarSeleccion(). Valor recomendado = 1 para apuntadores esfericos, 0 en otros casos)
	 * * @param nombreArchivo Ruta completa del archivo donde se guardar los resultados
	 */
	void guardarCalibracionApuntador(char * nombreArchivo,int area_min,float factorProporcionUnico,int erode, int dilate,int smooth,int usarEnclosingCircle);

	/*
	 * Setea la propiedad debug_imagenes en las props del calibrador
	 */
	void setDebugCalibracion(bool debug);

	/**
	 * Devuelve true si se esta en modo debug mientras se realiza la calibracion del apuntador
	 */
	bool isDebugCalibracion();

	/*
	 * Retorna los resultados de la calibracion del color del apuntador o
	 * los rangos que se esten usando actualmente (seteados manualmente o cargados desde un archivo de configuracion)
	 *
	 * */
	RangosColorApuntador getResultadosRangosColor();

		/******************************************************************************************************************/



		/******************************************* Manejo de ARToolkit **************************************************/
	

	/*Devuelve la informacion de todos los marcadores
	 * Tanto los marcadores  que no figuran en la estructura de dectados pero estan como visible o aquellos
	 * que directamente estan como no visibles tendran su area y cf en -1 */
	int detectarYCalcularPosicionTodosMultiMarkers();

	/**
	 * En caso de que Pui.modo sea MODO_MULTI_MARKERS invoca a la funcion detectarYCalcularPosicionTodosMultiMarkers
	 * si Pui.modo es MODO_SIMPLE invoca a la funcion detectarMarcadoresSimple
	 */
	int detectarMarcadores();

	/**
	 * Detecta un marcador cargado en modo MODO_SIMPLE.
	 * Se guarda el id del marcador detectado y puede ser consultado mediante
	 * la funcion Pui::getMarcadorSimpleDetectado
	 * Si se detecta mas de un marcador se guarda el id del marcador con mayor nivel
	 * de confianza (cf) detectado, es decir el que se haya detectado con mayor seguridad
	 */
	int detectarMarcadoresSimple();

	/**
	 * Retorna el id del marcador y objeto PUI detectado en la ultima invocacion
	 * a la funcion Pui::detectarMarcadoresSimple
	 */
	int getIdMarcadorSimpleDetectado();

	/**
	 * Retorna la informacion correspondiente al marcador detectado en la ultima invocacion
	 * a la funcion Pui::detectarMarcadoresSimple.
	 * El id del marcador es el mismo que el id del objeto PUI asociado.
	 */
	ARCoordMarcador getMarcadorSimpleDetectado();


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
	ARCoordMarcador getInfoMultiMarcador(int idMarcador);

	/**
	 * Utilizada en el modo MODO_SIMPLE
	 * Establece el minimo nivel de confianza para la deteccion de un marcador.
	 * Al detectar un marcador Artoolkit devuelve un valor llamado cf (entre 0 y 1) el cual indica
	 * la seguridad que se tiene de que el marcador detectado realmente sea ese
	 */
	void setARCf(double cf);

	/**
	 * Devuelve el nivel de confianza establecido para la deteccion de marcadores
	 * en el modo MODO_SIMPLE
	 */
	double getARCf();
	/**
	 * Setea el umbral de binarizacion usado por artoolkit.
	 * En condiciones de poca luz usar valores bajos.
	 * Valor por defecto = 100
	 */
	int getARThreshold();

	/**
	 *  Setea el umbral de binarizacion usado por artoolkit.
	 * En condiciones de poca luz usar valores bajos.
	 */
	void setARThreshold(int threshold);

	/**************************************************************************************************************************************/


















	/****************************************GENERALES PUI**********************************************************/

	/**
	 * Inicialización de PUI con soporte para capturar video
	 *
	 * @param modo Indica el modo de operación, las posibilidades son:
	 * 			1. MODO_SIMPLE : No trabaja con multiples marcadores relacionados, simplemente se detectan los marcadores que
	 * 							 se definieron en el archivo de configuración de objetos PUI y no soporta el uso de apuntadores.
	 * 							 Si un marcador no es detectado entonces sus coordenadas no podrán ser calculadas a partir de las
	 * 							 coordenadas de los demas marcadores (no existe relacion alguna entre ellos).
	 * 							 Cuando se inicializa PUI en este modo debe utilizarse la funcion Pui::detectarMarcadoresSimples
	 * 							 o Pui::detectarMarcadores la cual pregunta por el modo de operacion y realiza la correspondiente
	 * 							 invocacion a Pui::detectarMarcadoresSimples o Pui::detectarYCalcularPosicionTodosMultiMarkers segun corresponda.
	 *
	 * 			2. MODO_MULTI_MARKER : Trabaja con marcadores relacionados entre ellos. La posición de cada marcador es relativa a la de los
	 * 								   demas. Por lo tanto, detectando por lo menos un marcador es capaz de deducir la posición en pantalla del
	 * 								   resto.
	 * 								   Este modo se utiliza junto con apuntadores que al tapar un marcador disparan la selección del correspondiente
	 * 								   marcador.
	 * 								   Cuando se inicializa PUI en este modo debe utilizarse la funcion Pui::detectarMarcadoresSimples
	 * 							 	   o Pui::detectarMarcadores la cual pregunta por el modo de operacion y realiza la correspondiente
	 * 							       invocacion a Pui::detectarMarcadoresSimples o Pui::detectarYCalcularPosicionTodosMultiMarkers segun corresponda.
	 *
	 * @param propiedadesDeteccion Parametros para la detección del apuntador en caso que se utilice MODO_MULTI_MARKER
	 * 							   Ver documentacion de la funcion Pui::setPropiedadesDeteccionApuntador
	 *
	 * @param rangosColorApuntador Parametros que definen el rango de colores HSV en los cuales se busca el apuntador
	 * 						       Ver documentacion de la funcion Pui::setPropiedadesColorApuntador
	 *
	 *
	 * @param configMarcadoresAR Si modo = Pui.MODO_MULTI_MARKER
	 * 								==> Ruta completa del archivo de configuracion de Artoolkit que relaciona la posicion de los marcadores.
	 *			    					Solamente soportado para el modo MODO_MULTI_MARKER
	 *									Para mas informacion ver http://www.hitl.washington.edu/artoolkit/documentation/tutorialmulti.htm
	 *
	 *							 Si modo = Pui.MODO_SIMPLE
	 *							 	==> Ruta completa del archivo de configuracion donde se definen los marcadores que se van a utilizar
	 *							 		Este archivo consiste en un conjunto de rutas que apuntan a los .patt correspondientes a cada marcador.
	 *							 		El orden en que se declaran los archivos .patt debe ser el mismo orden en el que se declaran los objetos PUI
	 *							 		cargados por la funcion Pui::leerConfiguracionObjetosPUI
	 *
	 * @param camera_para	Archivo de calibracion de la camara
	 *
	 * @param vconf 		String de configuracion para gstreamer. En caso de ser vacio se utiliza el valor de la variable de ambiente ARTOOLKIT_CONFIG
	 *
	 * @param usarAlgoritmoRapido  Utiliza los algoritmos Lite de artoolkit para la deteccion de marcadores. Esto supone un tiemp de procesamiento menor
	 * 								 pero tambien una menor precision de la deteccion de los marcadores. En hardware con alto poder computacional no hay
	 * 								 motivo para utilizar este tipo de algoritmo Lite.
	 *
	 * @param loopSpeed		Posibles valores: Pui.FAST_SPEED_LOOP y Pui.NORMAL_SPEED_LOOP. Ver documentacion de estas dos constantes
	 *
	 * @param zoom 			En caso de haber compilado PUI con soporte para desplegar video (bandera GLUT_HABILITADO) entonces este
	 * 						parametro indica el zoom para desplegar video.
	 */
	void initPuiConCapturaDeVideo(int modo,PropiedadesDeteccionApuntador propiedadesDeteccion,RangosColorApuntador rangosColorApuntador,
								char * configMarcadoresAR,char * camera_para,char *vconf,int usarAlgoritmoRapido,int loopSpeed,double zoom);


	/**
	 * Inicialización de PUI con soporte para capturar video
	 *
	 * @param modo Indica el modo de operación, las posibilidades son:
	 * 			1. MODO_SIMPLE : No trabaja con multiples marcadores relacionados, simplemente se detectan los marcadores que
	 * 							 se definieron en el archivo de configuración de objetos PUI y no soporta el uso de apuntadores.
	 * 							 Si un marcador no es detectado entonces sus coordenadas no podrán ser calculadas a partir de las
	 * 							 coordenadas de los demas marcadores (no existe relacion alguna entre ellos).
	 * 							 Cuando se inicializa PUI en este modo debe utilizarse la funcion Pui::detectarMarcadoresSimples
	 * 							 o Pui::detectarMarcadores la cual pregunta por el modo de operacion y realiza la correspondiente
	 * 							 invocacion a Pui::detectarMarcadoresSimples o Pui::detectarYCalcularPosicionTodosMultiMarkers segun corresponda.
	 *
	 * 			2. MODO_MULTI_MARKER : Trabaja con marcadores relacionados entre ellos. La posición de cada marcador es relativa a la de los
	 * 								   demas. Por lo tanto, detectando por lo menos un marcador es capaz de deducir la posición en pantalla del resto.
	 * 								   Este modo se utiliza junto con apuntadores que al tapar un marcador disparan la selección del correspondiente
	 * 								   marcador.
	 * 								   Cuando se inicializa PUI en este modo debe utilizarse la funcion Pui::detectarMarcadoresSimples
	 * 							 	   o Pui::detectarMarcadores la cual pregunta por el modo de operacion y realiza la correspondiente
	 * 							       invocacion a Pui::detectarMarcadoresSimples o Pui::detectarYCalcularPosicionTodosMultiMarkers segun corresponda.
	 *
	 * @param propiedadesDeteccion_RangosColor_Filename : Ruta completa de archivo de configuracion incluyendo Parametros para la detección del apuntador
	 * 													  en caso que se utilice MODO_MULTI_MARKER
	 *
	 * @param configMarcadoresAR Si modo = Pui.MODO_MULTI_MARKER
	 * 								==> Ruta completa del archivo de configuracion de Artoolkit que relaciona la posicion de los marcadores.
	 *			    					Solamente soportado para el modo MODO_MULTI_MARKER
	 *									Para mas informacion ver http://www.hitl.washington.edu/artoolkit/documentation/tutorialmulti.htm
	 *
	 *							 Si modo = Pui.MODO_SIMPLE
	 *							 	==> Ruta completa del archivo de configuracion donde se definen los marcadores que se van a utilizar
	 *							 		Este archivo consiste en un conjunto de rutas que apuntan a los .patt correspondientes a cada marcador.
	 *							 		El orden en que se declaran los archivos .patt debe ser el mismo orden en el que se declaran los objetos PUI
	 *							 		cargados por la funcion Pui::leerConfiguracionObjetosPUI
	 *
	 * @param camera_para	Archivo de calibracion de la camara
	 *
	 * @param vconf 		String de configuracion para gstreamer. En caso de ser vacio se utiliza el valor de la variable de ambiente ARTOOLKIT_CONFIG
	 *
	 * @param usarAlgoritmoRapido  Utiliza los algoritmos Lite de artoolkit para la deteccion de marcadores. Esto supone un tiemp de procesamiento menor
	 * 								 pero tambien una menor precision de la deteccion de los marcadores. En hardware con alto poder computacional no hay
	 * 								 motivo para utilizar este tipo de algoritmo Lite.
	 *
	 * @param loopSpeed		Posibles valores: Pui.FAST_SPEED_LOOP y Pui.NORMAL_SPEED_LOOP. Ver documentacion de estas dos constantes
	 *
	 * @param zoom 			En caso de haber compilado PUI con soporte para desplegar video (bandera GLUT_HABILITADO) entonces este
	 * 						parametro indica el zoom para desplegar video.
	 */
	void initPuiConCapturaDeVideo(int modo,char* propiedadesDeteccion_RangosColor_Filename,char * configMarcadoresAR,char * camera_para,
								char *vconf,int usarAlgoritmoRapido,int loopSpeed,double zoom);




	/****************************************************************************************************************/

	/**************************************************OTRAS*********************************************************/

	/**
	 * Setea la propiedad que indica si se quiere habilitar el modo de depuración del
	 * algoritmo de detección de seleccion mediante apuntador utilizando imagenes.
	 */
	void setDebugConImagenes(int debugConImagenes);

	/**
	 * Retorna la propiedad que indica si se quiere habilitar el modo de depuración del
	 * algoritmo de detección de seleccion mediante apuntador utilizando imagenes.
	 */
	int getDebugConImagenes();

	/**
	 * Habilita o deshabilita la funcionalidad llamada histórico .
	 * La misma consta de llevar un conteo de las veces consecutivas que se detecto la
	 * seleccion de un mismo apuntador. Cuando se llega a un cierto valor establecido (cantHistorico)
	 * entonces recien se considerará como una seleccion y se reflejará en el resultado de la funcion Pui::detectarSeleccion
	 *
	 * El historico agrega robustez al algoritmo de detección evitando falsos positivos en los casos en que
	 * el apuntador no se detenga encima del marcador o que simplemente se haya introducido	ruido en la imágen
	 *
	 * @param habilitar Indica si se desea habilitar o deshabilitar la funcionalidad de Historico
	 * @param cantHistorico Cantidad de selecciones consecutivas que deben detectarse sobre un mismo marcador para que se
	 * 						considere una seleccion real
	 *
	 * @param valorInicial  Valor en que se inicializa el conteo, normalmente se utilizará el valor 0
	 */
	void habilitarHistorico(bool habilitar,int cantHistorico,int valorInicial);

	/**
	 * Retorna true si y solo si la funcionalidad Historico está habilitada
	 */
	bool isHistoricoHabilitado();


	/**
	 * Libera recursos.
	 * Debe ser invocada solo al finalizar el uso de Pui
	 */
	void finish();



	/**
	 * retorna la cantidad de imagenes procesadas por segundo y comienza el contador desde 0 nuevamente
	 * FIXME : Chequear la funcionalidad
	 */
	double getFPSAndResetCount();

	/**
	 * retorna la cantidad de imagenes procesadas por segundo
	 * FIXME : Chequear la funcionalidad
	 */
	double getFPS();

	/**
	 * Retorna un String con información de PUI (solo a modo informativo)
	 *
	 */
	char* getModuleInfo();

	/**
	 * Establece modo debug mediante impresiones en consola
	 *
	 * @param debugMode 1 en caso de que se quiera habilitar el modo debug en consola
	 * 					0 para deshabilitarlo
	 */
	void setDebugMode(int debugMode);

	/**
	 * @return Retorna 1 si esta el modo debug habilitado,
	 * 0 en caso contrario
	 */
	int getDebugMode();
};
