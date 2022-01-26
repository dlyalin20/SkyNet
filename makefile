all: player adder server adderclient includes.h

player:	player.o playerhelpers.o adderhelpers.o cJSON.o
	clang -o player  player.o -lSDL2 playerhelpers.o cJSON.o adderhelpers.o

adder: adder.o adderhelpers.o playerhelpers.o cJSON.o
	gcc -o adder -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -lSDL2 adder.o adderhelpers.o playerhelpers.o cJSON.o

player.o: player.c cJSON.h includes.h
	gcc -c player.c

server: server.o serverhelpers.o connections.o
	gcc -o server.run server.o serverhelpers.o connections.o

adderclient: adderclient.o connections.o
	gcc -o adderclient.run adderclient.o connections.o

playerhelpers.o: playerhelpers.c playerhelpers.h includes.h adderhelpers.h
	gcc -c playerhelpers.c

adder.o: adder.c
	gcc -c adder.c

adderhelpers.o: adderhelpers.c adderhelpers.h playerhelpers.h
	gcc -c adderhelpers.c

server.o: server.c serverhelpers.h connections.h
	gcc -c server.c

adderclient.o: adderclient.c connections.h
	gcc -c adderclient.c

serverhelpers.o: serverhelpers.c serverhelpers.h
	gcc -c serverhelpers.c

connections.o: connections.c connections.h
	gcc -c connections.c
clean:
	-rm -rf *.o
	-rm -rf *.run
