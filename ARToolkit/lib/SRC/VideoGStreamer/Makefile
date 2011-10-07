#
# For instalation. Change this to your settings.
#
INC_DIR = ../../../include
LIB_DIR = ../..
#
#  compiler
#
CC=cc
CFLAG= $(CFLAG_CROSS) -fpic -O -pthread -I/usr/include/gstreamer-0.10 `pkg-config --cflags glib-2.0` -I/usr/include/libxml2 -I/usr/X11R6/include -g -I$(INC_DIR)
#
# For making the library
#
AR= ar
ARFLAGS= rs
#
#   products
#
LIB= ${LIB_DIR}/libARvideo.a
INCLUDE= ${INC_DIR}/AR/video.h
#
#   compilation control
#
LIBOBJS= ${LIB}(video.o)

all:		${LIBOBJS}

${LIBOBJS}:	${INCLUDE}

.c.a:
	${CC} -c ${CFLAG} $<
	${AR} ${ARFLAGS} $@ $*.o


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

