VIDEO_DRIVER=VideoGStreamer

all:
	(cd AR;         make -f Makefile)
	(cd ARMulti;    make -f Makefile)
	(cd Gl;         make -f Makefile)
	(cd VideoGStreamer; make -f Makefile)

clean:
	(cd AR;         make -f Makefile clean)
	(cd ARMulti;    make -f Makefile clean)
	(cd Gl;         make -f Makefile clean)
	(cd VideoGStreamer;    make -f Makefile clean)

allclean:
	(cd AR;         make -f Makefile allclean)
	(cd ARMulti;    make -f Makefile allclean)
	(cd Gl;         make -f Makefile allclean)
	(cd VideoGStreamer;    make -f Makefile allclean)
	rm -f Makefile
