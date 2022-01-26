#include "connections.h"
#include "includes.h"

int main(int argc, char const *argv[]) {
  int sd = client_handshake();
  char buffer[BUFFER_SIZE];
  while (1){
    printf("What is your request: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    // testing
    write(sd, buffer, BUFFER_SIZE);
    read(sd, buffer, BUFFER_SIZE);
    printf("SERVER RESPONSE: %s\n", buffer);


  }
  return 0;
}
