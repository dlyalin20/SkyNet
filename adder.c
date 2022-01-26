/*
Handles
- set up of playlists
- set up of queues
- manipulation of queues
*/

#include "includes.h"
#include "adderhelpers.h"

void list_of_valid_commands(){ 
  printf("Warning: Max string length is %d, whitespace is not tolerated in prompts, and all songs are to be specified with .wav extension\n", BUFFER_SIZE);
  printf("Warning: Playlist max length is %d\n", BUFFER_SIZE);
  printf("-makep [playlist path]\n");
  printf("\tCreates a playlist from all songs in the specified directory\n");
  printf("-makeps\n");
  printf("\tCreates a playlist with specified songs\n");
  printf("-add2p\n");
  printf("\tAllows for adding a song to an extant playlist\n");
  printf("-ds\n");
  printf("\tAllows for removing a song from an extant playlist\n");
  printf("-permashuffle\n");
  printf("\tPermanently shuffles user-inputted playlist\n");
  printf("-savesort\n");
  printf("\tAllows for sorting a playlist permanently\n");
  printf("-addp2q [playlist name]\n");
  printf("\tAdds all the songs in specified playlist to queue\n");
  printf("-dq\n");
  printf("\tClears the queue\n");
  printf("-s2q [song name]\n");
  printf("\tAdds specified song to queue\n");
}

int main(int argc, char const *argv[]) {

  if (--argc < 1){
    // if there are no arguments
    printf("Please provide at least one argument\n");
    list_of_valid_commands();
    exit(0);
  }
  // make playlist from any directory
   if (!strcmp(argv[1], "-makep")) {
      printf("Creating playlist\n");
      make_playlist(argv[2]);
    }
    else
    // adds song to an extant playlist
    if (!strcmp(argv[1], "-add2p")) {
        add_to_playlist();
    }
    else
    // removes song from extant playlist
    if (!strcmp(argv[1], "-ds")) {
      remove_from_playlist();
    }
    else
    // permanently shuffles playlist
    if (!strcmp(argv[1], "-permashuffle")) {
      perma_shuffle();
    }
    else
    // saves a sorted playlist
    if (!strcmp(argv[1], "-savesort")) {
      sort_write();
    }
    else
    // make playlist from current working directory
    if (!strcmp(argv[1], "-makeps")) {
      printf("Creating simpled playlist\n");
      make_playlist_simple();
    }
    else
    // add playlist to queue
    if (!strcmp(argv[1], "-addp2q")){
      printf("Playlist Name: %s\n", argv[2]);
      add_playlist_to_queue(argv[2]);
    }
    else
    // clear_queue
    if (!strcmp(argv[1], "-dq")){
      clear_queue();
    }
    else
    // add_song_to_queue
    if (!strcmp(argv[1], "-s2q")){
      add_song_to_queue(argv[2]);
    }

    else{
    printf("%s is Not a valid command\n", argv[1]);
    list_of_valid_commands();
    exit(0);
  }
  return 0;
}
