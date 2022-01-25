/*
Handles
- set up of playlists
- set up of queues
- manipulation of queues
*/

#include "includes.h"
#include "adderhelpers.h"

void list_of_valid_commands(){
  printf("-makeplaylist [playlist path]\n");
  printf("looks at all .wav files and saves them to be played\n");
}

int main(int argc, char const *argv[]) {
  if (--argc < 1){
    // if there are no arguements
    printf("Please provide at least one arguement\n");
    list_of_valid_commands();
    exit(0);
  }
    if (!strcmp(argv[1], "-makeplaylist")) {
      printf("Creating playlist\n");
      make_playlist(argv[2]);
      // make playlist from current working directory
    }else if (!strcmp(argv[1], "-makeplaylistsimple")) {
      printf("Creating simpled playlist\n");
      make_playlist_simple();
      // --
    }else{
    printf("%s is Not a valid command\n", argv[1]);
    list_of_valid_commands();
    exit(0);
  }
  return 0;
}
