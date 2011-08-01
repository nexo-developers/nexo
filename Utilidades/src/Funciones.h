/*
 * Funciones.h
 *
 *  Created on: 26/11/2010
 *      Author: seba
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#include <stdlib.h>

/*
 * Devuelve los extremos opuestos del cuadrado defindo por puntos del parametro vertex.
 *
 * 		--------> x
 * 		|   v0     v1			minimo = v0 ( v0.x + v0.y es la menor de las sumas)
 * 		|						maximo = v2 ( v2.x + v2.y es la mayor de las sumas)
 * 		|	v3	   v2
 * 	   y\/
 */
void calcularPuntoOrigen(double vertices[4][2],int* resultado );

int aproximarArea(double vertices[4][2]);

int aproximarAreaVisible(double vertices[4][2],int xsize, int ysize, int offset_roi);

//Calcula el promedio de los elementos. largo indica el largo del arreglo elementos
double calcularPromedio(double* elementos,int largo);

//Calcula el promedio de los elementos. largo indica el largo del arreglo elementos
double calcularPromedio(int* elementos,int largo);

int calcularMaximo(double* elementos, int largo);

int calcularMinimo(double* elementos, int largo);

#endif /* FUNCIONES_H_ */
