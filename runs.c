// File for executing commands

// includes
#include "shell.h"
#include "parsing.h"
#include "includes.h"

// executes all commands except for piping; takes an array of strings, the file descriptor to reidrect to, and file descriptor of either stdin or stdout; returns an int status
int execute(char **args, int fd, int dest) {
    if (strcmp(args[0], "exit") == 0) { // if exit, exit
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0) { // if cd, use chdir
        int status = chdir(args[1]);
        if (status) {
            return -1;
        }
        else return 0;
    }

    int f = fork(); // forks bcs execvp auto-exits
    if (f) {
        int status;
        wait(&status); // parent waits for return value
        int return_val = WEXITSTATUS(status);
        if (return_val) {
            return -1;
        }
        return 0;
    }
    else {
        if (fd != -1) { // if using redirection, redirects output
            if (dest == -2) { // -2 for dest signifies &>
                // dup(STDERR_FILENO);
                dup(STDOUT_FILENO);
                // dup2(fd, STDERR_FILENO);
                dup2(fd, STDOUT_FILENO);
            }
            else {
                dup(dest);
                dup2(fd, dest);
            }
        }
        int status = execvp(args[0], args);
        if (status == -1) {
            log_error(strerror(errno));
            // printf("testchild:\n");

            exit(0);
        }

        return 0;
    }

}
