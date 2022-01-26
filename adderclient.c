#include "connections.h"
#include "includes.h"

int add(int sd){
  char buffer[BUFFER_SIZE];
  write(sd, "add", BUFFER_SIZE);
  read(sd, buffer, BUFFER_SIZE);
  printf("Recieved from server %s\n", buffer);
  if (strcmp(buffer, "SRecievedADD")){
    printf("%s\n", "ServerRequestNameResponseDidntMatch");
    return 1;
  }
  printf("What the the file path to the song u want to add: \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  buffer[strlen(buffer)-1] = '\0';// remove the newline
  // send song size
  struct stat st;
  stat(buffer, &st);
  ssize_t songsize = st.st_size;
  write(sd, &songsize, sizeof(int));
  ssize_t testsize;
  read(sd, &testsize, sizeof(int));
  if (testsize != songsize){
    return 1;
  }
  // send the song
  printf("%s\n", "Song Exists, testing get_file_info");
  open(buffer, O_RDONLY);

}

int main(int argc, char const *argv[]) {
  int sd = client_handshake();
  char buffer[BUFFER_SIZE];
  while (1){
    printf("What is your request: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    buffer[strlen(buffer)-1] = '\0';// remove the newline
    printf("Request: %s\n", buffer);
    if (!strcmp(buffer, "add")){
        printf("%s\n", "STARTING ADD SONG TO QUEUE REQUEST");
        if (!add(sd)){
          printf("%s\n", "successful add request");
        }else{
          printf("ADD request failed\n");
        }
      }


      // ask clock if enough time has passed
      // ask for queue
    }

  }
  //   // testing initial connection
  //   write(sd, buffer, BUFFER_SIZE);
  //   read(sd, buffer, BUFFER_SIZE);
  //   printf("SERVER RESPONSE: %s\n", buffer);
