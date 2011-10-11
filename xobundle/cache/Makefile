

FC11_REPO = archives.fedoraproject.org/pub/archive/fedora/linux/releases/11/Everything/i386/os/Packages




$(FC11_REPO)/%.rpm:
	rm -f temp.rpm
	wget $@ -O temp.rpm


%.i586.rpm: $(FC11_REPO)/%.i586.rpm
	mv temp.rpm $@



#to execute even if exist the file
.PHONY: all clean clean

all:
	@echo Llamar a este makefile con el RPM que se nesecita
	@echo Ejemplo: make libstdc++-devel-4.4.0-4.i586.rpm

clean:
	rm -rf *.rpm


.PRECIOUS: %.rpm
