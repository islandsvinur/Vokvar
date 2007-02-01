
CC = gcc
CFLAGS = -Wall -I/usr/include/GL

LIBS = -lm -lfftw -lrfftw -lglut

DEPS = global.h vector.h visualization.h simulation.h main.h
OBJS = vector.o visualization.o simulation.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

fluids: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o

distclean: clean
	rm fluids
