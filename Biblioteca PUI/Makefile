#@author Sebastián Marichal
# 		   sebastian_marichal@hotmail.com
# 		   sebarocker@gmail.com
#
# Universidad de la República, Montevideo , Uruguay
#
INC_DIR= ../ARToolkit/include
LIB_DIR= ../ARToolkit/lib/
SRC_DIR= c++
BIN_DIR= ../bin

LDFLAG=-pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR)
LIBS= -lARgsub -lARgsub_lite -lARvideo -lARMulti -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -lXmu -lcv -lcvaux -lSDL -lSDL_mixer -lSDL_gfx
#(XO) LDFLAG=-pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -L/usr/X11R6/lib -L/usr/local/lib -L$(LIB_DIR)
#(XO) LIBS= -lARgsub -lARvideo -lARMulti -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -lcv -lcvaux -lhighgui -lSDL -lSDL_mixer -lSDL_gfx


#Para optimizacion agresiva. Cuidado, pueden variar los resultados por la perdida de precision
CFLAG_RELEASE_AGGRESIVE= -pipe -O2 -ffast-math -funroll-all-loops -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)
#Para optimizacion agresiva. Cuidado, pueden variar los resultados por la perdida de precision
#(XO) CFLAG_RELEASE_AGGRESIVE = -pipe -O2 -ffast-math -funroll-all-loops -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)


#CFLAG_RELEASE incluye todas las banderas de optimizacion menos -funroll-all-loops
CFLAG_RELEASE= -fPIC -pipe -O2 -ffast-math -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)
#CFLAG_RELEASE incluye todas las banderas de optimizacion menos -funroll-all-loops
#CFLAG_RELEASE = -fPIC -pipe -O2 -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)
#(XO) CFLAG_RELEASE = -fPIC -pipe -O2 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)

#Banderas para debug, no optimizan (compilacion mas rapida) y generan simbolos de debug
CFLAG_DEBUG   = -pipe -g -O0 -fPIC -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)
#Banderas para debug, no optimizan (compilacion mas rapida) y generan simbolos de debug
#(XO) CFLAG_DEBUG = -pipe -g -O0 -fPIC -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)

CFLAG= $(CFLAG_DEBUG)
#(XO) CFLAG = $(CFLAG_RELEASE)

OBJS =
HEADDERS =

all: Pui.o Funciones.o cvUtilities.o lib_artoolkit
#(XO) all: Pui.o TimeUtils.o OpenCVCamera.o Funciones.o UtilidadesSDL.o cvUtilities.o Utilidades.o ProcesamientoImagenRangosCV ProcesamientoImagenRangosMemoria ProcesamientoImagenRangosMemoriaEstatica findCirclesTestParaCalibracion histogramasEnROI

Pui.o: $(SRC_DIR)/Pui.cpp cvUtilities.o Funciones.o TimeUtils.o
	g++ -c $(CFLAG) -DGLUT_HABILITADO=-1 $(SRC_DIR)/Pui.cpp

TimeUtils.o: $(SRC_DIR)/TimeUtils.cpp
	g++ -c $(CFLAG) $(SRC_DIR)/TimeUtils.cpp

Funciones.o: $(SRC_DIR)/Funciones.cpp 
	g++ -c $(CFLAG) $(SRC_DIR)/Funciones.cpp

cvUtilities.o: $(SRC_DIR)/cvUtilities.cpp Funciones.o
	g++ -c $(CFLAG) $(SRC_DIR)/cvUtilities.cpp

clean:
	rm -f *.o

#guille############## MakefileXO


OpenCVCamera.o: OpenCVCamera.h OpenCVCamera.cpp
	g++ -c $(CFLAG) OpenCVCamera.cpp

Utilidades.o: Utilidades.c cvUtilities.o Funciones.o
	g++ -c $(CFLAG) Utilidades.c
	
	
UtilidadesSDL.o: UtilidadesSDL.cpp
	g++ -c $(CFLAG) UtilidadesSDL.cpp

ProcesamientoImagenRangosCV.o: ProcesamientoImagenRangosCV.cpp
	g++ -c $(CFLAG) ProcesamientoImagenRangosCV.cpp

ProcesamientoImagenRangosCV: ProcesamientoImagenRangosCV.o
	g++ -o $(BIN_DIR)/ProcesamientoImagenRangosCV ProcesamientoImagenRangosCV.o  $(OBJS) $(LDFLAG) $(LIBS)

