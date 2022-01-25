// Standard includes used everywhere

#ifndef INCLUDES
#define INCLUDES "defined"

// preprocessor variables
#define BUFFER_SIZE 100
#define MUS_PATH "ex1.wav"
/* #define secKey ftok("main.c", getpid());
#define specKey ftok("shell.c", getpid());
#define lenKey ftok("helpers.c", getpid());
#define bufKey ftok("parsing.c", getpid()); */

#define secKey 100
#define specKey 200
#define lenKey 300
#define bufKey 400
#define IDKey 500
#define maxKey 600

// standard includes
#include "cJSON.h"
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include "parsing.h"
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <SDL2/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "linked_list.h"
#endif

// structs

struct WAV {

	char chunk_id[4]; // "RIFF" 4 bytes
	int chunk_size; // 405040 4 bytes
	char format[4]; // "WAVE" 4 bytes
	char subchunk1_id[4]; // "fmt" 4 bytes
	int subchunk1_size; // 16 4 bytes
	short audio_format; // 1 2 bytes
	short num_channels; // 2 2 bytes
	int sample_rate; // 22050 4 bytes
	int byte_rate; // 88200 4 bytes
	short block_align; // 4 2 bytes
	short bits_per_sample; // 16 2 bytes
	char subchunk2_id[4]; // "data" 4 bytes
	int subchunk2_size; // 405004 4 bytes

};

// functions
void shuffle(char **array, size_t n);
int play_wav(char *song);
int logic_controller(char *buffer);
int initialize(int argc, char **argv);
int make_playlist();
//int launch();
//int shut_down();
//static void sighandler(int sig);
//int unpause_wav();
