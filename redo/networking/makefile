all: client server
	
client: forking_client.o pipe_networking.o
	gcc -o client forking_client.o pipe_networking.o

server: forking_server.o pipe_networking.o setup.o
	gcc -o server forking_server.o pipe_networking.o setup.o

forking_client.o: forking_client.c pipe_networking.h
	gcc -c forking_client.c

forking_server.o: forking_server.c pipe_networking.h setup.h
	gcc -c forking_server.c setup.h

pipe_networking.o: pipe_networking.c pipe_networking.h setup.h
	gcc -c pipe_networking.c setup.h

setup.o: setup.c
	gcc -c setup.c

clean:
	-rm *.o
	-rm *~
	-rm client
	-rm server
	-rm mario
