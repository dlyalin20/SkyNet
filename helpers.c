#include "headers.h"

int initialize(int argc, char **argv) {

    int fd = open("data.txt", O_CREAT | O_EXCL | O_WRONLY | O_APPEND);
    char *path = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(path, "AUDPATH=");
    if (argc == 4 && !strcmp(argv[2], "-p")) {
        strcat(path, argv[3]);
        strcat(path, "\n");
        write(fd, path, strlen(path));
    }
    else {
        char *tmp = calloc(BUFFER_SIZE, sizeof(char));
        getcwd(tmp, BUFFER_SIZE);
        strcat(path, tmp);
        strcat(path, "\n");
        write(fd, path, strlen(path));
    }

    return 0;

}