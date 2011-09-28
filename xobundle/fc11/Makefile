



fc11_stdcpp = libstdc++-devel-4.4.0-4.i586.rpm
fc11_glibc = glibc-headers-2.10.1-2.i586.rpm glibc-devel-2.10.1-2.i586.rpm
fc11_kernel_headers = kernel-headers-2.6.29.4-167.fc11.i586.rpm
fc11_opencv = opencv-1.0.0-15.fc11.i586.rpm

FC11_RPM = $(fc11_glibc) $(fc11_kernel_headers) $(fc11_stdcpp)


$(FC11_REPO)/%.rpm:
	# ok, lets check if $(FC11) direxctory exist...
	(if [ ! -e $(FC11) ] ; then mkdir $(FC11) ; fi)
	# this is fa**ing freaking!, but works :-)
	(cd $(FC11) && wget $@)


$(FC11)/%.rpm: $(FC11_REPO)/%.rpm
	rpm2cpio $@ | (cd $(FC11) && cpio -idmv)

%.i586.rpm: $(FC11)/%.i586.rpm
	





all: ceibal_bundle
	$(MAKE) -C ../Biblioteca\ PUI       all
	$(MAKE) -C ..//ARToolkit/lib/SRC    all


cleanrpm:
	rm -rf $(FC11)/*

clean:
	$(MAKE) -C ../Biblioteca\ PUI      clean
	$(MAKE) -C ../ARToolkit/lib/SRC    clean



fc11_bundle: $(FC11_RPM)
	



ceibal_bundle: fc11_bundle
	# ceibal use fc11


#to execute even if exist the file
.PHONY: all clean cleanrpm %.fc11.i586.rpm




.PRECIOUS: $(FC11)/%.rpm
