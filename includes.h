// Standard includes used everywhere

#ifndef INCLUDES
#define INCLUDES "defined"

// preprocessor variables
#define CHARMAX 100
#define BUFFER_SIZE 1000
#define MUS_PATH "ex1.wav"

// standard includes
#include "runs.h"
#include "shell.h"
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
#include <unistd.h>
#include "parsing.h"
#include <termios.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <SDL2/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
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
int play_song(char *song);
int initialize(int argc, char **argv);