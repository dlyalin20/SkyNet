all: main.o helpers.o parsing.o
	clang -o main -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 main.o helpers.o parsing.o

main.o: main.c includes.h
	clang -c main.c 

helpers.o: helpers.c includes.h 
	clang -c helpers.c 

parsing.o: parsing.c parsing.h includes.h 
	clang -c parsing.c

clean: 
	rm -rf *.o

