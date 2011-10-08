#@author Sebastián Marichal
# 		   sebastian_marichal@hotmail.com
# 		   sebarocker@gmail.com
#
# Universidad de la República, Montevideo , Uruguay
#
INC_DIR= ../../../ARToolkit/include
LIB_DIR= ../../../ARToolkit/lib/

INC_DIR_UTIL= ../../c++
LIB_DIR_UTIL= ../..
BIN_DIR= ../../bin

LDFLAG= $(LDFLAG_CROSS) -lpthread -lgstreamer-0.10 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lxml2 -lglib-2.0 -L$(LIB_DIR) -L$(LIB_DIR_UTIL)
LIBS= -lARgsub -lARgsub_lite -lARvideo -lARMulti -lAR -lpthread -lglut -lGLU -lGL -lXi -lX11 -lm -lXmu -lcv -lcvaux -lcxcore -lSDL -lSDL_mixer -lSDL_gfx -lhighgui $(LIB_DIR_UTIL)/TimeUtils.o

#Para optimizacion agresiva. Cuidado, pueden variar los resultados por la perdida de precision
CFLAG_RELEASE_AGGRESSIVE = -pipe -O2 -ffast-math -funroll-all-loops -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow  -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL)

#CFLAG_RELEASE incluye todas las banderas de optimizacion menos -funroll-all-loops
CFLAG_RELEASE= -pipe -O2 -ffast-math -march=native -mtune=native -msse -msse2 -msse3 -mmmx -m3dnow -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL)

#Banderas para debug, no optimizan (compilacion mas rapida) y generan simbolos de debug
CFLAG_DEBUG   = -pipe -g -O0 -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -I/usr/include/opencv  -I$(INC_DIR) -I$(INC_DIR_UTIL) 

CFLAG= $(CFLAG_CROSS) $(CFLAG_DEBUG)

OBJS =
HEADDERS =

all:  UtilidadesSDL.o $(BIN_DIR)/CalibradorColorOO $(BIN_DIR)/TocarAnimalesPUI $(BIN_DIR)/MostrarAnimalesPUI

UtilidadesSDL.o: UtilidadesSDL.cpp
	g++ -c $(CFLAG) UtilidadesSDL.cpp

$(BIN_DIR)/CalibradorColorOO: CalibradorColorOO.o
	g++ -o $(BIN_DIR)/CalibradorColorOO $(LIB_DIR_UTIL)/cvUtilities.o $(LIB_DIR_UTIL)/Funciones.o $(LIB_DIR_UTIL)/Pui.o CalibradorColorOO.o $(OBJS) $(LDFLAG) $(LIBS)

$(BIN_DIR)/TocarAnimalesPUI: TocarAnimalesPUI.o
	g++ -o $(BIN_DIR)/TocarAnimalesPUI $(LIB_DIR_UTIL)/cvUtilities.o $(LIB_DIR_UTIL)/Funciones.o UtilidadesSDL.o $(LIB_DIR_UTIL)/Pui.o TocarAnimalesPUI.o $(OBJS) $(LDFLAG) $(LIBS)
	
$(BIN_DIR)/MostrarAnimalesPUI: MostrarAnimalesPUI.o
	g++ -o $(BIN_DIR)/MostrarAnimalesPUI $(LIB_DIR_UTIL)/cvUtilities.o $(LIB_DIR_UTIL)/Funciones.o UtilidadesSDL.o $(LIB_DIR_UTIL)/Pui.o MostrarAnimalesPUI.o $(OBJS) $(LDFLAG) $(LIBS)

Renata.o: Renata.cpp 
	g++ -c  $(CFLAG) Renata.cpp

CalibradorColorOO.o: CalibradorColorOO.cpp 
	g++ -c  $(CFLAG) CalibradorColorOO.cpp

TocarAnimalesPUI.o: TocarAnimalesPUI.cpp
	g++ -c $(CFLAG) TocarAnimalesPUI.cpp
	
MostrarAnimalesPUI.o: MostrarAnimalesPUI.cpp
	g++ -c $(CFLAG) MostrarAnimalesPUI.cpp

clean:
	rm -f *.o
	rm -f $(BIN_DIR)/CalibradorColorOO
	rm -f $(BIN_DIR)/MostrarAnimalesPui
	rm -f $(BIN_DIR)/TocarAnimalesPui

allclean:
	rm -f *.o
	rm -f $(BIN_DIR)/CalibradorColorOO
	rm -f $(BIN_DIR)/MostrarAnimalesPui
	rm -f $(BIN_DIR)/TocarAnimalesPui
