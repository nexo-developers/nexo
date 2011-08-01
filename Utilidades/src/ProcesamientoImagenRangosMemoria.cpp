// Librerias
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "highgui.h"
#include "cv.h"
#include "cvUtilities.h"

// Variables para la modificacion de valores HSV
// Estos valores segmentan tonalidades verde limon
int hlower = 100; // Valores iniciales de Hue
int hupper = 141;
int SaturationMax = 255;
int Saturation = 100;
int BrightnessMax = 255;
int Brightness = 0;
int Dilate = 0;
int Erode = 0;
int Smooth = 0;
int Canny = 0;
int SmoothType = CV_BLUR;
int approxMethod = CV_CHAIN_APPROX_SIMPLE;

int maxLevelDrawCountour = 0;
int thickness = 1;
int dp = 2;
int min_radio = 5;
int max_radio = 300;

int enclosingCircle = 0;
int findCircles = 0;
// Metodos para switch controlers

void switch_enclosingCirle( int position ) {

	enclosingCircle = position;

}

void switch_findCircle( int position ) {

	findCircles = position;

}

void switch_minRadio( int position ) {

	min_radio = position;

}

void switch_maxRadio( int position ) {

	max_radio = position;

}

void switch_dp( int position ) {
	if(position<1){
		dp=position;
	}else{
		dp = position;
	}

}

void switch_maxLevelDrawCountour( int position ) {

	maxLevelDrawCountour = position;

}

void switch_thickness( int position ) {

	thickness = position;

}

void switch_canny( int position ) {

    Canny = position;

}


void switch_hlower( int position ) {

    hlower = position;

}
void switch_hupper( int position ) {

    hupper = position;

}
void switch_saturation( int position ) {

    Saturation = position;

}

void switch_saturationmax( int position ) {

    SaturationMax = position;

}

void switch_brightness( int position ) {

    Brightness = position;

}

void switch_brightnessmax( int position ) {

    BrightnessMax = position;

}

void switch_erode( int position ) {

    Erode = position;

}

void switch_dilate( int position ) {

    Dilate = position;

}

void switch_smooth( int position ) {
    Smooth = position;
}


