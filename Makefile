
CC=gcc
CFLAGS=-I.

LDIR =../lib

LIBS=-lm

DEPS = simulation.h
OBJ = simulation.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fluids: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core

distclean: clean
	rm fluids
