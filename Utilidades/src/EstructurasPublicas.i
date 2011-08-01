%module EstructurasPublicas

//****************** NO UTILIZADO ACTUALMENTE********************/

/*
 * 
 *
 *  Created on: 8/03/2011
 *      Author: smarichal
 */

#ifndef ESTRUCTURASPUBLICAS_H_
#define ESTRUCTURASPUBLICAS_H_

typedef struct {
    int     area;
    double  centro[2];
    double  vertex[4][2];
    int visible;
    int id;
    double cf;
} ARCoordMarcador;

typedef struct {
	int idPatt;
	char imagen[255];
	char sonido[255];
	char nombre[255];
	int conjunto; // 0 es conjunto A y 1 es conjunto B
	int idElementoRelacionado;  // El elemento relacionado debe pertenecer al conjunto opuesto
	char nombreRelacion[255];
	int idImagenSDL;
}Elemento;


typedef struct {
	Elemento* objetosPUI;
	int modo;				//MODO_MULTI_MARKERS o MODO_SIMPLE
	int max_elementos;
	int cant_elementos_cargados;
	char soundOK[255];
	char soundError[255];
	int idSimpleMarcadorDetectado;
}PUI_Multimodal;

typedef struct {
	int tope;
	int cantHistorico;
	int * historicoElementos;
}Historico;

#ifndef RANGOS_COLOR_ESTRUCTURA
#define RANGOS_COLOR_ESTRUCTURA
typedef struct{
	int hmin, hmax, smin, smax,vmin,vmax;
}RangosColorApuntador;

#endif

typedef struct {
	int * areas_minimas;		//areas_minimas[i] indica el area cota inferior para la deteccion del apuntador sobre el marcador i
	int * areas_maximas;		//areas_maximas[i] indica el area cota superior para la deteccion del apuntador sobre el marcador i
	double * relaciones_promedio; // relaciones_promedio[i] indica la relacion que hay entre el area promedio del apuntador y el area promedio del marcador i
}ResultadosCalibracion;


typedef struct {
    int     area;
    double  centro[2];
}CoordApuntador;

#endif /* ESTRUCTURASPUBLICAS_H_ */

