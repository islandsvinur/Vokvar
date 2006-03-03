
CC=gcc
CFLAGS=-Wall -I. -I/usr/include/GL

LDIR =../lib

LIBS=-lm -lrfftw -lglut

DEPS = visualization.h simulation.h main.h
OBJ = visualization.o simulation.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fluids: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o *~ core

distclean: clean
	rm fluids
