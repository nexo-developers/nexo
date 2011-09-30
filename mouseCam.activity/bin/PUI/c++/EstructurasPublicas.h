/*
 * EstructurasPublicas.h
 *
 *  Created on: 8/03/2011
 * @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */

#ifndef ESTRUCTURASPUBLICAS_H_
#define ESTRUCTURASPUBLICAS_H_

/**
 * Representacion en coordenadas de pantalla
 * de un Marcador
 */
typedef struct {
	/**
	 * Area del marcador
	 */
    int     area;
    /**
     * (x,y) del centro del Marcador
     */
    double  centro[2];
    /**
     * (x,y) de los 4 vertices del marcador
     */
    double  vertex[4][2];
    /**
     * 0 - Marcador no visible, indica que no fue detectado
     * 	   sino que su posicion se calculo a partir de los demas marcadores
     * 1 - Marcador visible, indica que el marcador fue detectado
     */
    int visible;
    /**
     * Identificador del marcador
     */
    int id;
    /**
     * Confidence Value (grado de confianza) de que le marcador detectado
     * realmente sea él. Es una medida entre 0 y 1 para estimar con que
     * seguridad se detectó el marcador
     */
    double cf;
} ARCoordMarcador;

/**
 * Estructura que representa un objeto PUI
 */
typedef struct {
	/**
	 * Id del marcador asociado al objeto PUI
	 */
	int idPatt;
	/**
	 * Imagen asociada al objeto PUI
	 */
	char imagen[255];
	/**
	 * Sonido asociado al objeto PUI
	 */
	char sonido[255];
	/**
	 * Nombre del objeto PUI
	 */
	char nombre[255];
	/**
	 * Conjunto al cual pertenece el objeto PUI
	 * (se puede usar para separa en grupos de elementos y establecer relaciones entre ellos)
	 */
	int conjunto;
	/**
	 * Id de un elemento PUI con el cual se relaciona. Se utiliza
	 * para hacer asociaciones simples entre los objetos PUI
	 */
	int idElementoRelacionado;
	/**
	 * Nombre de la relacion con el objeto de identificador idElementoRelacionado
	 */
	char nombreRelacion[255];
	/**
	 * @deprecated
	 * Desactualizado, no es de uso obligatorio
	 */
	int idImagenSDL;
}Elemento;

/**
 * Estructura que almacena informacion Multimodal
 */
typedef struct {
	/**
	 * Arreglo de objetos PUI cargados
	 */
	Elemento* objetosPUI;
	/**
	 * Modo en que se va a operar.
	 * MODO_MULTI_MARKERS o MODO_SIMPLE
	 */
	int modo;
	/**
	 * Maximo de objetos PUI que se pueden cargar.
	 * Tope del arreglo objetosPUI
	 */
	int max_elementos;
	/**
	 * Cantidad de objetos PUI cargados
	 */
	int cant_elementos_cargados;
	/*
	 * Ruta de archivo de sonido asociado a casos de exito
	 * (puede utilizarse para cualquier otro proposito)
	 */
	char soundOK[255];
	/**
	 * Ruta de archivo de sonido asociado a casos de error
	 * (puede utilizarse para cualquier otro proposito)
	 */
	char soundError[255];
	/**
	 * En caso de trabajar en modo MODO_SIMPLE
	 * indica el identeificador del objeto PUI detectado.
	 * El identificador del objeto PUI y de los marcadores siempre coincide
	 */
	int idSimpleMarcadorDetectado;
}PUI_Multimodal;

/**
 * Estructura para almacenamiento de historico
 */
typedef struct {
	/**\
	 * Tope del arreglo historicoElementos
	 */
	int tope;
	/**
	 * Cantidad de veces que tiene que registrarse un elemento en el
	 * historico para que su deteccion sea positiva
	 */
	int cantHistorico;
	/**
	 * Arreglo conteniendo los identificadores de los marcadores detectados
	 * (MODO_SIMPLE) o marcadores sobre los cuales se detecto el apuntador
	 * (MODO_MULTI_MARKERS) en las ultimas cantHistorico veces
	 */
	int * historicoElementos;
}Historico;

#ifndef RANGOS_COLOR_ESTRUCTURA
#define RANGOS_COLOR_ESTRUCTURA
/**
 * Estructura que define la escala HSV para la deteccion del apuntador
 * (por lo general H:0-360, S:0-100,V:0-100)
 */
typedef struct{
	int hmin, hmax, smin, smax,vmin,vmax;
}RangosColorApuntador;

#endif

/**
 * Estructura para almacenar resultados de la
 * calibracion de area apuntador/sobre area marcador
 */
typedef struct {
	/*
	 *areas_minimas[i] indica el area cota inferior para la deteccion del apuntador sobre el marcador i
	 */
	int * areas_minimas;
	/**
	 * areas_maximas[i] indica el area cota superior para la deteccion del apuntador sobre el marcador i
	 */
	int * areas_maximas;
	/**
	 * relaciones_promedio[i] indica la relacion que hay entre el area promedio del apuntador y el area promedio del marcador i
	 */
	double * relaciones_promedio;
}ResultadosCalibracion;


/**
 * Coordenadas en pantalla del apuntador
 */
typedef struct {
	/**
	 * Area aproximada del apuntador
	 */
    int     area;
    /**
     * Coordenadas (x,y) del apuntador
     */
    double  centro[2];
}CoordApuntador;

#endif /* ESTRUCTURASPUBLICAS_H_ */
