/* @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */
#include "Funciones.h"

/*
 * Funciones.cpp
 *
 *  Created on: 26/11/2010
 *      Author: seba
 */

void calcularPuntoOrigen(double vertices[4][2],int* resultado ){
    int minimo = 0;
    int maximo = 0;
    int i;
    for(i=1;i<4;i++){
        if(vertices[i][0] + vertices[i][1] < vertices[minimo][0] + vertices[minimo][1] ){
            minimo = i;
        }
        if(vertices[i][0] + vertices[i][1] > vertices[maximo][0] + vertices[maximo][1] ){
            maximo = i;
        }
    }
    resultado[0] = minimo;
    resultado[1] = maximo;
}



/*
 * Devuelve una aproximacino del marcador definido por vertices.
 * No realiza ningun chequeo para no salirse de la pantalla, por lo tanto el area debe ser usada con cuidado para no exceder los limites
 * de la pantalla
 * */
int aproximarArea(double vertices[4][2]){
	int resultado[2];
	calcularPuntoOrigen(vertices, resultado);

	float v0_x = vertices[resultado[0]][0];
	float v0_y = vertices[resultado[0]][1];
	float v2_x = vertices[resultado[1]][0];
	float v2_y = vertices[resultado[1]][1];

	int width = abs( v2_x - v0_x);
	int height = abs(v2_y - v0_y);

	return height*width;
}

/*
 * Devuelve una aproximacion del marcador definido por vertices.
 * Chequea los limites de la pantalla
 * */
int aproximarAreaVisible(double vertices[4][2],int xsize, int ysize, int offset_roi){

	int resultado[2];
	calcularPuntoOrigen(vertices, resultado);

	float v0_x = vertices[resultado[0]][0];
	float v0_y = vertices[resultado[0]][1];
	float v2_x = vertices[resultado[1]][0];
	float v2_y = vertices[resultado[1]][1];

	int xStart = v0_x;
	if(v0_x>offset_roi){ //Chequeo para no salirme del ancho de la pantalla
		xStart = v0_x - offset_roi;
	}

	int yStart = 0;
	if(v0_y > offset_roi){ //Chequeo para no salirme del alto de la pantalla
		yStart = v0_y - offset_roi;
	}

	int width = abs( v2_x - xStart);
	if(xStart + width > xsize){ //Chequeo para no salirme del ancho de la pantalla
		width = xsize - xStart;
	}

	int height = abs(v2_y - yStart);
	if(yStart + height > ysize){ //Chequeo para no salirme del alto de la pantalla
		height = ysize - yStart;
	}

	return height*width;

}

double calcularPromedio(double* elementos,int largo){
	double promedio = 0;
	for(int i=0;i<largo;i++){
		promedio+=elementos[i];
	}
	if(promedio!=0){
		promedio = promedio/largo;
	}
	return promedio;
}


double calcularPromedio(int* elementos,int largo){
	double promedio = 0;
	for(int i=0;i<largo;i++){
		promedio+=elementos[i];
	}
	if(promedio!=0){
		promedio = promedio/largo;
	}
	return promedio;
}

int calcularMaximo(double* elementos, int largo){
	int maximo = 0;
	for(int i=0;i<largo;i++){
		if((int)elementos[i]>maximo){
			maximo= (int)elementos[i];
		}
	}
	return maximo;
}

int calcularMinimo(double* elementos, int largo){
	int minimo = 1000000;
	for(int i=0;i<largo;i++){
		if((int)elementos[i]<minimo){
			minimo = (int)elementos[i];
		}
	}
	return minimo;
}
