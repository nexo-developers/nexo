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

LDFLAG=-pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -lcxcore -lhighgui -L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR) -L$(LIB_DIR_UTIL)
LIBS= -lARgsub -lARgsub_lite -lARvideo -lARMulti -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -lXmu -lcv -lcvaux -lSDL -lSDL_mixer -lSDL_gfx $(LIB_DIR_UTIL)/TimeUtils.o

#Para optimizacion agresiva. Cuidado, pueden variar los resultados por la perdida de precision
CFLAG_RELEASE_AGGRESSIVE = -pipe -O2 -ffast-math -funroll-all-loops -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow  -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL)

#CFLAG_RELEASE incluye todas las banderas de optimizacion menos -funroll-all-loops
CFLAG_RELEASE= -pipe -O2 -ffast-math -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL)

#Banderas para debug, no optimizan (compilacion mas rapida) y generan simbolos de debug
CFLAG_DEBUG   = -pipe -g -O0 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL) 

CFLAG= $(CFLAG_DEBUG)

OBJS =
HEADDERS =

all: lib_PUI UtilidadesSDL.o nexo_event.o MouseCam

UtilidadesSDL.o: $(SOURCE_DIR)/UtilidadesSDL.cpp
	g++ -c $(CFLAG) $(SOURCE_DIR)/UtilidadesSDL.cpp

nexo_event.o: $(SOURCE_DIR)/nexo_event.c
	g++ -c $(CFLAG) $(SOURCE_DIR)/nexo_event.c

MouseCam: MouseCam.o
	g++ -o $(BIN_DIR)/MouseCam $(LIB_DIR_UTIL)/cvUtilities.o $(LIB_DIR_UTIL)/Funciones.o UtilidadesSDL.o nexo_event.o $(LIB_DIR_UTIL)/Pui.o MouseCam.o $(OBJS) $(LDFLAG) $(LIBS) $(LDFLAGS)

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
