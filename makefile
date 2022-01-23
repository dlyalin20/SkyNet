all: main.o helpers.o parsing.o linked_list.o
	clang -o main -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 main.o helpers.o parsing.o linked_list.o -lcJSON

main.o: main.c includes.h cJSON.h
	clang -c main.c 

helpers.o: helpers.c includes.h 
	clang -c helpers.c 

parsing.o: parsing.c parsing.h includes.h
	clang -c parsing.c

linked_list.o: linked_list.c linked_list.h includes.h
	clang -c linked_list.c

clean: 
	rm -rf *.o

