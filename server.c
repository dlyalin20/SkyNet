#include "connections.h"
#include "serverhelpers.h"
#include "includes.h"

int main(int argc, char const *argv[]) {
  int sd = server_setup();
  listen(sd,10);
  while (1){
    int client_socket = server_connect(sd);
    if (client_socket){
      //handshake handled, read request
      //testing server setup----
      // printStream(client_socket);
      //----
      // get request
      char input[BUFFER_SIZE];
      read(client_socket, input, BUFFER_SIZE);

      // add this song to the queue
      if (!strcmp(argv[1], "add")){
        add_song_to_queue(client_socket);
      }


      //request handled: exit
      printf("Request Handled\n");
      exit(0);
    }
  }
}
