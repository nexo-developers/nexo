

CACHE=../cache


stdcpp = libstdc++-devel-4.4.0-4.i586.rpm


################## GLIBC files ################

GLIBC = lib/ld-2.10.1.so
$(GLIBC):
	$(MAKE) FILE=glibc-2.10.1-2.i586.rpm extract

GLIBC: $(GLIBC)

GLIBC_HEADERS = usr/include/stdio.h usr/include/stdlib.h
$(GLIBC_HEADERS): 
	$(MAKE) FILE=glibc-headers-2.10.1-2.i586.rpm extract

GLIBC_HEADERS: $(GLIBC_HEADERS)

GLIBC_DEVEL = usr/include/gnu/stubs-32.h
$(GLIBC_DEVEL): $(GLIBC) $(GLIBC_HEADERS)
	$(MAKE) FILE=glibc-devel-2.10.1-2.i586.rpm extract

GLIBC_DEVEL: $(GLIBC_DEV)

################## KERNEL_HEADERS files ################

KERNEL_HEADERS = usr/include/asm/ioctl.h
$(KERNEL_HEADERS):
	$(MAKE) FILE=kernel-headers-2.6.29.4-167.fc11.i586.rpm extract

KERNEL_HEADERS: $(KERNEL_HEADERS)

################# GLIB ############################

GLIB = usr/lib/libglib-1.2.so.0.0.10 usr/lib/libgmodule-1.2.so.0.0.10 usr/lib/libgthread-1.2.so.0.0.10 
$(GLIB): 
	$(MAKE) FILE=glib-1.2.10-31.fc11.i586.rpm extract

GLIB: $(GLIB)

GLIB_DEVEL = usr/include/glib-1.2/glib.h
$(GLIB_DEVEL): $(GLIB)
	$(MAKE) FILE=glib-devel-1.2.10-31.fc11.i586.rpm extract

GLIB_DEVEL: $(GLIB_DEVEL)


################# GLIB2 ############################

GLIB2 = lib/libglib-2.0.so.0.2000.1
$(GLIB2): 
	$(MAKE) FILE=glib2-2.20.1-1.fc11.i586.rpm extract

GLIB2: $(GLIB2)

GLIB2_DEVEL = usr/include/glib-2.0/glib.h 
$(GLIB2_DEVEL): $(GLIB2)
	$(MAKE) FILE=glib2-devel-2.20.1-1.fc11.i586.rpm extract
	$(MAKE) FILE=glib2-static-2.20.1-1.fc11.i586.rpm extract

GLIB2_DEVEL: $(GLIB2_DEVEL)

################## OPENCV  #############################

OPENCV = usr/lib/libcv.so.1.0.0      usr/lib/libcvaux.so.1.0.0    \
         usr/lib/libcxcore.so.1.0.0  usr/lib/libhighgui.so.1.0.0  \
         usr/lib/libml.so.1.0.0
$(OPENCV):
	$(MAKE) FILE=opencv-1.0.0-15.fc11.i586.rpm extract

OPENCV: $(OPENCV)

OPENCV_DEVEL = usr/include/opencv/cv.h
$(OPENCV_DEVEL): $(OPENCV)
	$(MAKE) FILE=opencv-devel-1.0.0-15.fc11.i586.rpm extract

OPENCV_DEVEL: $(OPENCV_DEV)

################# LIBGL & LIBGLU & FREEGLUT #####################

LIBGL = usr/lib/libGL.so.1.2
$(LIBGL):
	$(MAKE) FILE=mesa-libGL-7.5-0.14.fc11.i586.rpm extract

LIBGL: $(LIBGL)

LIBGL_DEVEL = usr/include/GL/gl.h
$(LIBGL_DEVEL): $(LIBGL)
	$(MAKE) FILE=mesa-libGL-devel-7.5-0.14.fc11.i586.rpm extract

LIBGL_DEVEL: $(LIBGL_DEVEL)

LIBGLU = usr/lib/libGLU.so.1.3.070300
$(LIBGLU):
	$(MAKE) FILE=mesa-libGLU-7.5-0.14.fc11.i586.rpm extract

LIBGLU: $(LIBGLU)

LIBGLU_DEVEL = usr/include/GL/glu.h
$(LIBGLU_DEVEL): $(LIBGLU)
	$(MAKE) FILE=mesa-libGLU-devel-7.5-0.14.fc11.i586.rpm extract

LIBGLU_DEVEL: $(LIBGLU_DEVEL)

