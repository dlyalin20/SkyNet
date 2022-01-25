#include "pipe_networking.h"
#include "datatypes.h"

int main(){
  int max_chars = 200;
  int sd = client_handshake();
  while (1) {
    struct request req ;

    char *array = calloc(50, sizeof(char));

    printf("what is your command (help for list of commands)\n");
    fgets(array, max_chars, stdin);
    if (array[max_chars-1] != '\0' && array[max_chars-1] != '\n'){
      printf("%s\n", "sorry input too large");
      break;
    }
    if (array[strlen(array) -1 ] == '\n'){
      array[strlen(array) -1] = '\0';
    }
    if (!strcmp(array, "help")){
      printf("add - add song to queue\nget_queue - Get Queue\nget_song - Download Song from Server\n");
    }
    if (!strcmp(array, "add")){
      printf("what song do you want add to the queue\n");
      fgets(array, max_chars, stdin);
      strcpy(req.request_type, "add");
      strcpy(req.song_name, array);
      write(sd, &req, sizeof(struct request));
    }
    if (!strcmp(array, "get_queue")){
      printf("Asking server for queue\n");
      strcpy(req.request_type, "get_queue");
      strcpy(req.song_name, array); // idk why but it wont work if both fields arent filled
      write(sd, &req, sizeof(struct request));

    }
    if (!strcmp(array, "get_song")){
      printf("Asking server for song\n");
      strcpy(req.request_type, "get_song");
// idk why but it wont work if both fields arent filled
      strcpy(req.song_name, array);
      write(sd, &req, sizeof(struct request));

    }


    // printf("%s\n", array);
    // send to server
    // write(sd, array, 50);
    // // recieve from server
    // read(sd,array, 50);
    // printf("child rot13: %s\n", array);
  }
}
