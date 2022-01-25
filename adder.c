/*
Handles
- set up of playlists
- set up of queues
- manipulation of queues
*/

#include "includes.h"
#include "adderhelpers.h"

void list_of_valid_commands(){ // finish this
  printf("Warning: Max string length is %d, whitespace is not tolerated in prompts, and all songs are to be specified with .wav extension\n", BUFFER_SIZE);
  printf("Warning: Playlist max length is %d\n", BUFFER_SIZE);
  printf("-makeplaylist [playlist path]\n");
  printf("\tCreates a playlist from all songs in the specified directory\n");
  printf("-makeplaylistsimple\n");
  printf("\tCreates a playlist with specified songs\n");
  printf("-addtoplaylist\n");
  printf("\tAllows for adding a song to an extant playlist\n");
  printf("-removefromplaylist\n");
  printf("\tAllows for removing a song from an extant playlist\n");
  printf("-savesort\n");
  printf("\tAllows for sorting a playlist permanently\n");
  printf("-playsort\n");
  printf("\tAllows for sorting and playing a playlist without permanently altering it\n");
  printf("-addplaylisttoqueue [playlist name]\n");
  printf("\tAdds all the songs in specified playlist to queue\n");
  printf("-clearqueue\n");
  printf("\tClears the queue\n");
  printf("-addsongtoqueue [song name]\n");
  printf("\tAdds specified song to queue\n");
}

int main(int argc, char const *argv[]) {
  if (--argc < 1){
    // if there are no arguements
    printf("Please provide at least one argument\n");
    list_of_valid_commands();
    exit(0);
  }
  // make playlist from any directory: working on intergrating it
   if (!strcmp(argv[1], "-makeplaylist")) {
      printf("Creating playlist\n");
      make_playlist(argv[2]);
    }
    else
    // adds song to an extant playlist
    if (!strcmp(argv[1], "-addtoplaylist")) {
        add_to_playlist();
    }
    else
    // removes song from extant playlist
    if (!strcmp(argv[1], "-removefromplaylist")) {
      remove_from_playlist();
    }
    else
    // saves a sorted playlist
    if (!strcmp(argv[1], "-savesort")) {
      sort_write();
    }
    else
    // plays sorted playlist only
    if (!strcmp(argv[1], "-playsort")) {
      play_sorted();
    }
    else
    // make playlist from current working directory
    if (!strcmp(argv[1], "-makeplaylistsimple")) {
      printf("Creating simpled playlist\n");
      make_playlist_simple();
    }
    else
    // add playlist to queue
    if (!strcmp(argv[1], "-addplaylisttoqueue")){
      printf("Playlist Name: %s\n", argv[2]);
      add_playlist_to_queue(argv[2]);
    }
    else
    // clear_queue
    if (!strcmp(argv[1], "-clearqueue")){
      clear_queue();
    }
    else
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
