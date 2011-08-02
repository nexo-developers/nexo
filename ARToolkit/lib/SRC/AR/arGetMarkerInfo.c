/*******************************************************
 *
 * Author: Hirokazu Kato
 *
 *         kato@sys.im.hiroshima-cu.ac.jp
 *
 * Revision: 3.1
 * Date: 01/12/07
 *
*******************************************************/

#include <AR/ar.h>

static ARMarkerInfo    marker_infoL[AR_SQUARE_MAX];
static ARMarkerInfo    colorMarker_info[AR_SQUARE_MAX];
static ARMarkerInfo    marker_infoR[AR_SQUARE_MAX];

ARMarkerInfo *arGetMarkerInfo( ARUint8 *image,
                               ARMarkerInfo2 *marker_info2, int *marker_num )
{
    int            id, dir;
    double         cf;
    int            i, j;

    for (i = j = 0; i < *marker_num; i++) {
        marker_infoL[j].area   = marker_info2[i].area;
        marker_infoL[j].pos[0] = marker_info2[i].pos[0];
        marker_infoL[j].pos[1] = marker_info2[i].pos[1];

        if (arGetLine(marker_info2[i].x_coord, marker_info2[i].y_coord,
                      marker_info2[i].coord_num, marker_info2[i].vertex,
                      marker_infoL[j].line, marker_infoL[j].vertex) < 0 ) continue;

        arGetCode(image,
                  marker_info2[i].x_coord, marker_info2[i].y_coord,
                  marker_info2[i].vertex, &id, &dir, &cf );

        marker_infoL[j].id  = id;
        marker_infoL[j].dir = dir;
        marker_infoL[j].cf  = cf;

        j++;
    }
    *marker_num = j;

    return (marker_infoL);
}

ARMarkerInfo *arGetColorElementInfo( ARUint8 *image,
                               ARMarkerInfo2 *marker_info2, int *marker_num, int *clip )
{
	int dir = 0; 	//Los elementos detectados por color no tienen una forma definida para establecer una rotacion
    int            id = -10; //Los elementos detectados por color no tienen una propiedad que pueda distinguirlos unos de otros, por lo tanto a todos les establecemos el id -10
    double         cf = 1;   //TODO Ver como puedo obtener un cf para estos elementos
    int            i, j;

    for (i = j = 0; i < *marker_num; i++) {
    	colorMarker_info[j].area   = marker_info2[i].area;
    	colorMarker_info[j].pos[0] = marker_info2[i].pos[0];
    	colorMarker_info[j].pos[1] = marker_info2[i].pos[1];

    	//FIXME Probamos definir los vertex como los vertices del clip a ver que pasa??
    	// El marker2.vertex tiene solo los indices de los vertices en x_coord e y_coord.
    	// Si quiero usar los del clip tengo que buscarlos en x_coord e y_coord
    	/*marker_info2[j].vertex[0][0] = marker_info2[i].x_coord[marker_info2[i].vertex[0]];
    	marker_info2[j].vertex[0][1] = marker_info2[i].y_coord[marker_info2[i].vertex[0]];

    	marker_info2[j].vertex[1][0] = marker_info2[i].x_coord[marker_info2[i].vertex[1]];
    	marker_info2[j].vertex[1][1] = marker_info2[i].y_coord[marker_info2[i].vertex[1]];

    	marker_info2[j].vertex[2][0] = marker_info2[i].x_coord[marker_info2[i].vertex[2]];
    	marker_info2[j].vertex[2][1] = marker_info2[i].y_coord[marker_info2[i].vertex[2]];

    	marker_info2[j].vertex[3][0] = marker_info2[i].x_coord[marker_info2[i].vertex[3]];
    	marker_info2[j].vertex[3][1] = marker_info2[i].y_coord[marker_info2[i].vertex[3]];
*/
        if (arGetLine(marker_info2[i].x_coord, marker_info2[i].y_coord,
                      marker_info2[i].coord_num, marker_info2[i].vertex,
                      colorMarker_info[j].line, colorMarker_info[j].vertex) < 0 ) continue;

        /*arGetCode(image,
                  marker_info2[i].x_coord, marker_info2[i].y_coord,
                  marker_info2[i].vertex, &id, &dir, &cf );*/

        colorMarker_info[j].id  = id;
        colorMarker_info[j].dir = dir;
        colorMarker_info[j].cf  = cf;

        j++;
    }
    *marker_num = j;

    return (colorMarker_info);
}

ARMarkerInfo *arsGetMarkerInfo( ARUint8 *image,
                                ARMarkerInfo2 *marker_info2, int *marker_num, int LorR )
{
    ARMarkerInfo   *info;
    int            id, dir;
    double         cf;
    int            i, j;

    if (LorR) info = &marker_infoL[0];
	else      info = &marker_infoR[0];

    for (i = j = 0; i < *marker_num; i++) {
        info[j].area   = marker_info2[i].area;
        info[j].pos[0] = marker_info2[i].pos[0];
        info[j].pos[1] = marker_info2[i].pos[1];

        if (arsGetLine(marker_info2[i].x_coord, marker_info2[i].y_coord,
                       marker_info2[i].coord_num, marker_info2[i].vertex,
                       info[j].line, info[j].vertex, LorR) < 0 ) continue;

        arGetCode(image,
                  marker_info2[i].x_coord, marker_info2[i].y_coord,
                  marker_info2[i].vertex, &id, &dir, &cf );

        info[j].id  = id;
        info[j].dir = dir;
        info[j].cf  = cf;

        j++;
    }
    *marker_num = j;

    return (info);
}

