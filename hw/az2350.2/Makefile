CC = gcc
INCLUDES = 
CFLAGS = -g -Wall $(INCLUDES)
LDFLAGS = -g

build:	encdec

encdec:	encdec.o

encdec.o:	encdec.c encdec.h hmacsha1.h

.PHONY: clean
clean:
	rm -f *.o a.out core encdec
