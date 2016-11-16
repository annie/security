CC = gcc
INCLUDES = 
CFLAGS = -g -Wall $(INCLUDES)
LDFLAGS = -g

build:	encdec

encdec:	encdec.o hmacsha1.o

encdec.o: encdec.c encdec.h

hmacsha1.o: hmacsha1.c hmacsha1.h

.PHONY: clean
clean:
	rm -f *.o a.out core encdec
