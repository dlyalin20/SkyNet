all: main.o helpers.o parsing.o runs.o shell.o
	clang -o main -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 main.o helpers.o parsing.o runs.o shell.o

main.o: main.c includes.h
	clang -c main.c 

helpers.o: helpers.c includes.h 
	clang -c helpers.c 

parsing.o: parsing.c includes.h 
	clang -c parsing.c

runs.o: runs.c includes.h
	clang -c runs.c 

shell.o: shell.c includes.h
	clang -c shell.c

clean: 
	rm -rf *.o

