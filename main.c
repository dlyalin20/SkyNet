#include "includes.h"
#include "runs.h"
#include "parsing.h"
#include "shell.h"

// later remove files and see .o

int main(int argc, char *argv[]) {

	// initialize path
   if (!strcmp(argv[1], "init")) {
	   	initialize(argc, argv);
   }

   else if (!strcmp(argv[1], "launch")) {

	   launch_shell();

   }

   if (!strcmp(argv[1], "-play")) {
     printf("playing: %s\n", argv[2]);
	   char *song = calloc(BUFFER_SIZE, sizeof(char));
	   strcpy(song, argv[2]);
	   play_song(song); // CHECK FOR FILE FORMAT // ERROR CHECKING FOR ARG AMOUNTS

   }


}
