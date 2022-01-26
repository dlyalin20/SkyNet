#include "serverhelpers.h"
#include "includes.h"
// Holds function that server calls
void printStream(int client_socket){
  char input[BUFFER_SIZE];
  while (read(client_socket, input, BUFFER_SIZE)){
    printf("%s\n", input);
    write(client_socket,input, BUFFER_SIZE);
  }
}

// add a song to the queue
void add_song_to_queue(int client_socket){
  char buffer[BUFFER_SIZE];
  strcpy(buffer, "SRecievedADD");
  write(client_socket, buffer, BUFFER_SIZE);
  ssize_t songsize;
  read(client_socket, &songsize, sizeof(int));
  write(client_socket, &songsize, sizeof(int));

}