FREEGLUT = usr/lib/libglut.so.3.8.0
$(FREEGLUT):
	$(MAKE) FILE=freeglut-2.4.0-16.fc11.i586.rpm extract

FREEGLUT: $(FREEGLUT)

FREEGLUT_DEVEL = usr/include/GL/freeglut.h
$(FREEGLUT_DEVEL): $(FREEGLUT) $(LIBGL_DEVEL) $(LIBGLU_DEVEL)
	$(MAKE) FILE=freeglut-devel-2.4.0-16.fc11.i586.rpm extract

FREEGLUT_DEVEL: $(FREEGLUT_DEVEL)

##################### LIBXML2 #########################

LIBXML2 = usr/lib/libxml2.so.2.7.3
$(LIBXML2):
	$(MAKE) FILE=libxml2-2.7.3-2.fc11.i586.rpm extract

LIBXML2: $(LIBXML2)

LIBXML2_DEVEL = usr/include/libxml2/libxml/parser.h
$(LIBXML2_DEVEL): $(LIBXML2)
	$(MAKE) FILE=libxml2-devel-2.7.3-2.fc11.i586.rpm extract
	$(MAKE) FILE=libxml2-static-2.7.3-2.fc11.i586.rpm extract

LIBXML2_DEVEL: $(LIBXML2_DEVEL)

#      libxml2-python-2.7.3-2.fc11.i586.rpm                                       12-Mar-2009 19:12  406K  RPM Package

#################### GSTREAMER010 ########################

GSTREAMER010 = usr/lib/libgstreamer-0.10.so.0.19.0
$(GSTREAMER010):
	$(MAKE) FILE=gstreamer-0.10.22-3.fc11.i586.rpm extract

GSTREAMER010: $(GSTREAMER010)

GSTREAMER010_DEVEL = usr/include/gstreamer-0.10/gst/gst.h
$(GSTREAMER010_DEVEL): $(GSTREAMER010)
	$(MAKE) FILE=gstreamer-devel-0.10.22-3.fc11.i586.rpm extract

GSTREAMER010_DEVEL: $(GSTREAMER010_DEVEL)

#################### SDL & FRIENDS ##########################

SDL = usr/lib/libSDL-1.2.so.0.11.2
$(SDL):
	$(MAKE) FILE=SDL-1.2.13-9.fc11.i586.rpm extract

SDL: $(SDL)

SDL_DEVEL = usr/include/SDL/SDL.h
$(SDL_DEVEL): $(SDL)
	$(MAKE) FILE=SDL-devel-1.2.13-9.fc11.i586.rpm extract

SDL_DEVEL: $(SDL_DEVEL)

SDLGFX = usr/lib/libSDL_gfx.so.0.0.17
$(SDLGFX): $(SDL)
	$(MAKE) FILE=SDL_gfx-2.0.17-2.fc11.i586.rpm extract

SDLGFX: $(SDLGFX)

SDLGFX_DEVEL = usr/include/SDL/SDL_gfxPrimitives.h
$(SDLGFX_DEVEL): $(SDLGFX) $(SDL_DEVEL)
	$(MAKE) FILE=SDL_gfx-devel-2.0.17-2.fc11.i586.rpm extract

SDLGFX_DEVEL: $(SDLGFX_DEVEL)


SDLMIXER = usr/lib/libSDL_mixer-1.2.so.0.2.6
$(SDLMIXER): $(SDL)
	$(MAKE) FILE=SDL_mixer-1.2.8-12.fc11.i586.rpm extract

SDLMIXER: $(SDLMIXER)

SDLMIXER_DEVEL = usr/include/SDL/SDL_mixer.h
$(SDLMIXER_DEVEL): $(SDLMIXER) $(SDL_DEVEL)
	$(MAKE) FILE=SDL_mixer-devel-1.2.8-12.fc11.i586.rpm extract

SDLMIXER_DEVEL: $(SDLMIXER_DEVEL)


###################  LIBSUGARIZE  ################################

