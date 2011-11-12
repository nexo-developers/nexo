/**
 * Estructuras.h
 *
 *  Created on: 26/11/2010
 * @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 * @author Marcela Bonilla
 *		   bonilla.marce@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "EstructurasPublicas.h"

typedef struct{
	CvHistogram *histograma_h;			//Histograma acumulativo de la distribucion de color del apuntador en las diferentes muestras tomadas
	CvHistogram *histograma_s;			//Histograma acumulativo de la distribucion de saturacion del apuntador en las diferentes muestras tomadas
	CvHistogram *histograma_v;			//Histograma acumulativo de la distribucion de brillo del apuntador en las diferentes muestras tomadas
	int h_bins;							//Cantidad de los intervalos en los que se divide le histograma h
	int s_bins;							//Cantidad de los intervalos en los que se divide le histograma s
	int v_bins;							//Cantidad de los intervalos en los que se divide le histograma v
	RangosColorApuntador rangos;		//Rangos propios de la escala que se utiliza. [H:0-180,S:0-255,V:0-255] o [H:0-360,S:0-100,V:0-100]

}HistogramasData;


typedef struct {
/*Calibracion del apuntador*/
	int forma;							//Forma del apuntador
	HistogramasData data_histogramas;
	RangosColorApuntador resultado_rangos_color;	//Resultados de la calibracion por color

	int debug_imagenes;					//Cuando se quiera ver las imagenes de uso interno del calibrador (imagen enmascarada y mascara) debe ponerse en 1
	int restarAlRadio;					//Este valor se usa para restar al radio del circulo de las mascaras circulares y achicarlas para poder calibrar objetos mas pequenos que los marcadores. Por defecto es 0

/*Calibracion de la relacion marcador-apuntador*/
	int veces_por_marcador;  			//Cantidad de veces que se toma el area del apuntador sobre un marcador
	int metodo;							//Por el momento siempre vale 1 y quiere decir que la estimacion se hace usando un promedio de las medidas tomadas
	double ** medidas_apuntador;		//Matriz con las areas del apuntador. La matriz tiene las dimensiones (veces_por_marcador x cant_marcadores)
										// medidas_apuntador[i] --> Vector con las medidas del area del apuntador posando sobre el marcador i
	double ** medidas_marcador;			//Matriz con las areas de los marcadores. La matriz tiene las dimensiones (veces_por_marcador x cant_marcadores).
										//medidas_marcador[i]--> Vector con las medidas del area del marcador i. Existen veces_por_marcador medidas tomadas.
	double **  relaciones;				//relaciones[i] = medidas_apuntador[i]/medidas_marcador[i].
	double *  promedios;				//En la posicion i contiene el promedio (o el valor que arroje otro metodo indicado) del vector relaciones[i].

}PropiedadesCalibracion;

typedef struct {
	int min_area;
	float factorProporcionApuntadorMarcador;
	int erode;
	int dilate;
	int smooth;
	int enclosing_circle;
}PropiedadesDeteccionApuntador;

typedef struct {
	CvPoint2D32f centro;
	float radio;
} Circle;

#endif /* ESTRUCTURAS_H_ */
