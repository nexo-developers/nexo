

CACHE=../cache


stdcpp = libstdc++-devel-4.4.0-4.i586.rpm


################## GLIBC files ################

GLIBC = usr/include/stdio.h usr/include/stdlib.h
$(GLIBC): 
	$(MAKE) FILE=glibc-headers-2.10.1-2.i586.rpm extract


GLIBC_DEV = usr/include/gnu/stubs-32.h
$(GLIBC_DEV):
	$(MAKE) FILE=glibc-devel-2.10.1-2.i586.rpm extract


################## KERNEL_HEADERS files ################

KERNEL_HEADERS = usr/include/asm/ioctl.h
$(KERNEL_HEADERS):
	$(MAKE) FILE=kernel-headers-2.6.29.4-167.fc11.i586.rpm extract


################## OPENCV  #############################

OPENCV = usr/lib/libcv.so.1.0.0
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


##############################################



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




