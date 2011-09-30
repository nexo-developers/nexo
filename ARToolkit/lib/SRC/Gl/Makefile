#
# For instalation. Change this to your settings.
#
INC_DIR = ../../../include
LIB_DIR = ../..
#
#  compiler
#
CC= cc
CFLAG= $(CFLAG_CROSS) -fpic -O -pthread -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/libxml2 -I/usr/X11R6/include -g -I$(INC_DIR)
#
# For making the library
#
AR= ar
ARFLAGS= rs
RANLIB= 
#
#   products
#
LIB= ${LIB_DIR}/libARgsub.a
LIB2= ${LIB_DIR}/libARgsubUtil.a
LIB3= ${LIB_DIR}/libARgsub_lite.a

INCLUDE= ${INC_DIR}/AR/gsub.h \
         ${INC_DIR}/AR/config.h \
         ${INC_DIR}/AR/ar.h \
         ${INC_DIR}/AR/matrix.h \
         ${INC_DIR}/AR/param.h
INCLUDE2= ${INC_DIR}/AR/gsubUtil.h
INCLUDE3= ${INC_DIR}/AR/gsub_lite.h \
         ${INC_DIR}/AR/config.h \
         ${INC_DIR}/AR/ar.h \
         ${INC_DIR}/AR/param.h

#
#   compilation control
#
LIBOBJS= ${LIB}(gsub.o)
LIB2OBJS= ${LIB2}(gsubUtil.o)
LIB3OBJS= ${LIB3}(gsub_lite.o)

all:	${LIBOBJS} ${LIB2OBJS} ${LIB3OBJS}

${LIBOBJS}:		${INCLUDE}
${LIB2OBJS}:		${INCLUDE} ${INCLUDE2}
${LIB3OBJS}:		${INCLUDE3}

.c.a:
	${CC} -c ${CFLAG} $<
	${AR} ${ARFLAGS} $@ $*.o
	${RANLIB}


%.d : %.c
	${CC} $(CFLAG) -M $< > $@


clean:
	rm -f *.o
	rm -f *.d
	rm -f ${LIB} ${LIB2} ${LIB3}

#allclean:
#	rm -f *.o
#	rm -f ${LIB} ${LIB2} ${LIB3}
#	rm -f Makefile
