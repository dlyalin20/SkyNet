// Used for main shell functionality

// includes
#include "runs.h"
#include "parsing.h"
#include "includes.h"

// initializer arrays for color printing
/* char *(colors[8]) = {BRED, BGRN, BYEL, BBLU, BMAG, BCYN, BWHT, reset};
char *(colorNames[8]) = {"RED", "GREEN", "YELLOW", "BLUE", "MAGENTA", "CYAN", "WHITE", "NORMAL"};
 */
// struct declarations
struct termios orig_termios;

// holds ease out function, returns delay between each color at specific time
float yvalue(float x) {
    return x*x / (2.0f * (x * x - x) + 1.0f);
}


// function for printing random colors; takes no args; returns random color
/* int randomizeColor() {
    int randomColor;
    // immediately flush stdout
    setbuf(stdout, NULL);
    // float delay = .1;
  	float t =0;

  	// for (; delay < 4; delay *= 1.25){
  	for (; t < 1; t += .1) {
    	printf("\33[2K\r");
    	randomColor = rand() % 8;
    	printf("%s", colors[randomColor]);
    	printf("Picking Terminal Color: %s", colorNames[randomColor]);
    	// fflush(stdout);
    	sleep(.75 + yvalue(t));
  	}
  	printf("\n");
  	// char x = '\n';
  	setlinebuf(stdout);
  	return randomColor;
} */

// Logs errors and events to errorlog; takes error message; returns void
void log_error(char *message) {
    printf("Error: %s\n", message); // prints all errors --> errors don't cause crashing
    int file = open("error_log.txt", O_CREAT | O_WRONLY | O_APPEND); // opens an error_log
    if (file == -1) {
        printf("Error opening error log: %s\n", strerror(errno));
    }
    int w = write(file, message, strlen(message));
    if (!w) {
        printf("Error writing to file: %s\n", strerror(errno));
    }
    w = write(file, "\n", 1); // adds newline character after
}

// signal handler; takes int signal; no return, always exits
static void sighandler(int sig) {
    if (sig == SIGINT) {
        printf("\nExiting Shell\n"); // exits shell gracefully on ^C
        exit(0);
    }
    if (sig == SIGSEGV) {
        log_error(strerror(errno)); // won't crash on segfaults // exits gracefully
        exit(-1);
    }
}
// https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html

// disables Raw Mode and makes stdin buffered; takes and returns nothing
void disableRawMode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// allows for reading one char at a time; takes and returns nothing
void enableRawMode() {
  	tcgetattr(STDIN_FILENO, &orig_termios);
  	atexit(disableRawMode);
  	struct termios raw = orig_termios;
  	// read in input char by char + dont print out charaters that are inputed
  	// we will handle that separately
  	raw.c_lflag &= ~(ECHO | ICANON);
  	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// prints out djshell prompt
void prompt(char * path, int keyBinds) {
  if (keyBinds){
    printf("%s djshell [KeyBindsOn]$ ", path);
  }else{
    printf("%s djshell $ ", path);
  }
}

// main launch loop; takes no args; returns an int (should always return 0)
int launch_shell() {

    srand( time(NULL) );
    // choose color
    // int choiceColor = randomizeColor();

    printf("Launching prompt\n");

    // signalhandler
    //signal(SIGSEGV, sighandler);
    //signal(SIGINT, sighandler);

    // uses a history file to track commands for lseeking (TO IMPLEMENT)
    int file = open("history.txt", O_CREAT | O_RDWR | O_APPEND, 0777);
    if (file == -1) {
        printf("Error with launching prompt\n");
        log_error(strerror(errno));
        exit(0);
    }

    printf("Please separate all arguments with spaces!\n");
    // open file to get history
    int history = open_history();
    int keyBinds = 1;
    // loops until exit is asked or ^C sent
    while (1) {

        lseek(history, 0, SEEK_END);
        // gets filepath to display

        char *tmp_path = calloc(CHARMAX, sizeof(char));
        getcwd(tmp_path, CHARMAX);
        char *path = strrchr(tmp_path, '/');

        // used for generating colored output
        prompt(path, keyBinds);
        fflush(stdout);
        char *buffer = calloc(CHARMAX, sizeof(char)); // fix sizing?
        char *dummybuffer = calloc(CHARMAX, sizeof(char)); // fix sizing?

        // waits for input from stdin
        // fgets(buffer, CHARMAX, stdin);
        // raw input
        enableRawMode();
        char c;
        char * escape = calloc(2, sizeof(char));
        unsigned int buffer_int = 0;
        int alreadyinputed = 0;
        while(read(STDIN_FILENO, &c, 1) == 1 && c != 10){
          if (iscntrl(c)) {
            if (c == 127){
              // backspace
              if (buffer_int >0){
                buffer_int--;
                buffer[buffer_int] = '\0';
                // clear current terminal line and reset to front of terminal
                printf("\33[2K\r");
                prompt(path, keyBinds);
                printf("%s", buffer);
                fflush(stdout);
              }
            }else{
              // printf("%d\n", c);
              read(STDIN_FILENO, escape, 2);
              // printf("ESCAPE charaters:'%s'\n", escape);
              if (!strcmp(escape, "[A")){
                // printf("uparrow pressed\n");
                // uparrow pressed
                alreadyinputed = 1;
                int size = prevhistory(history);
                parse_data(history, buffer, size, strlen(buffer));
                buffer_int = size;
                printf("\33[2K\r");
                prompt(path, keyBinds);
                printf("%s", buffer);
                fflush(stdout);
              }else if(!strcmp(escape, "[B")){
                // down arrow pressed
                nexthistory(history);
                // printf("%c\n", );
                int size = nexthistory(history);
                prevhistory(history);

                parse_data(history, buffer, size, strlen(buffer));
                buffer_int = size;

                printf("\33[2K\r");
                prompt(path, keyBinds);
                printf("%s", buffer);
                fflush(stdout);
              }else if(!strcmp(escape, "\t")){
                printf("TAB\n");
                // tab
                if (keyBinds == 1){
                  keyBinds=0;
                }else{
                  keyBinds=1;
                };
                prompt(path, keyBinds);
                fflush(stdout);

              }
            }

          // }else if((c == ' ')){

          }else if(keyBinds){
              printf("%c", c);
              // detect single charaters
              if (c == ' '){
                printf("SPACEBAR PRESSED\n");
              }
              buffer[buffer_int] = c;
              fflush(stdout);
              buffer_int ++;
            }
        }
        printf("\n");
        disableRawMode();
        // end of raw mode
        // if (alreadyinputed){
        //   // we need to rewrite the temp history command
        //   parse_prevhistory(file);
        //   // move the poiner back a line
        // }
        write(file, buffer, strlen(buffer));
        char * hold = "\n";
        write(file, hold, 1);

        char *tmp;
        // LONG COMMAND
        printf("STRING SENT %s\n", buffer);

    }
    return 0;
}
