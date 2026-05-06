CC = gcc
CFLAGS = -Wall -Wextra -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

all: maze


maze:
	$(CC) maze.c -o maze $(CFLAGS) $(LDFLAGS)

clean:
	rm -f maze