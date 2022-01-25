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
  printf("Warning: Max string length is %d, whitespace is not tolerated in prompts, and all songs are to be specified with .wav extension\n", BUFFER_SIZE);
  printf("Warning: Once a song is playing, it can only be paused or escaped. Escaping is equivalent to skipping.\n");
  printf("-play [song path]\n");
  printf("\tPlays specified song\n");
  printf("-playqueue\n");
  printf("\tPlay the current queue\n");
  printf("-playplaylist [playlist name]\n");
  printf("\tPlays the specified playlist\n");
}
int main(int argc, char const *argv[]) {
  if (--argc < 1){
    // if there are no arguments
    printf("Please provide at least one argument\n");
    list_of_valid_commands();
    exit(0);
  }
  if (!strcmp(argv[1], "-play")) {
    printf("playing: %s\n", argv[2]);
    char *song = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(song, argv[2]);
    char *extension = strrchr(song, '.');
    if (!strcmp(extension+1, "wav")) 
     play_wav(song);
    else
      printf("Only .wav files can be played\n");
    free(song);
  }
  else if (!strcmp(argv[1], "-playqueue"))
    play_queue();
  else if (!strcmp(argv[1], "-playplaylist"))
    play_playlist(argv[2]);
  else {
    printf("%s is Not a valid command\n", argv[1]);
    list_of_valid_commands();
    exit(0);
  }
  return 0;
}
