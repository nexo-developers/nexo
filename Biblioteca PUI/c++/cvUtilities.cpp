/**
 * @author Sebastián Marichal
 * 		   sebastian_marichal@hotmail.com
 * 		   sebarocker@gmail.com
 *
 * Universidad de la República, Montevideo , Uruguay
 */
#include <stdio.h>
#include <stdlib.h>
#include "cvUtilities.h"
#include "Funciones.h"
#define max(a,b) ((a)>(b) ? (a) : (b))
#define min(a,b) ((a)<(b) ? (a) : (b))

const float FLOAT_TO_BYTE = 255.0f;
const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;
float fR, fG, fB;
float fH, fS, fV;

CvCapture* capture;


void templateMatching(IplImage* imgGris, IplImage* templateImage){

    CvPoint		minloc, maxloc;
	double		minval, maxval;

    IplImage* map= cvCreateImage( cvSize(imgGris->width - templateImage->width+1, imgGris->height - templateImage->height+1),IPL_DEPTH_32F, 1);
    cvMatchTemplate(imgGris,templateImage,map,CV_TM_CCOEFF_NORMED);
    cvNamedWindow("templateMatchMap", 1);
    cvNamedWindow("imgMatch", 1);
    cvShowImage("templateMatchMap",map);

    cvMinMaxLoc( map, &minval, &maxval, &minloc, &maxloc, 0 );
    cvRectangle( imgGris,  cvPoint( minloc.x, minloc.y ),  cvPoint( minloc.x + templateImage->width, minloc.y + templateImage->height ), cvScalar( 0, 0, 255, 0 ), 1, 0, 0 );
    cvShowImage("imgMatch",imgGris);
    cvWaitKey(0);
    cvDestroyWindow("templateMatchMap");
    cvDestroyWindow("imgMatch");
}

IplImage* crearImagenCvGris(ARUint8  *dataPtr,int xsize,int ysize){
  //Transformar la imagen a opencv
    CvSize*  size = new CvSize();
    int channels, canny_thres;

	IplImage* image = new IplImage();

	size->width = xsize;
	size->height = ysize;
 	channels = 3;
	canny_thres = 50;

	cvInitImageHeader( image, *size, IPL_DEPTH_8U, channels);

	cvSetImageData( image, dataPtr, size->width * channels );
	printf("-CvImage created (x,y) = (%d,%d)\n", size->width, size->height);


    IplImage *img2 = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
    cvCopy(image, img2,NULL);   //TODO Es necesario crear una copia en este caso????
    IplImage* gray_image = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
    cvCvtColor(img2, gray_image, CV_RGB2GRAY);
    return gray_image;
}

IplImage* crearImagenCv(ARUint8  *dataPtr,int xsize,int ysize){
     //Transformar la imagen a opencv
    CvSize*  size = new CvSize();

    int channels, canny_thres;

	IplImage* image = new IplImage();

	size->width = xsize;
	size->height = ysize;
 	channels = 3;
	canny_thres = 50;
	cvInitImageHeader( image, *size, IPL_DEPTH_8U, channels );

    printf("Antes de crearteImage: image->depth = %d\n",image->depth);

	cvSetImageData( image, dataPtr, size->width * channels );
	printf("-CvImage created (x,y) = (%d,%d)\n", size->width, size->height);
	//return image;
    //retorno una copia de la imagen para que al grabar otro frame no me sobrescriba la memoria
    IplImage *img2 = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
    cvCopy(image, img2,NULL);
    return img2;
}


IplImage* crearImagenCv3(int xsize,int ysize){
     //Transformar la imagen a opencv
    CvSize*  size = new CvSize();
    int channels;
	IplImage* image = new IplImage();
	size->width = xsize;
	size->height = ysize;
 	channels = 3;
	cvInitImageHeader( image, *size, IPL_DEPTH_8U, channels );
	cvSetImageData( image,NULL, size->width * channels );
	printf("-CvImage creada, con data=NULL (x,y) = (%d,%d)\n", size->width, size->height);
	delete size;
    return image;
}

IplImage* crearImagenCv2(ARUint8  *dataPtr,int xsize,int ysize){
     //Transformar la imagen a opencv
    CvSize*  size = new CvSize();
    int channels;
	IplImage* image = new IplImage();
	size->width = xsize;
	size->height = ysize;
 	channels = 3;
 	//image->colorModel = {'R','G','B'};
 	//image->channelSeq = {'R','G','B'};
	cvInitImageHeader( image, *size, IPL_DEPTH_8U, channels );
	dataPtr=NULL;
	cvSetImageData( image,dataPtr/*(uchar*) dataPtr*/, size->width * channels );
	printf("-CvImage created (x,y) = (%d,%d)\n", size->width, size->height);
	delete size;
    return image;
}



// 				CONVERSIONES MODELOS RGB Y HSV


void normalizarHSVOpenCV(IplImage* img){
	uchar* p;
	for( int y=0; y<img->height; y++ ) {
		p = (uchar*) (img->imageData + y * img->widthStep);
		for( int x=0; x<img->width; x++ ) {
			p[3*x] *=  2.0;
			p[3*x+1] /=  2.55;
			p[3*x+2] /=  2.55;
		}
	}
}


/*
 * Recibe componentes HSV en la escala H:0-360 S:0-100 V:0-100 y los trasnforma
 * a la escala usada por openCV H:0-180 S:0-255 V:0-255
 */
void HSVtoHSVcvUdouble(double *H,double *S,double *V){
	uchar Hcv,Scv,Vcv;
	Hcv = (uchar)*H;
	Scv = (uchar)*S;
	Vcv = (uchar)*V;
	Hcv /= 2;
	Scv *= 2.55;
	Vcv *= 2.55;
	*H = Hcv;
	*S = Scv;
	*V = Vcv;
}

/*
 * Recibe componentes HSV en la escala H:0-360 S:0-100 V:0-100 y los trasnforma
 * a la escala usada por openCV H:0-180 S:0-255 V:0-255
 */
