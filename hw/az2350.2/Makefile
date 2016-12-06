CC = gcc
INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
LDFLAGS = -g -lcrypto

build:	encdec

encdec:	encdec.o

encdec.o:	encdec.c encdec.h

.PHONY: clean
clean:
	rm -f *.o a.out core encdec
