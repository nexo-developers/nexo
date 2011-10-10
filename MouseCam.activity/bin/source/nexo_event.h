/*
 * Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 * Based on: "Send a fake keystroke event to an X window." by Adam Pierce - http://www.doctort.org/adam/
 * License at file end.
 */

#ifndef _NEXOEVENT_H_
#define _NEXOEVENT_H_

#include <stddef.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#define FALSE 0
#define TRUE !FALSE

/* Function to create a keyboard event */
int createKeyEvent(char press, int keycode, int modifiers);

/* Function to create a mouse event that moves the mouse pointer delta_x and delta_y relative from current position*/
int createRelativeMouseEvent(int delta_x, int delta_y);

/* Function to create a mouse event that moves the mouse pointer to the x, y position*/
int createAbsoluteMouseEvent(int x, int y);

/* Function to create a mouse click*/
int mouseClick(int button);

#endif // _NEXOEVENT_H_


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
