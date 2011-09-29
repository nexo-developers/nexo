
FC11=fc11


INC= -I`pwd`/fc11/usr/include

CFLAG= $(INC)


all: ceibal_bundle
	$(MAKE) -C  ../Biblioteca\ PUI       all
	$(MAKE) CFLAG_CROSS="$(CFLAG)" -C ../ARToolkit/lib/SRC    all


cross:
	$(MAKE) -C $(FC11)
	$(MAKE) CFLAG_CROSS="$(CFLAG)" -C $(DIR) $(OBJ)



cleanrpm:
	rm -rf $(FC11)/*

clean:
	$(MAKE) -C ../Biblioteca\ PUI       clean
	$(MAKE) -C ..//ARToolkit/lib/SRC    clean



fc11_bundle: $(FC11_RPM)
	



ceibal_bundle: fc11_bundle
	# ceibal use fc11


#to execute even if exist the file
.PHONY: all clean cross





#.PRECIOUS: $(FC11)/%.rpm