ProcesamientoImagenRangosMemoria.o: ProcesamientoImagenRangosMemoria.cpp
	g++ -c $(CFLAG) ProcesamientoImagenRangosMemoria.cpp

ProcesamientoImagenRangosMemoria: ProcesamientoImagenRangosMemoria.o
	g++ -o $(BIN_DIR)/ProcesamientoImagenRangosMemoria cvUtilities.o Utilidades.o UtilidadesSDL.o Funciones.o ProcesamientoImagenRangosMemoria.o  $(OBJS) $(LDFLAG) $(LIBS)

ProcesamientoImagenRangosMemoriaEstatica.o: ProcesamientoImagenRangosMemoriaEstatica.cpp
	g++ -c $(CFLAG) ProcesamientoImagenRangosMemoriaEstatica.cpp

ProcesamientoImagenRangosMemoriaEstatica: ProcesamientoImagenRangosMemoriaEstatica.o
	g++ -o $(BIN_DIR)/ProcesamientoImagenRangosMemoriaEstatica cvUtilities.o Utilidades.o UtilidadesSDL.o Funciones.o ProcesamientoImagenRangosMemoriaEstatica.o  $(OBJS) $(LDFLAG) $(LIBS)

findCirclesTestParaCalibracion: findCirclesTestParaCalibracion.o cvUtilities.o
	g++ -o $(BIN_DIR)/findCirclesTestParaCalibracion cvUtilities.o Utilidades.o UtilidadesSDL.o Funciones.o findCirclesTestParaCalibracion.o  $(OBJS) $(LDFLAG) $(LIBS)

findCirclesTestParaCalibracion.o : findCirclesTestParaCalibracion.cpp
	g++ -c $(CFLAG) findCirclesTestParaCalibracion.cpp

histogramasEnROI.o: histogramasEnROI.cpp
	g++ -c $(CFLAG) histogramasEnROI.cpp

histogramasEnROI: histogramasEnROI.o
	g++ -o $(BIN_DIR)/histogramasEnROI histogramasEnROI.o   $(OBJS) $(LDFLAG) $(LIBS)


#guille############################################3 Python


SWIG=swig
I_DIR=wrapper_interface


OUTDIR=python

#CFLAGS=-DRELEASE -fPIC -pipe -O2 -ffast-math -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)   -DGLUT_HABILITADO=1 -I/usr/include/python2.6 
#(XO)             -fpic           -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I../../ARToolkit/include   -DGLUT_HABILITADO=1 -I/usr/include/python2.5 -I/usr/lib/python2.5/site-packages/numpy/core/include

#CFLAGS_DEBUG=-DDEBUG -fPIC -pipe -g -O0 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv -I$(INC_DIR)   -DGLUT_HABILITADO=1 -I/usr/include/python2.6
CFLAGS_DEBUG += -I/usr/include/python2.6 -I../ARToolkit/include


python: _PuiModule.so

Pui_wrap.o: $(I_DIR)/Pui.i $(SRC_DIR)/Pui.cpp $(SRC_DIR)/Pui.h
	$(SWIG) -Wall -c++ -python $(I_DIR)/Pui.i 
	g++ -c $(CFLAGS_DEBUG) $(I_DIR)/Pui_wrap.cxx -I$(SRC_DIR)

_PuiModule.so: Pui.o Pui_wrap.o lib_artoolkit
	g++ -shared Pui.o Pui_wrap.o -lstdc++ -pthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -L$(LIB_DIR) -lARgsub -lARgsub_lite -lARvideo -lARMulti -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -lXmu -lcv -lcvaux cvUtilities.o Funciones.o TimeUtils.o -o _PuiModule.so 
	# include in PYTHONXO                      -L../../Utilidades/src                                                           -lhighgui -lSDL -lSDL_mixer -lSDL_gfx cvUtilities.o Funciones.o TimeUtils.o -o _PuiModule.so 
	mv Pui_wrap.* _PuiModule.so $(I_DIR)/PuiModule.py $(OUTDIR)

cleanpython:
	rm -f Pui.o $(OUTDIR)/Pui_wrap.* $(OUTDIR)/_PuiModule.so $(OUTDIR)/PuiModule.py

lib_artoolkit:
	$(MAKE) -C $(LIB_DIR)/SRC all


# PHONY TARGETS
.PHONY: all clean python cleanpython lib_artoolkit