LIBSUGARIZE = usr/lib/libsugarize.so
$(LIBSUGARIZE):
	mkdir -p usr/lib
	( cd usr/lib ; wget http://download.sugarlabs.org/services/libsugarize/libsugarize-1-1-Linux-i486.tar.bz2 )
	( cd usr/lib ; tar -xjf libsugarize-1-1-Linux-i486.tar.bz2 )
	( cd usr/lib ; rm -f libsugarize-1-1-Linux-i486.tar.bz2 README )

################## NCURSES #####################################
## TODO // MORE CLEANUP

NCURSES = usr/bin/tset usr/bin/tput usr/bin/reset usr/bin/clear
$(NCURSES):
	$(MAKE) FILE=ncurses-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-base-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-devel-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-libs-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-static-5.7-2.20090207.fc11.i586.rpm extract
	$(MAKE) FILE=ncurses-term-5.7-2.20090207.fc11.i586.rpm extract

NCURSES: $(NCURSES)

###########################  BASH  ##################################

#Y USE BASH_ to not conflict whit BASH entorn variable

BASH_ = bin/bash
$(BASH_): $(NCURSES) $(GLIBC)
	$(MAKE) FILE=bash-4.0-6.fc11.i586.rpm extract

BASH_: $(BASH_)

################# LIBSELINUX #######################
## //TODO MORE CLEANUP

LIBSELINUX = lib/libselinux.so.1
$(LIBSELINUX):
	$(MAKE) FILE=libselinux-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-devel-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-python-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-ruby-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-static-2.0.80-1.fc11.i586.rpm extract
	$(MAKE) FILE=libselinux-utils-2.0.80-1.fc11.i586.rpm extract

LIBSELINUX: $(LIBSELINUX)

################# LIBCAP ######################

LIBCAP = lib/libcap.so.2.16
$(LIBCAP):
	$(MAKE) FILE=libcap-2.16-2.fc11.i586.rpm extract

LIBCAP: $(LIBCAP)

LIBCAP_DEVEL = usr/include/sys/capability.h
$(LIBCAP_DEVEL): $(LIBCAP)
	$(MAKE) FILE=libcap-devel-2.16-2.fc11.i586.rpm extract

LIBCAP_DEVEL: $(LIBCAP_DEVEL)

################# LIBACL #####################

LIBACL = lib/libacl.so.1.1.0
$(LIBACL):
	$(MAKE) FILE=libacl-2.2.47-4.fc11.i586.rpm extract

LIBACL: $(LIBACL)

LIBACL_DEVEL = usr/include/acl/libacl.h usr/include/sys/acl.h
$(LIBACL_DEVEL): $(LIBACL)
	$(MAKE) FILE=libacl-devel-2.2.47-4.fc11.i586.rpm extract

LIBACL_DEVEL: $(LIBACL_DEVEL)

################## LIBATTR ################

LIBATTR = lib/libattr.so.1.1.0
$(LIBATTR):
	$(MAKE) FILE=libattr-2.4.43-3.fc11.i586.rpm extract

LIBATTR: $(LIBATTR)

LIBATTR_DEVEL= usr/include/attr/attributes.h usr/include/attr/libattr.h
$(LIBATTR_DEVEL): $(LIBATTR)
	$(MAKE) FILE=libattr-devel-2.4.43-3.fc11.i586.rpm extract

LIBATTR_DEVEL: $(LIBATTR_DEVEL)

###################### COREUTILS ##########################
	
COREUTILS = bin/cat bin/mv bin/cp bin/dd bin/ln bin/echo
$(COREUTILS): $(LIBSELINUX) $(LIBCAP) $(LIBACL) $(LIBATTR)
	$(MAKE) FILE=coreutils-7.2-1.fc11.i586.rpm extract

COREUTILS: $(COREUTILS)

################# FAKEROOT #################

FAKEROOT = usr/bin/fakeroot
$(FAKEROOT):
	$(MAKE) FILE=fakeroot-1.12.2-21.fc11.i586.rpm extract

FAKEROOT: $(FAKEROOT)

FAKEROOT_LIBS = usr/lib/libfakeroot/libfakeroot-sysv.so
$(FAKEROOT_LIBS):
	$(MAKE) FILE=fakeroot-libs-1.12.2-21.fc11.i586.rpm extract

FAKEROOT_LIBS: $(FAKEROOT_LIBS)

##################### MPFR #############################

MPFR = usr/lib/libmpfr.so.1.2.0
$(MPFR):
	$(MAKE) FILE=mpfr-2.4.1-1.fc11.i586.rpm extract

MPFR: $(MPFR)

MPFR_DEVEL = usr/include/mpfr.h
$(MPFR_DEVEL): $(MPFR)
	$(MAKE) FILE=mpfr-devel-2.4.1-1.fc11.i586.rpm extract

MPFR_DEVEL: $(MPFR_DEVEL)

################ GMP ###########################

GMP = usr/lib/libgmp.so.3.4.4 usr/lib/libgmpxx.so.4.0.4 usr/lib/libmp.so.3.1.13
$(GMP):
	$(MAKE) FILE=gmp-4.2.4-6.fc11.i586.rpm extract

GMP: $(GMP)

GMP_DEVEL = usr/include/gmp.h usr/lib/libgmp.a usr/lib/libgmpxx.a usr/lib/libmp.a
$(GMP_DEVEL):
	$(MAKE) FILE=gmp-devel-4.2.4-6.fc11.i586.rpm extract
	$(MAKE) FILE=gmp-static-4.2.4-6.fc11.i586.rpm extract

#      gmp-ecm-6.2.1-5.fc11.i586.rpm                                              11-Mar-2009 18:14  247K  RPM Package
#      gmp-ecm-devel-6.2.1-5.fc11.i586.rpm                                        11-Mar-2009 18:14  8.0K  RPM Package
#      gmp-ecm-static-6.2.1-5.fc11.i586.rpm                                       11-Mar-2009 18:14  144K  RPM Package
#      gmpc-0.16.1-2.fc11.i586.rpm                                                11-Mar-2009 18:14  620K  RPM Package
#      gmpc-devel-0.16.1-2.fc11.i586.rpm          


################## ZLIB ##########################

ZLIB = lib/libz.so.1.2.3
$(ZLIB):
	$(MAKE) FILE=zlib-1.2.3-22.fc11.i586.rpm extract

ZLIB: $(ZLIB)

ZLIB_DEVEL = usr/include/zlib.h 
$(ZLIB_DEVEL): $(ZLIB)
	$(MAKE) FILE=zlib-devel-1.2.3-22.fc11.i586.rpm extract
	$(MAKE) FILE=zlib-static-1.2.3-22.fc11.i586.rpm extract

ZLIB_DEVEL: $(ZLIB_DEVEL)

################## BINUTILS ######################3

BINUTILS = usr/bin/ar usr/bin/as usr/bin/ld usr/bin/strip
$(BINUTILS): $(ZLIB) $(GLIBC_DEVEL)
	$(MAKE) FILE=binutils-2.19.51.0.2-17.fc11.i586.rpm extract

BINUTILS: $(BINUTILS)

BINUTILS_DEVEL = usr/include/symcat.h usr/include/dis-asm.h
$(BINUTILS_DEVEL): $(BINUTILS)
	$(MAKE) FILE=binutils-devel-2.19.51.0.2-17.fc11.i586.rpm extract

BINUTILS_DEVEL: $(BINUTILS_DEVEL)

################### LIBGCC #########################

LIBGCC = lib/libgcc_s-4.4.0-20090506.so.1
$(LIBGCC):
	$(MAKE) FILE=libgcc-4.4.0-4.i586.rpm extract

LIBGCC: $(LIBGCC)

################### CPP GCC GCC-C++ ##########################

CPP = usr/libexec/gcc/i586-redhat-linux/4.4.0/cc1
$(CPP): $(MPFR) $(GMP)
	$(MAKE) FILE=cpp-4.4.0-4.i586.rpm extract

CPP: $(CPP)

GCC = usr/bin/gcc
$(GCC): $(LIBGCC) $(CPP) $(BINUTILS)
	$(MAKE) FILE=gcc-4.4.0-4.i586.rpm extract

GCC: $(GCC)

GCC_CPP = usr/bin/g++ usr/bin/c++ usr/libexec/gcc/i586-redhat-linux/4.4.0/cc1plus
$(GCC_CPP):
	$(MAKE) FILE=gcc-c++-4.4.0-4.i586.rpm extract

GCC_CPP: $(GCC_CPP)



#    gcc-objc++-4.4.0-4.i586.rpm                                                07-May-2009 15:34  3.9M  RPM Package
#      gcc-objc-4.4.0-4.i586.rpm     

######################### MAKE ###################3

MAKE_ = usr/bin/make
$(MAKE_):
	$(MAKE) FILE=make-3.81-15.fc11.i586.rpm extract

MAKE_: $(MAKE_)



####################### EDITORS #####################3

NANO = usr/bin/nano
$(NANO):
	$(MAKE) FILE=nano-2.0.6-6.fc11.i586.rpm extract

NANO: $(NANO)

#//TODO VI

#############################3








%.rpm:
	$(MAKE) -C $(CACHE) $@


extract:
	(if [ "$(FILE)" = "" ] ; then exit -1 ; fi)
	$(MAKE) -C $(CACHE) $(FILE)
	( rpm2cpio $(CACHE)/$(FILE) | cpio -idv )


all:
	@echo extract FILE bla bla



clean:
	rm -rf usr bin sbin lib share etc var proc


#to execute even if exist the file
.PHONY: all clean cleanrpm %.fc11.i586.rpm BASH_
