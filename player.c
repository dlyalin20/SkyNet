/*
Handles
- playing of music
    - from queue
    - from song files [for mainly testing purposes]
- listening controls
*/

#include "includes.h"
#include "playerhelpers.h"

void list_of_valid_commands(){
  printf("-play [song path]\n");
  printf("play this song path\n");
  printf("-playqueue\n");
  printf("play the current queue\n");
  printf("-playplaylist [playlist name]\n");

}
int main(int argc, char const *argv[]) {
  if (--argc < 1){
    // if there are no arguements
    printf("Please provide at least one arguement\n");
    list_of_valid_commands();
    exit(0);
  }
  if (!strcmp(argv[1], "-play")) {
    printf("playing: %s\n", argv[2]);
    char *song = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(song, argv[2]);
    char *extension = strrchr(song, '.');
    if (!strcmp(extension+1, "wav")) {
     play_wav(song);
    }
  }else if(!strcmp(argv[1], "-playqueue")){
    play_queue();
  }else if(!strcmp(argv[1], "-playplaylist")){
    play_playlist(argv[2]);
  }else{
    printf("%s is Not a valid command\n", argv[1]);
    list_of_valid_commands();
    exit(0);
  }
  return 0;
}
