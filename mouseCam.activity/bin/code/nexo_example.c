/*
 * Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 * Based on: "Send a fake keystroke event to an X window." by Adam Pierce - http://www.doctort.org/adam/
 * License at file end.
 */

#include "nexo_event.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*The key code to be sent. A full list of available codes can be found in /usr/include/X11/keysymdef.h */
#define KEYCODE XK_S
//#define KEYCODE XK_Down
//#define KEYCODE XK_Return


int main(){
	int i=5;
	while(i>0){
		   // Send a fake key press event to the window.
		   //int res = createKeyEvent(TRUE, KEYCODE, 0);
		   //sleep(1);
		   //res = createKeyEvent(FALSE, KEYCODE, 0);
		   createRelativeMouseEvent(-100,-50);	
		   sleep(1);
			mouseClick( Button1);
		   //createAbsoluteMouseEvent(0,0);
		   //sleep(1);
		i--;

	}
    return 0;
}



/************************************************************************
*
* Basic X11 events encapsulation
* Copyright 2011 Facultad de Ingenieria, Universidad de la
* Republica, Uruguay.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
************************************************************************/
