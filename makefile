CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

all: jeu

jeu: main.o function.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

main.o: main.c struct.h
	$(CC) $(CFLAGS) -c main.c

function.o: function.c struct.h
	$(CC) $(CFLAGS) -c function.c

clean:
	rm -f *.o jeu
