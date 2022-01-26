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
  printf("-p [song path]\n");
  printf("\tPlays specified song\n");
  printf("-pq\n");
  printf("\tPlay the current queue\n");
  printf("-pp [playlist name]\n");
  printf("\tPlays the specified playlist\n");
  printf("-playsort\n");
  printf("\tAllows for sorting and playing a playlist without permanently altering it\n");
  printf("-playshuffle\n");
  printf("\tShuffles and plays playlist without permanently altering it\n");
}
int main(int argc, char const *argv[]) {
  if (--argc < 1){
    // if there are no arguments
    printf("Please provide at least one argument\n");
    list_of_valid_commands();
    exit(0);
  }
  // -play [song name (including '.wav')]
  if (!strcmp(argv[1], "-p")) {
    printf("playing: %s\n", argv[2]);
    char *song = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(song, argv[2]);
    char *extension = strrchr(song, '.');
    if (!strcmp(extension+1, "wav")) 
     play_wav(song);
    else
      printf("Only .wav files can be played\n");
    free(song);
  } else 
  // plays queue
  if (!strcmp(argv[1], "-pq")) 
    play_queue();
  else 
  // -playplaylist [playlist name]
  if (!strcmp(argv[1], "-pp")) 
    play_playlist(argv[2]);
  else 
  // plays sorted playlist
  if (!strcmp(argv[1], "-playsort")) 
      play_sorted();
  else
  // shuffles and plays playlist
  if (!strcmp(argv[1], "-playshuffle")) {
    play_shuffle();
  }
  else {
    printf("%s is Not a valid command\n", argv[1]);
    list_of_valid_commands();
    exit(0);
  }
  return 0;
}
