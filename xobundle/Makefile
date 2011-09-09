FC11 = fc11
FC11_RPM = opencv-1.0.0-15.fc11.i586.rpm
FC11_REPO = archives.fedoraproject.org/pub/archive/fedora/linux/releases/11/Everything/i386/os/Packages


$(FC11_REPO)/%.rpm:
	# ok, lets check if $(FC11) direxctory exist...
	(if [ ! -e $(FC11) ] ; then mkdir $(FC11) ; fi)
	# this is fa**ing freaking!, but works :-)
	(cd $(FC11) && wget $@)


$(FC11)/%.rpm: $(FC11_REPO)/%.rpm
	#rpm2cpio $@ | cpio -idmv

%.fc11.i586.rpm: $(FC11)/%.fc11.i586.rpm
	echo pepe 32321


pepe: 
	echo pepe

all:
	echo yupi


cleanrpm:
	#rm -rf $(FC11)/*

clean:
	#rm nothing



fc11_bundle: 0xFFFF-0.3.9-3.fc11.i586.rpm
	

ceibal_bundle: fc11_bundle
	# ceibal use fc11




#to execute even if exist the file
.PHONY: all clean cleanrpm %.fc11.i586.rpm


.SECONDARY: $(FC11)/%.fc11.i586.rpm
	




#fc11_bundle ceibal_bundle
	


.PRECIOUS: $(FC11)/%.rpm