// Metodo Principal
int main( int argc, char** argv ) {


// Declaracion de variables para manipulacion de elementos

    int height , width , step , channels , k = 1;
    int step_mono , channels_mono;

    uchar *data , *data_mono;
    bool first = true;

 // Declaramos una Variable para poder obtener una imagen del video
IplImage *frame ,  *image , *mono_Image, *contornos;
CvScalar hsv_min;
CvScalar hsv_max;

CvMemStorage* storage = cvCreateMemStorage(0);
CvMemStorage* circleStorage = cvCreateMemStorage(0);
 // Creamos interface de Usuario ..........

   // Ventanas
   cvNamedWindow( "Imagen Fuente", 0 );
   cvResizeWindow("Imagen Fuente",550,1000);
   cvNamedWindow( "Imagen en HSV", 0 );
   cvResizeWindow("Imagen en HSV",550,1000);
   cvNamedWindow( "Img Mono", 0 );
   cvResizeWindow("Img Mono",550,800);
   cvNamedWindow( "Contornos", 0 );
   cvResizeWindow("Contornos",500,500);

   // TrackBar

   cvCreateTrackbar("Hue Upper","Imagen en HSV",&hupper,390,switch_hupper);
   cvCreateTrackbar("Hue Lower","Imagen en HSV",&hlower,360,switch_hlower);

   cvCreateTrackbar("SaturationMax","Imagen en HSV",&SaturationMax,100,switch_saturationmax);
   cvCreateTrackbar("SaturationMin","Imagen en HSV",&Saturation,100,switch_saturation);

   cvCreateTrackbar("BrightnessMax","Imagen en HSV",&BrightnessMax,100,switch_brightnessmax);
   cvCreateTrackbar("BrightnessMin","Imagen en HSV",&Brightness,100,switch_brightness);


   // Dilatacion y Erocion

    cvCreateTrackbar("Erode","Img Mono",&Erode,10,switch_erode);
    cvCreateTrackbar("Dilate","Img Mono",&Dilate,20,switch_dilate);
    cvCreateTrackbar("Smooth","Img Mono",&Smooth,1,switch_smooth);
    cvCreateTrackbar("Canny","Img Mono",&Canny,1,switch_canny);
    cvCreateTrackbar("MaxLevelDraw contour","Imagen Fuente",&maxLevelDrawCountour,20,switch_maxLevelDrawCountour);
    cvCreateTrackbar("Thickness DrawContour","Imagen Fuente",&thickness,40,switch_thickness);
    cvCreateTrackbar("Dp","Imagen Fuente",&dp,10,switch_dp);
    cvCreateTrackbar("Min Radio","Imagen Fuente",&min_radio,300,switch_minRadio);
    cvCreateTrackbar("Max Radio","Imagen Fuente",&max_radio,300,switch_maxRadio);
    cvCreateTrackbar("Enclosing Circle","Imagen Fuente",&enclosingCircle,1,switch_enclosingCirle);
    cvCreateTrackbar("Find Circle","Imagen Fuente",&findCircles,1,switch_findCircle);




// Iniciamos el Dispositivo de Captura de Video
   CvCapture* capture = cvCreateCameraCapture( atoi(argv[1]));


   while(1) {

      frame = cvQueryFrame( capture ); // Obtenemos Frame desde el Buffer de Video
      if( !frame ) break; // Si no Obtenemos Frame , Entonces Finalizamos el Ciclo

  // Obtenemos atributos de la  primera imagen Obtenida.
      if (first) {

        // Creamos 2 imagenes partiendo de la imagen Fuente
        image = cvCreateImage( cvGetSize(frame), 8, 3 );
        mono_Image = cvCreateImage( cvGetSize(frame), 8, 1 );
        contornos = cvCreateImage( cvGetSize(frame), 8, 1 );

        // Obtenemos atributos de la imagen HSV
           height     = image->height;
           width      = image->width;
           step       = image->widthStep/sizeof(uchar);
           channels   = image->nChannels;

           step_mono   = mono_Image->widthStep/sizeof(uchar);
           channels_mono = mono_Image->nChannels;

           first = false;
      }

      // No convertimos los colores, solo copiamos la imagen
      cvCopy(frame,image,NULL);

      // Obtenemos los valores RGB de la Imagen
	data = (uchar *)image->imageData;
	data_mono = (uchar *)mono_Image->imageData;


   //hsv_min = cvScalar((double)hlower,(double)Saturation,(double)Brightness,0);
   //hsv_max = cvScalar((double)(hupper +1),(double)(SaturationMax+1),(double)(BrightnessMax+1),0);
   //cvInRangeS(image,hsv_min, hsv_max,mono_Image);

   double H,S,V;
   for(int i = 0; i <height; i++ ) {
   		 for(int j = 0; j <width; j++ ) {
   			 //ATENCION: La imagen que trae cvQueryFrame esta en formato BGR en lugar de RGB, por lo tanto el canal Red es +2, el Blue es +0 y el Green es +1
   		  RGBtoHSVDouble((double)data[(i)*step+ (j)*channels +2],(double)data[(i)*step+ (j)*channels+1],(double)data[(i)*step+ (j)*channels + 0], &H,&S, &V);

   		  //----Esto es nuevo para ver la imagen en hsv desplegada en pantalla
   		  image->imageData[(i)*step+ (j)*channels] = H;
   		  image->imageData[(i)*step+ (j)*channels +1] = S;
   		  image->imageData[(i)*step+ (j)*channels +2] = V;
   		  //------------------------------------------------------------------
   		  // printf(" (%f,%f,%f)\n",(double)H,(double)S,(double)V);
   	   // Segmentamos la imagen mediante los angulos de Hue
   		  if(hupper>360){
   			  if ( ((H >= hlower) && ((H) <= hupper)) || H<=(hupper-360) ){
   						if ( (S>= Saturation)&&(S<=SaturationMax) ) {
   							if ( (V>= Brightness)&&(V<=BrightnessMax)) {
   								// Coloreamos el pixel en negro
   								data_mono[(i)*step_mono+(j)] = 255;
   							}else {
   								// Coloreamos el pixel en blanco
   								data_mono[(i)*step_mono+(j)] = 0;
   							}
   						}else{
   							// Coloreamos el pixel en blanco
   							data_mono[(i)*step_mono+(j)] = 0;
   						}
   				//printf("negro\n");
   			  } else {
   				   // Coloreamos el pixel en blanco
   				   //printf("blanco\n");
   				   data_mono[i*step_mono+j*channels_mono] = 0;
   			  }
   		  }else{
   			  if ( (H >= hlower) && ((H) <= hupper) ){
					if ( (S>= Saturation)&&(S<=SaturationMax) ) {
						if ( (V>= Brightness)&&(V<=BrightnessMax)) {
							// Coloreamos el pixel en negro
							data_mono[(i)*step_mono+(j)] = 255;
						}else {
							// Coloreamos el pixel en blanco
							data_mono[(i)*step_mono+(j)] = 0;
						}
					}else{
						// Coloreamos el pixel en blanco
						data_mono[(i)*step_mono+(j)] = 0;
					}
   			  } else {
   				   // Coloreamos el pixel en blanco
   				   //printf("blanco\n");
   				   data_mono[i*step_mono+j*channels_mono] = 0;
   			  }
   		  }
   	   } // fin del for j
   	 }
// Erocionar y Dilatar , para la eliminacion de pixeles perdidos

	if(Erode>0){
		cvErode(mono_Image,mono_Image,0,Erode);
	}
	if(Dilate>0){
		cvDilate( mono_Image,mono_Image,0,Dilate);
	}
	if(Smooth>0){
		cvSmooth(mono_Image,mono_Image,SmoothType,7,7);
	}

	CvSeq* contours = NULL;
	cvCopy(mono_Image,contornos,NULL);
	if(Canny>0){
		cvCanny(mono_Image,contornos,50,200,3);
	}

	int cantContornos = cvFindContours( contornos, storage, &contours, sizeof(CvContour),CV_RETR_TREE, approxMethod, cvPoint(0,0) );
	//printf("Contornos detectados: %d\n",cantContornos);

	cvDrawContours(frame,contours,cvScalar(0,255,0,0),cvScalar(0,0,255,0),maxLevelDrawCountour,thickness,8);

	//Deteccion de circulos. Sobre la imagen contornos que es a la que se le aplica el Canny
	if(findCircles){
		CvSeq* circles =  cvHoughCircles(contornos,circleStorage,CV_HOUGH_GRADIENT,dp,100,100,300,min_radio,max_radio);
		printf("Circulos detectados: %d\n",circles->total);

		for(int i=0;i<circles->total;i++){
			float* p = (float*)cvGetSeqElem( circles, i );

			cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])), 3, CV_RGB(255,0,0), -1, 8, 0 );

			cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])), cvRound(p[2]), CV_RGB(200,0,0), 1, 8, 0 );

			//px[i]=cvRound(p[0]); py[i]=cvRound(p[1]);
		}
	}
	CvSeq* iter = contours;
	CvPoint2D32f centro;
	float radio;
	if(enclosingCircle){
		printf("Encontrando enclosing circles: \n");
		while(iter){
			cvMinEnclosingCircle(iter,&centro,&radio);
			cvCircle(frame,cvPoint(centro.x,centro.y),cvRound(radio),CV_RGB(255,0,0),thickness + 2,8,0 );

			printf("*** Centro : (%f,%f) , Radio : %f \n",centro.x,centro.y,radio);
			printf("*** AREA   : %f \n",pow(radio,2)*CV_PI);
			iter = iter->h_next;
		}

	}


