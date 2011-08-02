#ifndef FORMATOPIXEL_H
#define FORMATOPIXEL_H

#include "AR/config.h"
#if	AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_ARGB
#define B_COMP *(pnt+3)
#define G_COMP *(pnt+2)
#define R_COMP *(pnt+1)
#define A_COMP *(pnt+0)
#define VALORES_POR_PIXEL 4
#elif AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_ABGR
#define B_COMP *(pnt+1)
#define G_COMP *(pnt+2)
#define R_COMP *(pnt+3)
#define A_COMP *(pnt+0)
#define VALORES_POR_PIXEL 4
#elif AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_BGRA
#define B_COMP *(pnt+0)
#define G_COMP *(pnt+1)
#define R_COMP *(pnt+2)
#define A_COMP *(pnt+3)
#define VALORES_POR_PIXEL 4
#elif AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_BGR
#define B_COMP *(pnt+0)
#define G_COMP *(pnt+1)
#define R_COMP *(pnt+2)
#define VALORES_POR_PIXEL 3
#elif AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_RGBA
#define B_COMP *(pnt+2)
#define G_COMP *(pnt+1)
#define R_COMP *(pnt+0)
#define A_COMP *(pnt+3)
#define VALORES_POR_PIXEL 4
#elif AR_DEFAULT_PIXEL_FORMAT == AR_PIXEL_FORMAT_RGB
#define B_COMP *(pnt+2)
#define G_COMP *(pnt+1)
#define R_COMP *(pnt+0)
#define VALORES_POR_PIXEL 3
#else
#error formato de video no soportado
#endif

#define H_COMP *(pnth + 0)
#define S_COMP *(pnth + 1)
#define V_COMP *(pnth + 2)

#endif
