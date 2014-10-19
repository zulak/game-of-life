CC=gcc
FLAGS=-Wall -Wextra -Wshadow -std=c99 -lm
SDL=-I/usr/local/include/SDL -D_GNU_SOURCE=1 -D_THREAD_SAFE -L/usr/local/lib -lSDLmain -lSDL -Wl,-framework,Cocoa

game_of_life: game_of_life.c
	${CC} game_of_life.c ${FLAGS} ${SDL} -o game_of_life.out

clean:
	rm game_of_life
