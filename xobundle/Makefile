
FC11=fc11
CACHE=cache
THISDIR=xobundle

CFLAG= -I`pwd`/fc11/usr/include

LDFLAG= -L`pwd`/fc11
# -L`pwd`/fc11/usr/lib

all: help


help:
	@echo " Objetivos utiles"
	@echo "=================="
	@echo
	@echo "make chroot: Genera un chroot de fc11 que permite compilar"
	@echo "    el Artoolkit, la lib pui y los ejemplos               "
	@echo
	@echo "make all_cross: trata de compilar Artoolkit y PUI contra  "
	@echo "    los .h y .so de fc11 sin utilizar chroot              "
	@echo
	@echo "make DIR=DIRECTORIO cross: compila el makefile del direc- "
	@echo "    torio DIRECTORIO pasandole las variables CFLAG_CROSS  "
	@echo "    y la variable LDFLAG_CROSS"
	@echo



all_cross:
	$(MAKE) -C $(FC11) cross
	$(MAKE) DIR="../ARToolkit/lib/SRC" OBJ=all cross
	$(MAKE) DIR="../Biblioteca\ PUI" OBJ=all cross
	$(MAKE) DIR="../Biblioteca\ PUI/samples/c++" OBJ=all cross

cross:
	$(MAKE) -C $(FC11) cross
	$(MAKE) CFLAG_CROSS="$(CFLAG)" LDFLAG_CROSS="$(LDFLAG)" -C $(DIR) $(OBJ)

chroot: symlinks
	$(MAKE) -C $(FC11) BASH_
	$(MAKE) -C $(FC11) COREUTILS
	$(MAKE) -C $(FC11) NANO
	# BASIC CHROOT
	$(MAKE) -C $(FC11) KERNEL_HEADERS
	$(MAKE) -C $(FC11) GCC
	$(MAKE) -C $(FC11) MAKE_
	$(MAKE) -C $(FC11) FREEGLUT_DEVEL
	$(MAKE) -C $(FC11) GLIB2_DEVEL
	$(MAKE) -C $(FC11) GSTREAMER010_DEVEL
	$(MAKE) -C $(FC11) LIBXML2_DEVEL
	# NEEDED BY ARTOOLKIT
	# //TODO compile PUI
	(cd .. ; sudo chroot .)



clean_rpm:
	$(MAKE) -C $(CACHE) clean

clean_rootfs:
	$(MAKE) -C $(FC11) clean

clean:
	$(MAKE) -C ../Biblioteca\ PUI               clean
	$(MAKE) -C ../ARToolkit/lib/SRC             clean
	$(MAKE) -C ../Biblioteca\ PUI/samples/c++   clean

symlinks:
	ln -fs $(THISDIR)/$(FC11)/usr ../usr
	ln -fs $(THISDIR)/$(FC11)/bin ../bin
	ln -fs $(THISDIR)/$(FC11)/sbin ../sbin
	ln -fs $(THISDIR)/$(FC11)/lib ../lib
	ln -fs $(THISDIR)/$(FC11)/share ../share
	ln -fs $(THISDIR)/$(FC11)/etc ../etc
	ln -fs $(THISDIR)/$(FC11)/var ../var


#to execute even if exist the file
.PHONY: all clean cross all-cross symlinks help

