#include "headers.h"



int main(int argc, char *argv[]) {

	// initialize path
   if (!strcmp(argv[1], "init")) {
	   	initialize(argc, argv);
   }

   if (!strcmp(argv[1], "-play")) {

	   char *song = calloc(BUFFER_SIZE, sizeof(char));
	   strcpy(song, argv[2]);
	   play_song(song); // CHECK FOR FILE FORMAT // ERROR CHECKING FOR ARG AMOUNTS

   }


}