CC=gcc
CFLAGS=-c -g -pthread `pkg-config --cflags gconf-2.0` `pkg-config --cflags --libs gtk+-2.0`

LDFLAGS=-g -lpthread `pkg-config --libs gconf-2.0` `pkg-config --cflags --libs gtk+-2.0`


pthread-y-gconf: pthread-y-gconf.o
	$(CC) -o pthread-y-gconf $(LDFLAGS) pthread-y-gconf.o

pthread-y-gconf.o: pthread-y-gconf.c
	$(CC) -o pthread-y-gconf.o $(CFLAGS) pthread-y-gconf.c

clean:
	rm -rf *.o pthread-y-gconf
