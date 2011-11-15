#@author Sebastián Marichal
# 		   sebastian_marichal@hotmail.com
# 		   sebarocker@gmail.com
#
# Universidad de la República, Montevideo , Uruguay
#
INC_DIR= ./ARToolkit/include
LIB_DIR= ./ARToolkit/lib/

INC_DIR_UTIL= ./PUI/c++
LIB_DIR_UTIL= ./PUI
BIN_DIR= .
SOURCE_DIR= ./source

LDFLAG=-pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -lcxcore -lhighgui -lgconf-2 -L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR) -L$(LIB_DIR_UTIL) -lpthread `pkg-config --libs gconf-2.0` `pkg-config --cflags --libs gtk+-2.0`
LIBS= -lARvideo -lARMulti -lAR -lpthread -lXi -lX11 -lm -lcv $(LIB_DIR_UTIL)/TimeUtils.o 
#-lARgsub -lARgsub_lite -lglut -lGLU -lGL

#Para optimizacion agresiva. Cuidado, pueden variar los resultados por la perdida de precision
CFLAG_RELEASE_AGGRESSIVE = -pipe -O2 -ffast-math -funroll-all-loops -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow  -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL)

#CFLAG_RELEASE incluye todas las banderas de optimizacion menos -funroll-all-loops
CFLAG_RELEASE= -pipe -O2 -ffast-math -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I/usr/include/gconf/2 -I$(INC_DIR) -I$(INC_DIR_UTIL)

#Banderas para debug, no optimizan (compilacion mas rapida) y generan simbolos de debug
CFLAG_DEBUG   = -pipe -g -O0 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I/usr/include/gconf/2 -I$(INC_DIR) -I$(INC_DIR_UTIL) 
#En caso de que no encuentre glib puede probar "`pkg-config --cflags glib-2.0`"

CFLAG= $(CFLAG_DEBUG) `pkg-config --cflags gconf-2.0` `pkg-config --cflags --libs gtk+-2.0`

OBJS =
HEADDERS =

all: lib_PUI nexo_event.o MouseCam

nexo_event.o: $(SOURCE_DIR)/nexo_event.c
	g++ -c $(CFLAG) $(SOURCE_DIR)/nexo_event.c

MouseCam: MouseCam.o
	g++ -o $(BIN_DIR)/MouseCam $(LIB_DIR_UTIL)/cvUtilities.o $(LIB_DIR_UTIL)/Funciones.o nexo_event.o $(LIB_DIR_UTIL)/Pui.o MouseCam.o $(OBJS) $(LDFLAG) $(LIBS) $(LDFLAGS)

MouseCam.o: $(SOURCE_DIR)/MouseCam.cpp
	g++ -c $(CFLAG) $(SOURCE_DIR)/MouseCam.cpp
clean:
	rm -f *.o
	rm -f $(BIN_DIR)/MouseCam

allclean:
	rm -f *.o
	rm -f $(BIN_DIR)/MouseCam

#Compilacion de biblioteca PUI
lib_PUI:
	$(MAKE) -C $(LIB_DIR_UTIL) all
