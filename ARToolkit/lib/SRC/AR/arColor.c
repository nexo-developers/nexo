#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef _WIN32
#define max(a,b) ((a)>(b) ? (a) : (b))
#define min(a,b) ((a)<(b) ? (a) : (b))
#endif
//smarichal: Pongo las variables como static para ganar el tiempo que se pierde al crear memoria para las variables
// locales
//static double D;
//static double min;
//funcion para transformar RGB a HSV
void RGBtoHSV(double r, double g, double b, double *H, double *S, double *V) {
 // double D;
 // double min;
 /* min = min(min(r, g),b);
  *V = max(max(r, g),b);
  D = *V - min;
  if(*V == 0.0) {
	  *S = 0;
  }
  else {
	  *S = D / *V;
  }
  if(*S == 0.0) {
	  *H = 0;
  }else if (r == *V) {
	  *H = 60.0 * (g - b) / D;
  }else if (g == *V){
	  *H = 120.0 + 60.0 * (b - r) / D;
  }else if (b == *V) {
	  *H = 240.0 + 60.0 * (r - g) / D;
  }
//		  printf("6\n");
  if (*H < 0.0) {
	  *H = *H + 360.0;
  }
  *S=*(S) * 100; //conversion de escala 0 a 1, a escala 0 a 100
  *V=*(V)/255 * 100;//conversion de escala 0 a 255, a escala 0 a 100
		//  printf("7\n");

  */

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
