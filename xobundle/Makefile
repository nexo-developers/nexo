FC11 = fc11
FC11_RPM = opencv-1.0.0-15.fc11.i586.rpm
FC11_REPO = archives.fedoraproject.org/pub/archive/fedora/linux/releases/11/Everything/i386/os/Packages


$(FC11_REPO)/%.rpm:
	# ok, lets check if $(FC11) direxctory exist...
	(if [ ! -e $(FC11) ] ; then mkdir $(FC11) ; fi)
	# this is fa**ing freaking!, but works :-)
	(cd $(FC11) && wget $@)


$(FC11)/%.rpm: $(FC11_REPO)/%.rpm
	

%.rpm: $(FC11)/%.rpm
	



all:
	echo yupi


clean:
	rm -rf $(FC11)/*


fc11_bundle: opencv-1.0.0-15.fc11.i586.rpm
	

ceibal_bundle: fc11_bundle
	





.PHONY: all clean fc11_bundle ceibal_bundle
	