void HSVtoHSVcvUchar(uchar *H,uchar *S,uchar *V){
	*H /= 2;
	*S *= 2.55;
	*V *= 2.55;
}
void convertShervineRGBtoHSV(uchar R,uchar G, uchar B, uchar *H,uchar *S, uchar *V){

	// Convert from 8-bit integers to floats.
	float fR = R * BYTE_TO_FLOAT;
	float fG = G * BYTE_TO_FLOAT;
	float fB = B * BYTE_TO_FLOAT;

	// Convert from RGB to HSV, using float ranges 0.0 to 1.0.
	float fDelta;
	float fMin, fMax;
	int iMax;
	// Get the min and max, but use integer comparisons for slight speedup.
	if (B < G) {
		if (B < R) {
			fMin = fB;
			if (R > G) {
				iMax = R;
				fMax = fR;
			}
			else {
				iMax = G;
				fMax = fG;
			}
		}
		else {
			fMin = fR;
			fMax = fG;
			iMax = G;
		}
	}
	else {
		if (G < R) {
			fMin = fG;
			if (B > R) {
				fMax = fB;
				iMax = B;
			}
			else {
				fMax = fR;
				iMax = R;
			}
		}
		else {
			fMin = fR;
			fMax = fB;
			iMax = B;
		}
	}
	fDelta = fMax - fMin;
	fV = fMax;				// Value (Brightness).
	if (iMax != 0) {			// Make sure its not pure black.
		fS = fDelta / fMax;		// Saturation.
		float ANGLE_TO_UNIT = 1.0f / (6.0f * fDelta);	// Make the Hues between 0.0 to 1.0 instead of 6.0
		if (iMax == R) {		// between yellow and magenta.
			fH = (fG - fB) * ANGLE_TO_UNIT;
		}
		else if (iMax == G) {		// between cyan and yellow.
			fH = (2.0f/6.0f) + ( fB - fR ) * ANGLE_TO_UNIT;
		}
		else {				// between magenta and cyan.
			fH = (4.0f/6.0f) + ( fR - fG ) * ANGLE_TO_UNIT;
		}
		// Wrap outlier Hues around the circle.
		if (fH < 0.0f)
			fH += 1.0f;
		if (fH >= 1.0f)
			fH -= 1.0f;
	}
	else {
		// color is pure Black.
		fS = 0;
		fH = 0;	// undefined hue
	}

	// Convert from floats to 8-bit integers.
	int bH = (int)(0.5f + fH * 255.0f);
	int bS = (int)(0.5f + fS * 255.0f);
	int bV = (int)(0.5f + fV * 255.0f);

	// Clip the values to make sure it fits within the 8bits.
	if (bH > 255)
		bH = 255;
	if (bH < 0)
		bH = 0;
	if (bS > 255)
		bS = 255;
	if (bS < 0)
		bS = 0;
	if (bV > 255)
		bV = 255;
	if (bV < 0)
		bV = 0;

	// Set the HSV pixel components.
	*(H) = bH;		// H component
	*(S) = bS;		// S component
	*(V) = bV;		// V component
}

// Create a HSV image from the RGB image using the full 8-bits, since OpenCV only allows Hues up to 180 instead of 255.
// ref: "http://cs.haifa.ac.il/hagit/courses/ist/Lectures/Demos/ColorApplet2/t_convert.html"
// Remember to free the generated HSV image.
IplImage* convertImageRGBtoHSV(const IplImage *imageRGB)
{
	float fR, fG, fB;
	float fH, fS, fV;
	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

	// Create a blank HSV image
	IplImage *imageHSV = cvCreateImage(cvGetSize(imageRGB), 8, 3);
	if (!imageHSV || imageRGB->depth != 8 || imageRGB->nChannels != 3) {
		printf("ERROR in convertImageRGBtoHSV()! Bad input image.\n");
		exit(1);
	}

	int h = imageRGB->height;		// Pixel height.
	int w = imageRGB->width;		// Pixel width.
	int rowSizeRGB = imageRGB->widthStep;	// Size of row in bytes, including extra padding.
	char *imRGB = imageRGB->imageData;	// Pointer to the start of the image pixels.
	int rowSizeHSV = imageHSV->widthStep;	// Size of row in bytes, including extra padding.
	char *imHSV = imageHSV->imageData;	// Pointer to the start of the image pixels.
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			// Get the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
			uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
			int bB = *(uchar*)(pRGB+0);	// Blue component
			int bG = *(uchar*)(pRGB+1);	// Green component
			int bR = *(uchar*)(pRGB+2);	// Red component

			// Convert from 8-bit integers to floats.
			fR = bR * BYTE_TO_FLOAT;
			fG = bG * BYTE_TO_FLOAT;
			fB = bB * BYTE_TO_FLOAT;

			// Convert from RGB to HSV, using float ranges 0.0 to 1.0.
			float fDelta;
			float fMin, fMax;
			int iMax;
			// Get the min and max, but use integer comparisons for slight speedup.
			if (bB < bG) {
				if (bB < bR) {
					fMin = fB;
					if (bR > bG) {
						iMax = bR;
						fMax = fR;
					}
					else {
						iMax = bG;
						fMax = fG;
					}
				}
				else {
					fMin = fR;
					fMax = fG;
					iMax = bG;
				}
			}
			else {
				if (bG < bR) {
					fMin = fG;
					if (bB > bR) {
						fMax = fB;
						iMax = bB;
					}
					else {
						fMax = fR;
						iMax = bR;
					}
				}
				else {
					fMin = fR;
					fMax = fB;
					iMax = bB;
				}
			}
			fDelta = fMax - fMin;
			fV = fMax;				// Value (Brightness).
			if (iMax != 0) {			// Make sure its not pure black.
				fS = fDelta / fMax;		// Saturation.
				float ANGLE_TO_UNIT = 1.0f / (6.0f * fDelta);	// Make the Hues between 0.0 to 1.0 instead of 6.0
				if (iMax == bR) {		// between yellow and magenta.
					fH = (fG - fB) * ANGLE_TO_UNIT;
				}
				else if (iMax == bG) {		// between cyan and yellow.
					fH = (2.0f/6.0f) + ( fB - fR ) * ANGLE_TO_UNIT;
				}
				else {				// between magenta and cyan.
					fH = (4.0f/6.0f) + ( fR - fG ) * ANGLE_TO_UNIT;
				}
				// Wrap outlier Hues around the circle.
				if (fH < 0.0f)
					fH += 1.0f;
				if (fH >= 1.0f)
					fH -= 1.0f;
			}
			else {
				// color is pure Black.
				fS = 0;
				fH = 0;	// undefined hue
			}

			// Convert from floats to 8-bit integers.
			int bH = (int)(0.5f + fH * 255.0f);
			int bS = (int)(0.5f + fS * 255.0f);
			int bV = (int)(0.5f + fV * 255.0f);

			// Clip the values to make sure it fits within the 8bits.
			if (bH > 255)
				bH = 255;
			if (bH < 0)
				bH = 0;
			if (bS > 255)
				bS = 255;
			if (bS < 0)
				bS = 0;
			if (bV > 255)
				bV = 255;
			if (bV < 0)
				bV = 0;

			// Set the HSV pixel components.
			uchar *pHSV = (uchar*)(imHSV + y*rowSizeHSV + x*3);
			*(pHSV+0) = bH;		// H component
			*(pHSV+1) = bS;		// S component
			*(pHSV+2) = bV;		// V component
		}
	}
	return imageHSV;
}


