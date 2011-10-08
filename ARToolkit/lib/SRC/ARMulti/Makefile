#
# For instalation. Change this to your settings.
#
INC_DIR = ../../../include
LIB_DIR = ../..
#
#  compiler
#
CC= cc
CFLAG= $(CFLAG_CROSS) -O -fpic -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -g -I$(INC_DIR)
#
# For making the library
#
AR= ar
ARFLAGS= rs
RANLIB= 
#
#   products
#
LIB= ${LIB_DIR}/libARMulti.a
INCLUDE= ${INC_DIR}/AR/config.h \
         ${INC_DIR}/AR/matrix.h \
         ${INC_DIR}/AR/param.h \
         ${INC_DIR}/AR/ar.h \
         ${INC_DIR}/AR/arMulti.h
#
#   compilation control
#
LIBOBJS= ${LIB}(arMultiReadConfigFile.o) \
         ${LIB}(arMultiGetTransMat.o) \
         ${LIB}(arMultiActivate.o)


all:		${LIBOBJS}

${LIBOBJS}:	${INCLUDE}

.c.a:
	${CC} -c ${CFLAG} $<
	${AR} ${ARFLAGS} $@ $*.o
	${RANLIB}


%.d : %.c
	${CC} $(CFLAG) -M $< > $@


clean:
	rm -f *.o
	rm -f *.d
	rm -f ${LIB}

#allclean:
#	rm -f *.o
#	rm -f ${LIB}
#	rm -f Makefile


# PHONY TARGETS
.PHONY: all clean allclean

