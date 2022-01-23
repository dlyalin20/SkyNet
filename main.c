#include "includes.h"
#include "runs.h"
#include "parsing.h"
#include "shell.h"
// later remove files and see .o

int main(int argc, char *argv[]) {
  // test shuffle
  /* char *strarray[3] = {"a", "b", "c"};
  int i;
  for (i = 0; i < 3; i ++){
    printf("%s, ", strarray[i]);
  }
  printf("\nshuffle now:\n ");
  shuffle(strarray, 3);
  for (i = 0; i < 3; i ++){
    printf("%s, ", strarray[i]);
  }
  printf("\n"); */

	// initialize path
   if (!strcmp(argv[1], "init")) {
	   	initialize(argc, argv);
   }

   /* else if (!strcmp(argv[1], "launch")) {

     launch();
     printf("Shared memory created!");

   } */

   /* else if (!strcmp(argv[1], "launch")) {

	   launch_shell();

   } */

   else if (!strcmp(argv[1], "-play")) {
     printf("playing: %s\n", argv[2]);
	   char *song = calloc(BUFFER_SIZE, sizeof(char));
	   strcpy(song, argv[2]);
     //char **array = calloc(5, sizeof(char *));
     //split(song, array);
     char *extension = strrchr(song, '.');
     if (!strcmp(extension+1, "wav")) {
      play_wav(song);
     }
	 // CHECK FOR FILE FORMAT // ERROR CHECKING FOR ARG AMOUNTS

   }

   /* else if (!strcmp(argv[1], "unpause")) {

     unpause_wav();

   }


  else if(!strcmp(argv[1], "close")) {

    shut_down();

  } */

}
