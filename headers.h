#ifndef HEADERS
#define HEADERS "headers"

// stdincludes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <ctype.h>

// variables
#define BUFFER_SIZE 1000

// functions
int initialize(int argc, char **argv);

#endif