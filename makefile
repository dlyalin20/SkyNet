all: main.o helpers.o
	clang -o main main.o helpers.o

main.o: main.c headers.h
	clang -c main.c 

helpers.o: helpers.c headers.h 
	clang -c helpers.c 

clean: 
	rm -rf *.o