// Create an RGB image from the HSV image using the full 8-bits, since OpenCV only allows Hues up to 180 instead of 255.
// ref: "http://cs.haifa.ac.il/hagit/courses/ist/Lectures/Demos/ColorApplet2/t_convert.html"
// Remember to free the generated RGB image.
IplImage* convertImageHSVtoRGB(const IplImage *imageHSV)
{
	float fH, fS, fV;
	float fR, fG, fB;
	const float FLOAT_TO_BYTE = 255.0f;
	const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

	// Create a blank RGB image
	IplImage *imageRGB = cvCreateImage(cvGetSize(imageHSV), 8, 3);
	if (!imageRGB || imageHSV->depth != 8 || imageHSV->nChannels != 3) {
		printf("ERROR in convertImageHSVtoRGB()! Bad input image.\n");
		exit(1);
	}

	int h = imageHSV->height;			// Pixel height.
	int w = imageHSV->width;			// Pixel width.
	int rowSizeHSV = imageHSV->widthStep;		// Size of row in bytes, including extra padding.
	char *imHSV = imageHSV->imageData;		// Pointer to the start of the image pixels.
	int rowSizeRGB = imageRGB->widthStep;		// Size of row in bytes, including extra padding.
	char *imRGB = imageRGB->imageData;		// Pointer to the start of the image pixels.
	for (int y=0; y<h; y++) {
		for (int x=0; x<w; x++) {
			// Get the HSV pixel components
			uchar *pHSV = (uchar*)(imHSV + y*rowSizeHSV + x*3);
			int bH = *(uchar*)(pHSV+0);	// H component
			int bS = *(uchar*)(pHSV+1);	// S component
			int bV = *(uchar*)(pHSV+2);	// V component

			// Convert from 8-bit integers to floats
			fH = (float)bH * BYTE_TO_FLOAT;
			fS = (float)bS * BYTE_TO_FLOAT;
			fV = (float)bV * BYTE_TO_FLOAT;

			// Convert from HSV to RGB, using float ranges 0.0 to 1.0
			int iI;
			float fI, fF, p, q, t;

			if( bS == 0 ) {
				// achromatic (grey)
				fR = fG = fB = fV;
			}
			else {
				// If Hue == 1.0, then wrap it around the circle to 0.0
				if (fH >= 1.0f)
					fH = 0.0f;

				fH *= 6.0;			// sector 0 to 5
				fI = floor( fH );		// integer part of h (0,1,2,3,4,5 or 6)
				iI = (int) fH;			//		"		"		"		"
				fF = fH - fI;			// factorial part of h (0 to 1)

				p = fV * ( 1.0f - fS );
				q = fV * ( 1.0f - fS * fF );
				t = fV * ( 1.0f - fS * ( 1.0f - fF ) );

				switch( iI ) {
					case 0:
						fR = fV;
						fG = t;
						fB = p;
						break;
					case 1:
						fR = q;
						fG = fV;
						fB = p;
						break;
					case 2:
						fR = p;
						fG = fV;
						fB = t;
						break;
					case 3:
						fR = p;
						fG = q;
						fB = fV;
						break;
					case 4:
						fR = t;
						fG = p;
						fB = fV;
						break;
					default:		// case 5 (or 6):
						fR = fV;
						fG = p;
						fB = q;
						break;
				}
			}

			// Convert from floats to 8-bit integers
			int bR = (int)(fR * FLOAT_TO_BYTE);
			int bG = (int)(fG * FLOAT_TO_BYTE);
			int bB = (int)(fB * FLOAT_TO_BYTE);

			// Clip the values to make sure it fits within the 8bits.
			if (bR > 255)
				bR = 255;
			if (bR < 0)
				bR = 0;
			if (bG > 255)
				bG = 255;
			if (bG < 0)
				bG = 0;
			if (bB > 255)
				bB = 255;
			if (bB < 0)
				bB = 0;

			// Set the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
			uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
			*(pRGB+0) = bB;		// B component
			*(pRGB+1) = bG;		// G component
			*(pRGB+2) = bR;		// R component
		}
	}
	return imageRGB;
}

