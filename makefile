all: player adder includes.h

player:	player.o playerhelpers.o adderhelpers.o cJSON.o
	clang -o player  player.o -lSDL2 playerhelpers.o cJSON.o adderhelpers.o

adder: adder.o adderhelpers.o playerhelpers.o cJSON.o
	gcc -o adder -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -lSDL2 adder.o adderhelpers.o playerhelpers.o cJSON.o

player.o: player.c cJSON.h includes.h
	gcc -c player.c

playerhelpers.o: playerhelpers.c playerhelpers.h includes.h adderhelpers.h
	gcc -c playerhelpers.c

adder.o: adder.c
	gcc -c adder.c

adderhelpers.o: adderhelpers.c adderhelpers.h playerhelpers.h
	gcc -c adderhelpers.c
	
clean:
	-rm -rf *.o
	-rm -rf *.run
