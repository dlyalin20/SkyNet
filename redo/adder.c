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
  // make playlist from any directory: working on intergrating it
   if (!strcmp(argv[1], "-makeplaylist")) {
      printf("Creating playlist\n");
      make_playlist(argv[2]);
    }else
    // make playlist from current working directory
    if (!strcmp(argv[1], "-makeplaylistsimple")) {
      printf("Creating simpled playlist\n");
      make_playlist_simple();
    }else
    // add playlist to queue
    if (!strcmp(argv[1], "-addplaylisttoqueue")){
      printf("Playlist Name: %s\n", argv[2]);
      add_playlist_to_queue(argv[2]);
    }else
    // clear_queue
    if (!strcmp(argv[1], "-clearqueue")){
      clear_queue();
    }else
    // add_song_to_queue
    if (!strcmp(argv[1], "-addsongtoqueue")){
      add_song_to_queue(argv[2]);
    }

    else{
    printf("%s is Not a valid command\n", argv[1]);
    list_of_valid_commands();
    exit(0);
  }
  return 0;
}
