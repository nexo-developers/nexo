VIDEO_DRIVER=VideoGStreamer

all:
	$(MAKE) CFLAG_CROSS=$(CFLAG_CROSS) LDFLAG_CROSS=$(LDFLAG_CROSS) -C AR               all
	$(MAKE) CFLAG_CROSS=$(CFLAG_CROSS) LDFLAG_CROSS=$(LDFLAG_CROSS) -C ARMulti          all
	$(MAKE) CFLAG_CROSS=$(CFLAG_CROSS) LDFLAG_CROSS=$(LDFLAG_CROSS) -C Gl                all
	$(MAKE) CFLAG_CROSS=$(CFLAG_CROSS) LDFLAG_CROSS=$(LDFLAG_CROSS) -C VideoGStreamer   all

clean:
	(cd AR;         make -f Makefile clean)
	(cd ARMulti;    make -f Makefile clean)
	(cd Gl;         make -f Makefile clean)
	(cd VideoGStreamer;    make -f Makefile clean)

#allclean:
#	(cd AR;         make -f Makefile allclean)
#	(cd ARMulti;    make -f Makefile allclean)
#	(cd Gl;         make -f Makefile allclean)
#	(cd VideoGStreamer;    make -f Makefile allclean)
#	rm -f Makefile
