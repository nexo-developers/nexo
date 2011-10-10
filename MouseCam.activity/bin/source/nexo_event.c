/*
 * Facultad de Ingenieria, Universidad de la Republica, Uruguay.
 * Andres Aguirre, Lorena Calvo, Guillermo Fernandez, Maria Emilia Silveira
 * Based on: "Send a fake keystroke event to an X window." by Adam Pierce - http://www.doctort.org/adam/
 * License at file end.
 */


// para compilar: g++ -o XFakeKey XFakeKey.cpp -L/usr/X11R6/lib -lX11
#include "nexo_event.h"

/* Function to create a keyboard event */
int createKeyEvent(char press, int keycode, int modifiers){

   /* Obtain the X11 display. */
   Display *display = XOpenDisplay(0);
   if(display == 0)
      return -1;

   /* Get the root window for the current display.*/
   Window winRoot = XDefaultRootWindow(display);
   /* Find the window which has the current keyboard focus.*/
   Window winFocus;
   int    revert;
   XGetInputFocus(display, &winFocus, &revert);

   XKeyEvent event;	
   event.display     = display;
   event.window      = winFocus;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = TRUE;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press == TRUE)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   // Send a fake key press event to the window.
   XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
   XFlush(display);	
   XCloseDisplay(display);
   return 0;	
}

/* Function to create a mouse event that moves the mouse pointer delta_x and delta_y relative from current position*/
int createRelativeMouseEvent(int delta_x, int delta_y){

   /* Obtain the X11 display. */
   Display *display = XOpenDisplay(0);
   if(display == 0)
      return -1;

   /* Get the root window for the current display.*/
   Window winRoot = XDefaultRootWindow(display);

   int x, y, tmp;
   unsigned int tmp_u;
   Window fromroot, tmpwin;
   XQueryPointer(display, winRoot, &fromroot, &tmpwin, &x, &y, &tmp, &tmp, &tmp_u);
   x += delta_x;
   y += delta_y;
   XWarpPointer(display, None, winRoot, 0, 0, 0, 0, x, y);
   XFlush(display);	
   XCloseDisplay(display);
   return 0;
}

/* Function to create a mouse event that moves the mouse pointer to the x, y position*/
int createAbsoluteMouseEvent(int x, int y){

   /* Obtain the X11 display. */
   Display *display = XOpenDisplay(0);
   if(display == 0)
      return -1;

   /* Get the root window for the current display.*/
   Window winRoot = XDefaultRootWindow(display);
 
   XWarpPointer(display, None, winRoot, 0, 0, 0, 0, x, y);
   XFlush(display);	
   XCloseDisplay(display);
   return 0;
}

int mouseClick(int button)
{
	Display *display = XOpenDisplay(NULL);

	XEvent event;
	
	if(display == NULL)
	   return -1;
	
	memset(&event, 0x00, sizeof(event));
	event.type = ButtonPress;
	event.xbutton.button = button;
	event.xbutton.same_screen = True;
	
	XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	
	event.xbutton.subwindow = event.xbutton.window;
	
	while(event.xbutton.subwindow)
	{
		event.xbutton.window = event.xbutton.subwindow;
		
		XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	}
	//TODO FIX this kind of error handling, it is better return the error to the calling code
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
	
	XFlush(display);
	
	usleep(100000);
	
	event.type = ButtonRelease;
	event.xbutton.state = 0x100;
	
	//TODO FIX this kind of error handling, it is better return the error to the calling code
	if(XSendEvent(display, PointerWindow, True, 0xfff, &event) == 0) fprintf(stderr, "Errore nell'invio dell'evento !!!\n");
	
	XFlush(display);
	
	XCloseDisplay(display);
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
