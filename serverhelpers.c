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