void RGBtoHSVchar(uchar r, uchar g, uchar b, uchar *H, uchar *S, uchar *V) {
	double D;
	double min;
	min = min(min(r, g),b);
	*V = max(max(r, g),b);
	D = *V - min;
	if(*V == 0.0) *S = 0;
	else *S = D / *V;
	if(*S == 0.0) *H = 0;
	else if (r == *V) *H = 60.0 * (g - b) / D;
	 else if(g == *V) *H = 120.0 + 60.0 * (b - r) / D;
	  else if (b == *V) *H = 240.0 + 60.0 * (r - g) / D;
	if (*H < 0.0) *H = *H + 360.0;
	*S=*(S) * 100; //conversion de escala 0 a 1, a escala 0 a 100
	*V=*(V)/255 * 100;//conversion de escala 0 a 255, a escala 0 a 100
}

void RGBtoHSVDouble(double r, double g, double b, double *H, double *S, double *V) {
	double D;
	double min;
	min = min(min(r, g),b);
	*V = max(max(r, g),b);
	D = *V - min;
	if(*V == 0.0) *S = 0;
	else *S = D / *V;
	if(*S == 0.0) *H = 0;
	else if (r == *V) *H = 60.0 * (g - b) / D;
	 else if(g == *V) *H = 120.0 + 60.0 * (b - r) / D;
	  else if (b == *V) *H = 240.0 + 60.0 * (r - g) / D;
	if (*H < 0.0) *H = *H + 360.0;
	*S=*(S) * 100; //conversion de escala 0 a 1, a escala 0 a 100
	*V=*(V)/255 * 100;//conversion de escala 0 a 255, a escala 0 a 100
}

/*Convierte el area indicada usando RGBtoHSVDouble para cada pixel dentro del area indicada*/
void rgb2hsv(IplImage* img,IplImage*hsv,CvRect rect){
	uchar* data = (uchar *)img->imageData;
	double H,S,V;
	int height     = img->height;
	int width      = img->width;
	int step       = img->widthStep/sizeof(uchar);
	int channels   = img->nChannels;

	// Recorremos la imagen
	for(int i = 0; i <rect.height; i++ ) {
		for(int j = 0; j <rect.width; j++ ) {//(i+yStart)*step+ (j+xStart)*channels
			//ATENCION: La imagen esta pensada en RGB y no en BGR
			RGBtoHSVDouble((double)data[(i+rect.y)*step+ (j+rect.x)*channels],(double)data[(i+rect.y)*step+ (j+rect.x)*channels+1],(double)data[(i+rect.y)*step+ (j+rect.x)*channels + 2], &H,&S, &V);
			hsv->imageData[(i+rect.y)*step+ (j+rect.x)*channels] = H;
			hsv->imageData[(i+rect.y)*step+ (j+rect.x)*channels +1] = S;
			hsv->imageData[(i+rect.y)*step+ (j+rect.x)*channels +2] = V;
		}
	}
}

/**
 * Busca contornos de por lo menos minArea en la imagen gray.
 * Si encuentra un contorno que tenga area mayor o igual a minArea entonces retorna el area encontrada, de lo contrario
 * retorna 0.
 * El modo de extraccion de los contornos es CV_RETR_TREE y solo se examinan los nodos raiz, es decir solo los contornos exteriores.
 * Si enclosingCircle es true entonces el area del contorno es el area del circulo mas pequeno que lo contenga.
 * Si enclosingCircle es false entonces el area del contorno es la verdadera area del contorno y se obtiene mediante una aproximacion a un poligono
 * (cvApproxPoly) y luego el calculo del area mediante cvContourArea
 */
