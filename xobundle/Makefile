
FC11=fc11


CFLAG= -I`pwd`/fc11/usr/include

LDFLAG= -L`pwd`/fc11
# -L`pwd`/fc11/usr/lib


all:
	$(MAKE) DIR="../ARToolkit/lib/SRC" OBJ=all cross
	$(MAKE) DIR="../Biblioteca\ PUI" OBJ=all cross
	$(MAKE) DIR="../Biblioteca\ PUI/samples/c++" OBJ=all cross

cross:
	$(MAKE) -C $(FC11)
	$(MAKE) CFLAG_CROSS="$(CFLAG)" LDFLAG_CROSS="$(LDFLAG)" -C $(DIR) $(OBJ)



clean:
	$(MAKE) -C ../Biblioteca\ PUI               clean
	$(MAKE) -C ../ARToolkit/lib/SRC             clean
	$(MAKE) -C ../Biblioteca\ PUI/samples/c++   clean



fc11_bundle: $(FC11_RPM)
	



ceibal_bundle: fc11_bundle
	# ceibal use fc11


#to execute even if exist the file
.PHONY: all clean cross





#.PRECIOUS: $(FC11)/%.rpm
