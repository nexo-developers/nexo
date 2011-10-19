CC=gcc
CFLAGS=-c -g `pkg-config --cflags gconf-2.0` `pkg-config --cflags --libs gtk+-2.0`

LDFLAGS=-g `pkg-config --libs gconf-2.0` `pkg-config --cflags --libs gtk+-2.0`


hello-gconf: hello-gconf.o
	$(CC) -o hello-gconf $(LDFLAGS) hello-gconf.o

hello-gconf.o: hello-gconf.c
	$(CC) -o hello-gconf.o $(CFLAGS) hello-gconf.c

clean:
	rm -rf *.o hello-gconf
