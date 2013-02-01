CC=gcc
CFLAGS=-I. -lX11 -lGL -lGLU -lglut -lm
DEPS = lamegine.h vectors.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: lamegine.o vectors.o
	$(CC) ${CFLAGS} -o lamegine lamegine.o vectors.o

clean:
	rm lamegine.o
	rm vectors.o
	rm lamegine