// Desplegamos la imagenes en Ventanas

    cvShowImage( "Imagen Fuente", frame );
    cvShowImage( "Imagen en HSV", image );
    cvShowImage( "Img Mono", mono_Image );
    cvShowImage( "Contornos", contornos );


      // Esperamos 33 milesimas de Segundo , si el usuario presiona alguna Tecla
      char c = cvWaitKey(33);
      if( c == 27 ) break; // Si se Preciono la Tecla ESC entonces se rompe el Ciclo
      if(c == 's'){ //Seleccion tipo de smooth
    	  printf("Seleccionar tipo de Smooth:\n");
    	  printf(" 1 - Blur sin escalado (CV_BLUR_NO_SCALE)\n");
    	  printf(" 2 - Blur con escalado (CV_BLUR)\n");
    	  printf(" 3 - Gaussian (CV_GAUSSIAN)\n");
    	  printf(" 4 - Mediana (CV_MEDIAN)\n");
    	  int tipo = 0;
    	  scanf("%d",&tipo); while( getchar()!='\n' );
    	  switch(tipo){
			  case 1:
				  SmoothType = CV_BLUR_NO_SCALE;
			  break;
			  case 2:
				  SmoothType = CV_BLUR;
			  break;
			  case 3:
				  SmoothType = CV_GAUSSIAN;
			  break;
			  case 4:
				  SmoothType = CV_MEDIAN;
			  break;
    	  }
		  printf("\n");
      }
      if(c=='c'){//configuracion del metodo para aproximar contornos
    	  printf("Seleccionar Metodo para extraer contornos:\n");
		  printf(" 1 - CV_CHAIN_CODE (Freeman chain code)\n");
		  printf(" 2 - CV_CHAIN_APROX_NONE (No aproxima)\n");
		  printf(" 3 - CV_CHAIN_APPROX_SIMPLE (Comprime lineas horiz, vert y diag. Deja solo puntos finales)\n");
		  printf(" 4 - CV_CHAIN_APPROX_TC89_L1 (Algoritmo de aproximacion de Teh-Chin)\n");
		  printf(" 5 - CV_CHAIN_APPROX_TC89_KCOS (Otro Algoritmo de aproximacion de Teh-Chin)\n");
		  printf(" 6 - CV_LINK_RUNS (Une segmentos horizontals de unos. Solo se puede usar con CV_RETR_LIST)\n");
		  int tipo=0;
		  scanf("%d",&tipo); while( getchar()!='\n' );
		  switch(tipo){
			  case 1:
				  approxMethod= CV_CHAIN_CODE;
			  break;
			  case 2:
				  approxMethod= CV_CHAIN_APPROX_NONE;
			  break;
			  case 3:
				  approxMethod= CV_CHAIN_APPROX_SIMPLE;
			  break;
			  case 4:
				  approxMethod = CV_CHAIN_APPROX_TC89_L1;
			  break;
			  case 5:
				  approxMethod = CV_CHAIN_APPROX_TC89_KCOS;
			  break;
			  case 6:
				  approxMethod = CV_LINK_RUNS;
			  break;
		  }
		  printf("\n");
      }
   }

   // Liberamos Variables en Memoria
   cvReleaseImage(&frame);
   cvReleaseImage(&image);
   cvReleaseImage(&mono_Image);
   cvReleaseImage(&contornos);

   cvReleaseMemStorage(&storage);
   cvReleaseMemStorage(&circleStorage);

   cvReleaseCapture(&capture );
	cvDestroyWindow( "Imagen Fuente" );
	cvDestroyWindow( "Imagen en HSV" );
	cvDestroyWindow( "Img Mono" );
	cvDestroyWindow( "Contornos" );
} // fin del metodo principal
