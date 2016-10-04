CC = gcc
INCLUDES = 
CFLAGS = -g -Wall $(INCLUDES)
LDFLAGS = -g

all: addqueue showqueue rmqueue

addqueue: addqueue.o

addqueue.o: addqueue.c addqueue.h

showqueue: showqueue.o

showqueue.o: showqueue.c showqueue.h

rmqueue: rmqueue.o

rmqueue.o: rmqueue.o rmqueue.h

.PHONY: clean
clean:
	rm -f *.o a.out core addqueue showqueue rmqueue
	rm -rf queue
