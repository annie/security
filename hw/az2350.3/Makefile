CC = gcc
INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
LDFLAGS = -g

build:	hwcopy

encdec:	hwcopy.o

encdec.o:	hwcopy.c hwcopy.h

.PHONY: clean
clean:
	rm -f *.o a.out core hwcopy
