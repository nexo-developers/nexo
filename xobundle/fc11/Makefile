

CACHE=../cache


stdcpp = libstdc++-devel-4.4.0-4.i586.rpm


################## GLIBC files ################

GLIBC = lib/ld-2.10.1.so
$(GLIBC):
	$(MAKE) FILE=glibc-2.10.1-2.i586.rpm extract


GLIBCH = usr/include/stdio.h usr/include/stdlib.h
$(GLIBCH): 
	$(MAKE) FILE=glibc-headers-2.10.1-2.i586.rpm extract


GLIBC_DEV = usr/include/gnu/stubs-32.h
$(GLIBC_DEV):
	$(MAKE) FILE=glibc-devel-2.10.1-2.i586.rpm extract


################## KERNEL_HEADERS files ################

KERNEL_HEADERS = usr/include/asm/ioctl.h
$(KERNEL_HEADERS):
	$(MAKE) FILE=kernel-headers-2.6.29.4-167.fc11.i586.rpm extract


################## OPENCV  #############################

OPENCV = usr/lib/libcv.so.1.0.0      usr/lib/libcvaux.so.1.0.0    \
         usr/lib/libcxcore.so.1.0.0  usr/lib/libhighgui.so.1.0.0  \
         usr/lib/libml.so.1.0.0
$(OPENCV):
	$(MAKE) FILE=opencv-1.0.0-15.fc11.i586.rpm extract


OPENCV_DEVEL = usr/include/opencv/cv.h
$(OPENCV_DEVEL):
	$(MAKE) FILE=opencv-devel-1.0.0-15.fc11.i586.rpm extract

################# LIBGL & LIBGLU #########################

LIBGL = usr/lib/libGL.so.1.2
$(LIBGL):
	$(MAKE) FILE=mesa-libGL-7.5-0.14.fc11.i586.rpm extract

LIBGL_DEVEL = usr/include/GL/gl.h
$(LIBGL_DEVEL):
	$(MAKE) FILE=mesa-libGL-devel-7.5-0.14.fc11.i586.rpm extract

LIBGLU = usr/lib/libGLU.so.1.3.070300
$(LIBGLU):
	$(MAKE) FILE=mesa-libGLU-7.5-0.14.fc11.i586.rpm extract

LIBGLU_DEVEL = usr/include/GL/glu.h
$(LIBGLU_DEVEL):
	$(MAKE) FILE=mesa-libGLU-devel-7.5-0.14.fc11.i586.rpm extract


#################### SDL_GFX  ##########################

SDLGFX = usr/lib/libSDL_gfx.so.0.0.17
$(SDLGFX):
	$(MAKE) FILE=SDL_gfx-2.0.17-2.fc11.i586.rpm extract

SDLGFX_DEV = usr/include/SDL/SDL_gfxPrimitives.h
$(SDLGFX_DEV):
	$(MAKE) FILE=SDL_gfx-devel-2.0.17-2.fc11.i586.rpm extract

###################  LIBSUGARIZE  ################################

LIBSUGARIZE = usr/lib/libsugarize.so
$(LIBSUGARIZE):
	mkdir -p usr/lib
	( cd usr/lib ; wget http://download.sugarlabs.org/services/libsugarize/libsugarize-1-1-Linux-i486.tar.bz2 )
	( cd usr/lib ; tar -xjf libsugarize-1-1-Linux-i486.tar.bz2 )
	( cd usr/lib ; rm -f libsugarize-1-1-Linux-i486.tar.bz2 README )

###########################  BASH  ##################################

#NEED NCURSES y GLIBC (ld-linux.so) //TODO

RPMBASH = bin/bash
$(RPMBASH):
	$(MAKE) FILE=bash-4.0-6.fc11.i586.rpm extract

	
###################### COREUTILS ##########################

#NEED LIBSELINUX LIBCAP

COREUTILS = bin/cat bin/mv bin/cp bin/dd bin/ln /bin/echo
$(COREUTILS):
	$(MAKE) FILE=coreutils-7.2-1.fc11.i586.rpm extract

################## NCURSES ##################

NCURSES:
	$(MAKE) FILE=ncurses-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-base-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-devel-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-libs-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-static-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-term-5.7-2.20090207.fc11.i586.rpm extract

################# LIBSELINUX #######################

LIBSELINUX:
	$(MAKE) FILE=libselinux-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-devel-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-python-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-ruby-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-static-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-utils-2.0.80-1.fc11.i586.rpm extract

################# LIBCAP ######################

LIBCAP:
	$(MAKE) FILE=libcap-2.16-2.fc11.i586.rpm extract
	$(MAKE) FILE=libcap-devel-2.16-2.fc11.i586.rpm extract

################# LIBACL #####################

LIBACL:
	$(MAKE) FILE=libacl-2.2.47-4.fc11.i586.rpm extract
	$(MAKE) FILE=libacl-devel-2.2.47-4.fc11.i586.rpm extract

################## LIBATTR ################

LIBATTR:
	$(MAKE) FILE=libattr-2.4.43-3.fc11.i586.rpm extract
	$(MAKE) FILE=libattr-devel-2.4.43-3.fc11.i586.rpm extract

##################################

%.rpm:
	$(MAKE) -C $(CACHE) $@


extract:
	(if [ "$(FILE)" = "" ] ; then exit -1 ; fi)
	$(MAKE) -C $(CACHE) $(FILE)
	( rpm2cpio $(CACHE)/$(FILE) | cpio -idmv )





$(FC11)/%.rpm: $(FC11_REPO)/%.rpm
	rpm2cpio $@ | (cd $(FC11) && cpio -idmv)

%.i586.rpm: $(FC11)/%.i586.rpm
	


all:	$(GLIBC) $(GLIBC_DEV) $(KERNEL_HEADERS) $(OPENCV) $(OPENCV_DEVEL)
	@echo extract FILE bla bla
	




clean:
	rm -rf usr bin lib share


fc11_bundle: $(FC11_RPM)
	



ceibal_bundle: fc11_bundle
	# ceibal use fc11


#to execute even if exist the file
.PHONY: all clean cleanrpm %.fc11.i586.rpm




