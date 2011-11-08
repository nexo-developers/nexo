CC=gcc
CFLAGS=-c -g -pthread

LDFLAGS=-g -lpthread


hello-gconf: hello-pthread.o
	$(CC) -o hello-pthread $(LDFLAGS) hello-pthread.o

hello-gconf.o: hello-pthread.c
	$(CC) -o hello-pthread.o $(CFLAGS) hello-pthread.c

clean:
	rm -rf *.o hello-pthread