float hayContorno(IplImage* gray,  int minArea, int enclosingCircle, Circle * circle){
				CvMemStorage* storage = cvCreateMemStorage(0);

				// find contours and store them all as a list
				CvSeq* contours = NULL;
				cvFindContours( gray, storage, &contours, sizeof(CvContour),CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

				// test each contour

				float area =0;
				if(enclosingCircle){	//Circulo mas pequeno que contenga al contorno y luego calculo del area del circulo
					CvPoint2D32f centro;
					float radio;
					while(contours){
						cvMinEnclosingCircle(contours,&(circle->centro),&(circle->radio));

						area = pow(circle->radio,2)*CV_PI;			//FIXME no es necesario calcular el area, con el radio basta para establecer la relacion
						if(fabs(area)>=minArea){
							cvReleaseMemStorage(&storage);
							return area;
						}
						//printf("*** Centro : (%f,%f) , Radio : %f \n",centro.x,centro.y,radio);
						//printf("*** AREA   : %f \n",pow(radio,2)*CV_PI);
						contours = contours->h_next;
					}
				}else{		//Aproximacion a poligono y luego calculo del area
					while( contours ){
						// approximate contour with accuracy proportional
						// to the contour perimeter
						CvSeq* result = cvApproxPoly( contours, sizeof(CvContour), storage,	CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
						// square contours should have 4 vertices after approximation
						// relatively large area (to filter out noisy contours)
						// and be convex.
						// Note: absolute value of an area is used because
						// area may be positive or negative - in accordance with the
						// contour orientation
						area = cvContourArea(result,CV_WHOLE_SEQ);
						if(fabs(area)>=minArea){
							/*printf("Area del contorno = %f\n",area);
							printf("retorno true\n");*/
							cvReleaseMemStorage(&storage);
							return area;
						}
						// take the next contour
						contours = contours->h_next;
					}
				}

				cvReleaseMemStorage(&storage);
				return 0;

}


/**
 * Busca contornos de por lo menos minArea y maximo maxArea en la imagen gray.
 * Si encuentra un contorno que tenga area mayor o igual a minArea entonces retorna el area encontrada, de lo contrario
 * retorna 0.
 * El modo de extraccion de los contornos es CV_RETR_TREE y solo se examinan los nodos raiz, es decir solo los contornos exteriores.
 * Si enclosingCircle es true entonces el area del contorno es el area del circulo mas pequeno que lo contenga.
 * Si enclosingCircle es false entonces el area del contorno es la verdadera area del contorno y se obtiene mediante una aproximacion a un poligono
 * (cvApproxPoly) y luego el calculo del area mediante cvContourArea
 */
float hayContornoFiltrandoMinMaxArea(IplImage* gray,  int minArea, int maxArea,int enclosingCircle, Circle * circle){
				CvMemStorage* storage = cvCreateMemStorage(0);

				// find contours and store them all as a list
				CvSeq* contours = NULL;
				cvFindContours( gray, storage, &contours, sizeof(CvContour),CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

				// test each contour

				float area =0;
				if(enclosingCircle){	//Circulo mas pequeno que contenga al contorno y luego calculo del area del circulo
					CvPoint2D32f centro;
					float radio;
					while(contours){
						cvMinEnclosingCircle(contours,&(circle->centro),&(circle->radio));

						area = pow(circle->radio,2)*CV_PI;			//FIXME no es necesario calcular el area, con el radio basta para establecer la relacion
						if(fabs(area)>=minArea && fabs(area)<=maxArea){
							cvReleaseMemStorage(&storage);
							return area;
						}
						//printf("*** Centro : (%f,%f) , Radio : %f \n",centro.x,centro.y,radio);
						//printf("*** AREA   : %f \n",pow(radio,2)*CV_PI);
						contours = contours->h_next;
					}
				}else{		//Aproximacion a poligono y luego calculo del area
					while( contours ){
						// approximate contour with accuracy proportional
						// to the contour perimeter
						//TODO se deben evaluar en profundidad los parametros pasados a cvApproxPoly
						CvSeq* result = cvApproxPoly( contours, sizeof(CvContour), storage,	CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0 );
						// square contours should have 4 vertices after approximation
						// relatively large area (to filter out noisy contours)
						// and be convex.
						// Note: absolute value of an area is used because
						// area may be positive or negative - in accordance with the
						// contour orientation
						area = cvContourArea(result,CV_WHOLE_SEQ);
						if(fabs(area)>=minArea && fabs(area)<=maxArea){
							/*printf("Area del contorno = %f\n",area);
							printf("retorno true\n");*/
							cvReleaseMemStorage(&storage);
							return area;
						}
						// take the next contour
						contours = contours->h_next;
					}
				}

				cvReleaseMemStorage(&storage);
				return 0;

}


int hayObjetoColor(IplImage* img, IplImage* mono,int hlower,int hupper, int slower, int supper, int vlower,
					int vupper,int mostrarImagen, int xStart, int yStart, int width, int height,int minArea,
					int maxArea,int Erode,int Dilate,int Smooth,int enclosingCircle,char* ventana,char* ventanaColor){
	uchar *data, *data_mono;
	if(img->width < xStart + width){ //La zona a analizar se va del ancho de la imagen
		fprintf(stderr, "[hayObjetoColor]: Error, la zona a analizar excede el ancho de la pantalla\n");
	}
	if(img->height < yStart + height){
		fprintf(stderr, "[hayObjetoColor]: Error, la zona a analizar excede el alto de la pantalla\n");
	}
	if(img == 0){
		fprintf(stderr, "[hayObjetoColor]: Error, img es NULL");
	}
	if(mono == 0){
			fprintf(stderr, "[hayObjetoColor]: Error, la imagen en mono es NULL");
	}

    // Obtenemos atributos de la imagen HSV
	int step       = img->widthStep/sizeof(uchar);
	int channels   = img->nChannels;

	int step_mono   = mono->widthStep/sizeof(uchar);
	int channels_mono = mono->nChannels;

    // Obtenemos los valores RGB de la Imagen
   data = (uchar *)img->imageData;
   data_mono =(uchar *) mono->imageData;
   double H,S,V;
   int pixelesBlancos = 0;
   // Recorremos la imagen
      for(int i = 0; i <height; i++ ) {
          for(int j = 0; j <width; j++ ) {
        	  RGBtoHSVDouble((double)data[(i+yStart)*step+ (j+xStart)*channels],(double)data[(i+yStart)*step+ (j+xStart)*channels + 1],(double)data[(i+yStart)*step+ (j+xStart)*channels + 2], &H,&S, &V);
        	 // printf(" (%f,%f,%f)\n",(double)H,(double)S,(double)V);
        // Segmentamos la imagen mediante los angulos de Hue
       		  if(hupper>360){
       			  if ( ((H >= hlower) && ((H) <= hupper)) || H<=(hupper-360) ){
       						if ( (S>= slower)&&(S<=supper) ) {
       							if ( (V>= vlower)&&(V<=vupper)) {
       								// Coloreamos el pixel en negro
       								data_mono[(i+yStart)*step_mono+(j+xStart)] = 255;

       							}else {
       								// Coloreamos el pixel en blanco
       								data_mono[(i+yStart)*step_mono+(j+xStart)] = 0;
       							}
       						}else{
       							// Coloreamos el pixel en blanco
       							data_mono[(i+yStart)*step_mono+(j+xStart)] = 0;
       						}
       				//printf("negro\n");
       			  } else {
       				   // Coloreamos el pixel en blanco
       				   //printf("blanco\n");
       				data_mono[(i+yStart)*step_mono+(j+xStart)] = 0;
       			  }
       		  }else{
       			  if ( (H >= hlower) && ((H) <= hupper) ){
    					if ( (S>= slower)&&(S<=supper) ) {
    						if ( (V>= vlower)&&(V<=vupper)) {
    							// Coloreamos el pixel en negro
    							data_mono[(i+yStart)*step_mono+(j+xStart)] = 255;
    						}else {
    							// Coloreamos el pixel en blanco
    							data_mono[(i+yStart)*step_mono+(j+xStart)] = 0;
    						}
    					}else{
    						// Coloreamos el pixel en blanco
    						data_mono[(i+yStart)*step_mono+(j+xStart)] = 0;
    					}
       			  } else {
       				   // Coloreamos el pixel en blanco
       				   //printf("blanco\n");
       				  data_mono[(i+yStart)*step_mono+(j+xStart)] = 0;
       			  }
       		  }
        } // fin del for j
      } // fin del for i
      //printf("Pixeles Blancos = %d\n",pixelesBlancos);
	  if(Smooth>0){
		  cvSmooth(mono,mono,CV_BLUR,7,7);
	  }
	  if(Erode>0){
		  cvErode(mono,mono,0,Erode);
	  }
	  if(Dilate>0){
		  cvDilate(mono,mono,0,Dilate);
	  }


      //Ahora que tenemos la imagen en mono podemos encontrar los contornos
      CvRect roi = cvRect(xStart,yStart,width,height);
      cvSetImageROI(mono,roi);
      cvSetImageROI(img,roi);
      if(mostrarImagen && ventana==NULL){
      		cvNamedWindow( "ROI Mono", CV_WINDOW_AUTOSIZE );
      		cvShowImage( "ROI Mono", mono );
      		cvWaitKey(0);
      		cvDestroyWindow( "ROI Mono" );

      		cvNamedWindow( "Original", CV_WINDOW_AUTOSIZE );
			cvShowImage( "Original", img);
			cvWaitKey(0);
			cvDestroyWindow( "Original" );

		}else if(mostrarImagen){
			cvShowImage(ventana,mono);
			cvWaitKey(27);
		}
      Circle circle;
	  //float area = hayContorno(mono,minArea,enclosingCircle,&circle);
	  float area =hayContornoFiltrandoMinMaxArea(mono,minArea,maxArea,enclosingCircle,&circle);
	  //(IplImage* gray,  float minArea, int enclosingCircle, Circle * circle){
	  if(enclosingCircle && mostrarImagen){
		  printf("muestro\n");
		  cvCircle(img,cvPoint(circle.centro.x,circle.centro.y),cvRound(circle.radio),CV_RGB(200,0,0),3,8,0 );
		  cvShowImage(ventanaColor,img);
		  cvWaitKey(27);
	  }
      return area;
}


/**
 * Muestra la subimagen rectangular cuyo vertice superior izquierdo es (xStart,yStart)
 * y es de ancho y alto width y height respectivamente.
 */
void mostrarTarget(IplImage* img,float xStart,float yStart, int width,int height)
{
    cvSetImageROI(img, cvRect(xStart, yStart, width, height));
    cvNamedWindow("target");
    cvShowImage("target", img);
    cvWaitKey(0);
    cvDestroyWindow("target");
    cvSetImageROI(img, cvRect(0, 0, img->width, img->height));
}


/**
 * Nueva version.
 * Recibe un parametro factor el cual se utiliza para calcular un area maxima segun el area de la zona de estudio.
 * area_max = areaZonaEstudio/factor
 */
float hayApuntador(RangosColorApuntador rangos, double vertices[4][2], int offsetRoi, uchar* dataPtr,
					IplImage* img,IplImage* mono_Image,int min_area_width_height,int min_area,float factor,int erode, int dilate,int smooth, int enclosingCircle,double *areaZonaEstudio,char* ventanaMostrarImagenMono,char* ventanaMostrarImagenColor){

	if(factor==0)
		factor=1;
	int resultado[2];
	int xsize = img->width;
	int ysize = img->height;
	calcularPuntoOrigen(vertices, resultado);
	float v0_x = vertices[resultado[0]][0];
	float v0_y = vertices[resultado[0]][1];
	float v2_x = vertices[resultado[1]][0];
	float v2_y = vertices[resultado[1]][1];
	//Calculo x inicial
	int xStart = 0;
	if(v0_x>offsetRoi){ //Chequeo para no salirme del ancho de la pantalla
		xStart = v0_x - offsetRoi;
	}
	//Calculo y inicial
	int yStart = 0;
	if(v0_y > offsetRoi){ //Chequeo para no salirme del alto de la pantalla
		yStart = v0_y -offsetRoi;
	}
	//Calculo ancho
	int widthZonaEstudio = abs( v2_x - xStart) + offsetRoi;
	if(xStart + widthZonaEstudio > xsize){ //Chequeo para no salirme del ancho de la pantalla
		printf("Borde pantalla: xStart = %d , widthZonaEstudio = %d \n",xStart,widthZonaEstudio);
		widthZonaEstudio = xsize - xStart;
	}
	//Calculo alto
	int heightZonaEstudio = abs(v2_y - yStart) + offsetRoi;
	if(yStart + heightZonaEstudio > ysize){ //Chequeo para no salirme del alto de la pantalla
		printf("Borde pantalla: yStart = %d , heightZonaEstudio = %d \n",yStart,heightZonaEstudio);
		heightZonaEstudio = ysize - yStart;
	}

	cvSetImageData( img, (uchar*)(dataPtr), img->width* img->nChannels );
	img->origin = IPL_ORIGIN_TL;
	cvZero(mono_Image);							//FIXME Es necesario? Probar sin esto. Creo que antes era necesario porque en el else del filtrado por hsv tenia mal el acceso al pixel que tenia que setear en 0. En ese caso faltaba sumar los xstart e ystart
	float area = -1;

	//Calculo el area de la zona de estudio. Segun este area vamos a obtener la cota minima y maxima del area del apuntador esperado segun la calibracion que se hay realizado
	//Por ejemplo si el apuntador ocupa 1/8 del marcador, y el area de la zona de estudio (que es aproximadamente el area del marcador) es de 800, entonces
	//buscaremos un apuntador de (800)*(1/8) = 100 +/- UMBRAL_AREA_APUNTADOR
	*areaZonaEstudio = heightZonaEstudio*widthZonaEstudio;

	if(heightZonaEstudio>min_area_width_height  &&  widthZonaEstudio > min_area_width_height){ //Ver definicion de MIN_AREA_WIDTH_HEIGHT para comprender mejor este chequeo
		area = fabs(hayObjetoColor(img,mono_Image,rangos.hmin,rangos.hmax,rangos.smin,rangos.smax,rangos.vmin,rangos.vmax,ventanaMostrarImagenMono!=NULL,xStart,yStart,widthZonaEstudio,heightZonaEstudio,min_area, *areaZonaEstudio/factor,erode,dilate,smooth,enclosingCircle,ventanaMostrarImagenMono,ventanaMostrarImagenColor));
		if(area>0){
			printf("Area zona estudio = %f\n",*areaZonaEstudio);
			printf("Area apuntador = %f\n",area);
			/************************************************************************************************************
			 * 	DESCOMENTAR EL SIGUIENTE CODIGO PARA VER LOS SEGMENTOS DE IMAGEN EN LOS CUALES SE DETECTO EL APUNTADOR
			 *	mostrarTarget(img,xStart,yStart,widthZonaEstudio,heightZonaEstudio);
			 *	mostrarTarget(mono_Image,xStart,yStart,widthZonaEstudio,heightZonaEstudio);
			 * **********************************************************************************************************/
			//mostrarTarget(img,xStart,yStart,widthZonaEstudio,heightZonaEstudio);
			//mostrarTarget(mono_Image,xStart,yStart,widthZonaEstudio,heightZonaEstudio);

		}
	}
	return area;

}


/*@deprecated
 * Queda por compatibilidad hacia atras pero hace un manejo del area maxima diferente a la nueva version de esta funcion*/
float hayApuntador(int minH,int maxH, int minS, int maxS, int minV, int maxV, double vertices[4][2], int offsetRoi, uchar* dataPtr,
					IplImage* img,IplImage* mono_Image,int min_area_width_height,int min_area,int max_area,int erode, int dilate,int smooth, int enclosingCircle,double *areaZonaEstudio,char* ventanaMostrarImagenMono,char* ventanaMostrarImagenColor){
	int resultado[2];
	int xsize = img->width;
	int ysize = img->height;
	calcularPuntoOrigen(vertices, resultado);
	float v0_x = vertices[resultado[0]][0];
	float v0_y = vertices[resultado[0]][1];
	float v2_x = vertices[resultado[1]][0];
	float v2_y = vertices[resultado[1]][1];
	//Calculo x inicial
	int xStart = 0;
	if(v0_x>offsetRoi){ //Chequeo para no salirme del ancho de la pantalla
		xStart = v0_x - offsetRoi;
	}
	//Calculo y inicial
	int yStart = 0;
	if(v0_y > offsetRoi){ //Chequeo para no salirme del alto de la pantalla
		yStart = v0_y -offsetRoi;
	}
	//Calculo ancho
	int widthZonaEstudio = abs( v2_x - xStart) + offsetRoi;
	if(xStart + widthZonaEstudio > xsize){ //Chequeo para no salirme del ancho de la pantalla
		printf("Borde pantalla: xStart = %d , widthZonaEstudio = %d \n",xStart,widthZonaEstudio);
		widthZonaEstudio = xsize - xStart;
	}
	//Calculo alto
	int heightZonaEstudio = abs(v2_y - yStart) + offsetRoi;
	if(yStart + heightZonaEstudio > ysize){ //Chequeo para no salirme del alto de la pantalla
		printf("Borde pantalla: yStart = %d , heightZonaEstudio = %d \n",yStart,heightZonaEstudio);
		heightZonaEstudio = ysize - yStart;
	}

	cvSetImageData( img, (uchar*)(dataPtr), img->width* img->nChannels );
	img->origin = IPL_ORIGIN_TL;
	cvZero(mono_Image);							//FIXME Es necesario? Probar sin esto. Creo que antes era necesario porque en el else del filtrado por hsv tenia mal el acceso al pixel que tenia que setear en 0. En ese caso faltaba sumar los xstart e ystart
	float area = -1;

	//Calculo el area de la zona de estudio. Segun este area vamos a obtener la cota minima y maxima del area del apuntador esperado segun la calibracion que se hay realizado
	//Por ejemplo si el apuntador ocupa 1/8 del marcador, y el area de la zona de estudio (que es aproximadamente el area del marcador) es de 800, entonces
	//buscaremos un apuntador de (800)*(1/8) = 100 +/- UMBRAL_AREA_APUNTADOR
	*areaZonaEstudio = heightZonaEstudio*widthZonaEstudio;

	if(heightZonaEstudio>min_area_width_height  &&  widthZonaEstudio > min_area_width_height){ //Ver definicion de MIN_AREA_WIDTH_HEIGHT para comprender mejor este chequeo
		area = fabs(hayObjetoColor(img,mono_Image,minH,maxH,minS,maxS,minV,maxV,ventanaMostrarImagenMono!=NULL,xStart,yStart,widthZonaEstudio,heightZonaEstudio,min_area, max_area,erode,dilate,smooth,enclosingCircle,ventanaMostrarImagenMono,ventanaMostrarImagenColor));
		if(area>0){
			printf("Area zona estudio = %f\n",*areaZonaEstudio);
			printf("Area apuntador = %f\n",area);
			/************************************************************************************************************
			 * 	DESCOMENTAR EL SIGUIENTE CODIGO PARA VER LOS SEGMENTOS DE IMAGEN EN LOS CUALES SE DETECTO EL APUNTADOR
			 *	mostrarTarget(img,xStart,yStart,widthZonaEstudio,heightZonaEstudio);
			 *	mostrarTarget(mono_Image,xStart,yStart,widthZonaEstudio,heightZonaEstudio);
			 * **********************************************************************************************************/
			//mostrarTarget(img,xStart,yStart,widthZonaEstudio,heightZonaEstudio);
			//mostrarTarget(mono_Image,xStart,yStart,widthZonaEstudio,heightZonaEstudio);

		}
	}
	return area;

}

		/******************************MANEJO DE HISTOGRAMAS**********************************/

	/*Crea un Histograma y setea su valor en 0*/
	//TODO No testeada
	CvHistogram* crearHistogramaHS(RangosColorApuntador rangos,int h_bins,int s_bins){
		int    hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { rangos.hmin, rangos.hmax +1};
		float s_ranges[] = { rangos.smin, rangos.smax +1};
		float* ranges[]    = { h_ranges, s_ranges };
		CvHistogram* hist_hs = cvCreateHist(2,hist_size,CV_HIST_ARRAY, ranges, 1);
		cvSetZero(hist_hs->bins);
		return hist_hs;
	}


	CvHistogram* crearHistograma1D(float *ranges, int bins){
		CvHistogram* resultado=NULL;
		float* ranges_array[]    = { ranges };
		resultado = cvCreateHist(1,&bins,CV_HIST_ARRAY, ranges_array, 1);
		cvSetZero(resultado->bins);
		return resultado;
	}

	void separarImagenEnPlanos(IplImage* hsv, IplImage* h_plane,IplImage* s_plane,IplImage* v_plane){
		//Separo la imagen hsv en 3 planos
		cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
	}

	int calcularHistograma(CvHistogram* hist,IplImage* plane,IplImage* mascara,int bins,float* ranges,int acumular,int debugMode){
		if(hist==NULL){ //El histograma debe estar creado antes de invocar esta funcion
			fprintf(stderr, "calcularHistograma: El histograma recibido es NULL, nada para hacer...\n");
			return ERROR_HISTOGRAMA_FALLO;
		}
		cvCalcHist( &plane, hist, acumular, mascara ); //Calculo es histograma

		if(debugMode){
			float min_val_h,max_val_h;
			int idx_min_val_h,idx_max_val_h;
			cvGetMinMaxHistValue( hist, &min_val_h, &max_val_h, &idx_min_val_h, &idx_max_val_h );

			printf("------- Histograma  -------\n");
			printf("Min value: %f\n",min_val_h);
			printf("Max value: %f\n",max_val_h);
			printf("idx Min value: %d\n",idx_min_val_h);
			printf("idx Max value: %d\n",idx_max_val_h);

			float binAncho_h = ranges[1]/bins;
			int intervaloMaximo[] = {-1,-1};
			intervaloMaximo[0] = cvFloor(idx_max_val_h*binAncho_h); 				//extremo izq del intervalo
			intervaloMaximo[1] = cvRound(intervaloMaximo[0] + binAncho_h);		//extremo derecho del intervalo
			printf("**rango maximo extraido de Histograma=[%d,%d]\n",intervaloMaximo[0],intervaloMaximo[1]);
			printf("--------------------------------\n");

			if(min_val_h < 0 || max_val_h<0){
				fprintf(stderr, "calcularHistograma: Error calculando histograma, min_val= %f, max_val= %f\n",min_val_h,max_val_h);
			}
		}
		return ERROR_HISTOGRAMA_OK;
	}



	void imprimirInfoHist(CvHistogram* hist, int nbins){
		for(int i=0;i<nbins;i++){
			printf("bin %d : %f\n",i,cvQueryHistValue_1D(hist,i));
		}
	}


	int getTotalBins1D(CvHistogram* hist,int nbins){
		int resultado = 0;
		for(int i=0;i<nbins;i++){
			resultado+=cvQueryHistValue_1D(hist,i);
		}
		return resultado;
	}

	/*Recibe un histograma al cual ya se le aplico el threshold y en resCalc deja la suma de los rangos en donde hay algun dato.
	 * En caso de que no sean todos consecutivos y la bandera continuo este en 1, devuelve {-1,-1}, es decir, no permite que haya agujeros en el histograma
	 * */
	int calcRangosHist(CvHistogram* hist, int nbins, float* ranges, int continuo,double *resCalc){
		resCalc[0]=-1;
		resCalc[1]=-1;
		int aux = 0;
		int indiceAnterior= 0;
		float anchoBin = ranges[1]/nbins;
		int error =0;
		for(int i=0;(i<nbins)&&(!error);i++){
			if(cvQueryHistValue_1D(hist,i)>0){ //Hay valores en este intervalo
				if(resCalc[0]>=0){ //El extremo izquierdo ya lo seleccione previamente
					if(continuo){ //Tengo que preguntar si la seleccion anterior fue i-1, sino quiere decir que el histograma no quedo continuo y no sirve
						if(indiceAnterior==i-1){ //No hay agujero hasta el momento, venimos del intervalo de la izq de i
							resCalc[1]=cvRound( (i+1)*anchoBin); //Extremo derecho del intervalo i
							indiceAnterior=i;
						}else{	//Hay un agujero en el hitograma y debe ser continuo, lo descarto!
							error = 1;
							resCalc[0]=-1;
							resCalc[1]=-1;
						}
					}else{ //Como el histograma no tiene porque ser continuo entonces actualizo el extremo derecho de los rangos calculados
						resCalc[1]=cvRound( (i+1)*anchoBin); //Extremo derecho del intervalo i
						indiceAnterior=i;
					}
				}else{
					resCalc[0] = cvFloor(i*anchoBin);	//Extremo izquierdo del intervalo i
					indiceAnterior = i;
					if(i==nbins-1){ //Es el ultimo intervalo y recien estableci el extremo izq, tengo que setear el extremo derecho ahora
						resCalc[1]=cvRound( (i+1)*anchoBin);
					}
				}
			}/*else{
				printf("i=%d VACIO\n",i);
			}*/
		}
		if(error){
			fprintf(stderr, "Error, el histograma no es continuo");
			return ERROR_RANGOS_HISTOGRAMA_NO_CONTINUO;
		}

		return ERROR_RANGOS_CALCULADOS_OK;
	}



	int initCvCapture(int idDispositivo){
		capture = NULL;
		capture = cvCreateCameraCapture(0);
		if(capture!=NULL){
			return 1;
		}else{
			return 0;
		}
	}

	unsigned char* getRawVideoFrame(){
		if(capture!=NULL){
			IplImage* frame = cvQueryFrame(capture);
			if(frame!=NULL){
				return (uchar*)frame->imageData;
			}
		}
	}


