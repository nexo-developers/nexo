/**
 * @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */
#ifndef CVUTILITIES_H_INCLUDED
#define CVUTILITIES_H_INCLUDED

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include "Estructuras.h"
#include <AR/video.h>

#define ERROR_HISTOGRAMA_OK 100
#define ERROR_HISTOGRAMA_FALLO 101

#define ERROR_RANGOS_CALCULADOS_OK 102
#define ERROR_RANGOS_HISTOGRAMA_NO_CONTINUO 103

#define ERROR_HISTOGRAMA_NULL 104

IplImage* crearImagenCvGris(ARUint8  *dataPtr,int xsize,int ysize);
IplImage* crearImagenCv(ARUint8  *dataPtr,int xsize,int ysize);
IplImage* crearImagenCv2(ARUint8  *dataPtr,int xsize,int ysize);
IplImage* crearImagenCv3(int xsize,int ysize);
void templateMatching(IplImage* image, IplImage* templateImage);
void convertShervineRGBtoHSV(uchar R,uchar G, uchar B, uchar *H,uchar *S, uchar *V);
IplImage* convertImageRGBtoHSV(const IplImage *imageRGB);
void normalizarHSVOpenCV(IplImage* img);
void HSVtoHSVcvUchar(uchar *H,uchar *S,uchar *V);
void HSVtoHSVcvUdouble(double *H,double *S,double *V);
void RGBtoHSVDouble(double r, double g, double b, double *H, double *S, double *V);
/*Convierte el area indicada usando RGBtoHSVDouble para cada pixel dentro del area indicada*/
void rgb2hsv(IplImage* img,IplImage*hsv,CvRect area);
void RGBtoHSVchar(uchar r, uchar g, uchar b, uchar *H, uchar *S, uchar *V);
int hayObjetoColor(IplImage* img, IplImage* mono,int hlower,int hupper, int slower, int supper, int vlower,
					int vupper,int mostrarImagen, int xStart, int yStart, int width, int height,int minArea,
					int maxArea,int Erode,int Dilate,int Smooth,int enclosingCircle, char* ventana,char * ventanaColor);
/**
 * Muestra el segmento de imagen correspondiente donde (xStart,yStart) es el vertice
 * superior izquierdo
 *
 */
void mostrarTarget(IplImage* img,float xStart,float yStart, int width,int height);

/*
 * minH, maxH, minS, maxS, minV, maxV : Randos de color que se quiere detectar
 * verticesMarcador : Vertices definiendo zona rectangular donde se buscara el apuntador. Por ejemplo ,pueden ser los vertices de un marcador ARtoolkit
 * offsetRoi : Offset de la zona de estudio. zona de estudio es el area definida por los vertices + offsetRoi
 * dataPtr : Imagen original
 * img, mono_Image : Imagenes inicializadas con los tamanos adecuados para contener a dataPtr pero vacias
 * min_area : Area minima del apuntador. Si se detecta un apuntador con area menor no se toma en cuenta
 * max_area : Area maxima del apuntador. Si se detecta un apuntador con area mayor no se toma en cuenta
 * min_area_width_height : El minimo de alto o ancho que tiene que tener el area de estudio. Si alguna de sus 2 dimensiones es menor a este
 * 							valor entonces se descarta y no se busca el apuntador. El valor minimo que puede recibir en este parametro es 1
 * erode,smooth,dilate : Propios del algoritmo de reconocimiento de color. Si se usa erode y dilate no se usa smooth.
 *
 */
float hayApuntador(int minH,int maxH, int minS, int maxS, int minV, int maxV, double vertices[4][2], int offsetRoi, uchar* dataPtr,
					IplImage* img,IplImage* mono_Image,int min_area_width_height,int min_area,int max_area,int erode, int dilate,
					int smooth,int enclosingCircle, double *areaZonaEstudio, char* ventanaMostrarImagenMono,char* ventanaMostrarImagenColor);


/**
 * Version mas nueva
 * Idem a la anterior pero recibe el rango de colores en una estructura RangosColorApuntador.
 * Ademas no recibe un max_area sino que lo calcula mediante areaZonaEstudio/factor
 */
float hayApuntador(RangosColorApuntador rangos, double vertices[4][2], int offsetRoi, uchar* dataPtr,
					IplImage* img,IplImage* mono_Image,int min_area_width_height,int min_area,float factor,int erode, int dilate,int smooth, int enclosingCircle,double *areaZonaEstudio,char* ventanaMostrarImagenMono,char* ventanaMostrarImagenColor);



				/******************************MANEJO DE HISTOGRAMAS**********************************/

/*Crea un Histograma y setea su valor en 0*/
CvHistogram* crearHistogramaHS(RangosColorApuntador rangos,int h_bins,int s_bins);
CvHistogram* crearHistograma1D(float *ranges, int bins);
void separarImagenEnPlanos(IplImage* hsv, IplImage* h_plane,IplImage* s_plane,IplImage* v_plane);
int calcularHistograma(CvHistogram* hist,IplImage* plane,IplImage* mascara,int bins,float* ranges,int acumular,int debugMode);

int getTotalBins1D(CvHistogram* hist,int nbins);
void imprimirInfoHist(CvHistogram* hist, int nbins);

int calcRangosHist(CvHistogram* hist, int nbins, float* ranges, int continuo,double *resCalc);


//Video Functions
unsigned char* getRawVideoFrame();

#endif // CVUTILITIES_H_INCLUDED
