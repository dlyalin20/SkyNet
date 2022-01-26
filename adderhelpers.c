#include "includes.h"
#include "adderhelpers.h"
#include "playerhelpers.h"
/*
Holds functions that relate to song storage
*/

// takes int; handles signal by killing jobs; returns void
static void sighandler(int sigint) {

  if (sigint == SIGINT) {
    printf("Ctrl+C sensed; Quitting\n");
    kill(0, SIGKILL); // kills all jobs
    exit(0);
  }
  if (sigint == SIGSTOP) {
    printf("Ctrl+Z sensed; Quitting\n");
    kill(0, SIGKILL);
    exit(0);
  }

}

// takes file name; returns duration of song as float
float get_duration(char *filename) {

    signal(SIGINT, &sighandler);
    signal(SIGSTOP, &sighandler);

    FILE *fp = fopen(filename, "rb"); // opens file for reading in binary
    if (fp == NULL) {
      printf("Error Opening File for Duration: %s\n", strerror(errno));
      exit(-1);
    }

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    int err = fread(wav_header, 1, sizeof(struct WAV), fp); // reads in wav header
    if (err != sizeof(struct WAV)) {
      printf("Error Reading Wav_Header: %s\n", strerror(errno));
      exit(-1);
    }

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds; // algo for calculating duration

    fclose(fp);
    free(wav_header);

    return duration_in_milliseconds;

}

// takes song_info struct pointer and filepath; reads file metadata into struct; returns struct
struct song_info * get_song_info(struct song_info *info, char *PATH) {

    signal(SIGINT, &sighandler);
    signal(SIGSTOP, &sighandler);

    int err;
    char *cmd = "exiftool";
    // all args for getting json file of metadata using exiftool
    char *args[] = {cmd, "-charset", "filename=utf8", "-charset", "exif=utf8", "-charset", "iptc=utf8", "-use", "mwg", "-m", "-G0:1", "-wm", "wcg", "-a", "-j", "-struct", "-w!", "\%d\%f.json", "-All", PATH, NULL};
    int wstatus;
    int pid = fork();
    if (!pid) { // child process for using execvp
        err = execvp(cmd, args);
        if (err) {
          printf("Error Executing Command for JSON: %s\n", strerror(errno));
          exit(-1);
        }
        exit(0);
    }
    else {
        wait(&wstatus);
        if (WEXITSTATUS(wstatus)) {
          exit(-1); // if failed 
        }
    }

    struct stat stats;
    char *pathcopy = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(pathcopy, PATH);
    char *lastExt = strrchr(pathcopy, '.');
    *lastExt = '\0';
    strcat(pathcopy, ".json"); // for opening json file (name.wav -> name.json)
    stat(pathcopy, &stats);
    char *buffer = calloc(stats.st_size, sizeof(char));
    FILE *fd = fopen(pathcopy, "r");
    if (fd == NULL) {
      printf("Error opening path for song_info: %s\n", strerror(errno));
      exit(-1);
    }
    fread(buffer, 1, 1, fd);
    fread(buffer, 1, stats.st_size-3, fd);
    cJSON *song_json = cJSON_Parse(buffer); // reads json to buffer and then to cJSON struct

    // jason workaround ---- reads fields from cJSON struct into song_info struct
    cJSON *cTitle = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Title");
    char unknownstring[] = "Unknown";
    if (cJSON_IsString(cTitle) && (cTitle->valuestring != NULL)) 
        strcpy(info->title, cTitle->valuestring);
    else 
        strcpy(info->title, unknownstring);

    strcpy(info->path, PATH);

    cJSON *cArtist = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Artist");
    if (cJSON_IsString(cArtist) && (cArtist->valuestring != NULL)) 
        strcpy(info->artist, cArtist->valuestring);
    else
        strcpy(info->artist, unknownstring);
    

    cJSON *cGenre = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Genre");
    if (cJSON_IsString(cGenre) && (cGenre->valuestring != NULL)) 
      strcpy(info->genre, cGenre->valuestring);
    else
      strcpy(info->genre, unknownstring);

    info->seconds = get_duration(PATH);

    remove(pathcopy);
    free(pathcopy);
    free(buffer);

    return info;

}

 // takes playlist name as string; deletes playlist file; returns int
int delete_playlist(char *playlist_name) {

    signal(SIGINT, &sighandler);
    signal(SIGSTOP, &sighandler);
    return remove(playlist_name); // removes file

}

// takes char buffer and file stream; writes buffer contents to file; returns null
void s2f(char *buffer, FILE *file) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  struct song_info *tmp = calloc(1, sizeof(struct song_info));
  get_song_info(tmp, buffer); // fills song_info with song info
  fwrite(tmp->path, BUFFER_SIZE, 1, file);
  fwrite(tmp->artist, BUFFER_SIZE, 1, file);
  fwrite(tmp->title, BUFFER_SIZE, 1, file);
  fwrite(tmp->genre, BUFFER_SIZE, 1, file);
  fwrite(&(tmp->seconds), sizeof(tmp->seconds), 1, file); // writes song_info struct to file
  free(tmp);

}

// takes string buffer; prompts for playlist name; returns null
void get_pname(char *buffer) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  printf("Please input playlist name, or 'exit': \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  char *rem;
  rem = strrchr(buffer, '\n');
  rem[0] = '\0';
}

// takes string buffer; prompts for song name; returns null
void get_sname(char *buffer) {

    signal(SIGINT, &sighandler);
    signal(SIGSTOP, &sighandler);

    printf("Please input a song to add, or type exit: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *end;
    end = strrchr(buffer, '\n');
    end[0] = '\0';

}

// takes null; makes playlist by prompting for songs; returns int
int make_playlist_simple() { 

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer); // prompts for playlist name

  FILE *file = fopen(buffer, "wb+"); // opens for binary writing

  if (file == NULL) {
    printf("Error: %s\n", strerror(errno));
    exit(-1);
  }

  struct stat stats;
  while (1) { // loops until exit or error or signal
    get_sname(buffer); // prompts for song name
    char * ext = strrchr(buffer,'.'); // check for .wav
    if (!strcmp(buffer, "exit"))
      break;
    if (!stat(buffer, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
      s2f(buffer, file); // writes song to file
    }
    else {
      printf("%s doesn't exist\n", buffer);
    }
  }

  free(buffer);
  fclose(file);

  printf("Playlist Created\n");

  return 0;

}

// takes null; makes a playlist; returns int
int make_playlist(const char * path) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  // copy target path
  char *filelocation = calloc(BUFFER_SIZE, sizeof(char));
  strcpy(filelocation, path);
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer);
  char playlist_name[BUFFER_SIZE];
  strcpy(playlist_name, buffer);
  strcat(filelocation, "/");
  strcat(filelocation, playlist_name);
  FILE *file = fopen(playlist_name, "wb+");
  if (file == NULL) {
    printf("Error: %s\n", strerror(errno));
    exit(-1);
  }
  // search songs from the target path
  struct stat stats;
  while (1) {
    get_sname(buffer);
    strcpy(filelocation, path);
    strcat(filelocation, "/");
    strcat(filelocation, buffer);
    printf("%s\n", filelocation);
    // filelocation is now [path]/song'/0'
    char * ext = strrchr(filelocation,'.');
    if (!strcmp(buffer, "exit"))
      break;
    if (!stat(filelocation, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
      strcpy(buffer, filelocation);
      s2f(buffer, file);
    }
    else {
      printf("%s doesn't exist\n", filelocation);
    }
  }
  free(buffer);
  free(filelocation);
  fclose(file);
  printf("Playlist Created\n");
  return 0;
}

// takes null; adds song to playlist; returns int
int add_to_playlist() {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer); // prompts for playlist name
  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }
  FILE *file = fopen(buffer, "wb+"); // opens for binary writing
  if (file == NULL) {
    printf("Error opening playlist: %s\n", strerror(errno));
    free(buffer);
    exit(-1);
  }
  fseek(file, 0, SEEK_END); // writing to end
  get_sname(buffer); // prompts for song name
  struct stat stats;
  char *ext = strrchr(buffer, '.'); // make sure .wav
  if (!stat(buffer, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
    s2f(buffer, file); // writes song to file
  }
  else {
    printf("Error finding that song!\n");
  }
  fclose(file);
  free(buffer);
  return 0;

}

// takes playlist, file stream, and int pointer; reads all of playlist into file; returns null
void p2f(struct song_info *playlist, FILE *file, int *i) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  int n; // loops thru entire playlist
  for (n = 0; n < *i; n++) {
    fwrite(playlist[n].path, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[n].artist, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[n].title, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[n].genre, BUFFER_SIZE, sizeof(char), file);
    fwrite(&(playlist[n].seconds), sizeof(float), sizeof(char), file);
  }

}


// takes null; removes song from playlist; returns null
void remove_from_playlist() {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));

  get_pname(buffer); // prompts for playlist

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb"); // opens for binary reading

  char *filename = calloc(BUFFER_SIZE, sizeof(char));
  strcpy(filename, buffer);

  get_sname(buffer); // prompts for song name

  fseek(file, 0, SEEK_END);
  int bytsiz = ftell(file);
  fseek(file, 0, SEEK_SET); // set to end to get number of bytes

  struct song_info playlist[BUFFER_SIZE];

  int i, n;
  struct song_info *tmp = calloc(sizeof(struct song_info), 1);
  for (i = 0; i < BUFFER_SIZE && bytsiz != ftell(file); i++) { // loops until done or at last byte

    fread(tmp->path, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->artist, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->title, BUFFER_SIZE, sizeof(char), file);
    if (!strcmp(buffer, tmp->title)) {
      fseek(file, BUFFER_SIZE + sizeof(float), SEEK_CUR);
      continue;
    }
    fread(tmp->genre, BUFFER_SIZE, sizeof(char), file);
    fread(&(tmp->seconds), sizeof(float), 1, file);
    playlist[n] = *tmp; // adds every song to playlist
    n++;
  }

  free(tmp);
  fclose(file); 
  remove(filename); // deletes file bcs impossible to overwrite; easier to just remake

  file = fopen(filename, "wb"); // opens for binary writing

  p2f(playlist, file, &n); // writes playlist to file

  fclose(file);
  free(filename);
  free(buffer);

}

// takes playlist and length; sorts by title alphabetically; returns null
void sort_title(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  int i, j;
  int n = size;
  char *key = calloc(BUFFER_SIZE, 1);
    for (i = 1; i < n; i++) {
        strcpy(key, arr[i].path); // just for now
        j = i - 1;
        while (j >= 0 && strcmp(arr[j].path, key)>0) {
            struct song_info *temp = &arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = *temp;
            j--;
        }
        strcpy(arr[j + 1].path, key);
    }
  free(key);
}

// takes playlist and length; sorts by artist alphabetically; returns null
void sort_artist(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  sort_title(arr, size);
  int i, j;
  int n = size;
  char *key = calloc(BUFFER_SIZE,1);
    for (i = 1; i < n; i++) {
        strcpy(key,arr[i].artist);
        j = i - 1;
        while (j >= 0 && strcmp(arr[j].artist, key)>0) {
            struct song_info *temp = &arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = *temp;
            j--;
            free(temp);
        }
        strcpy(arr[j + 1].artist, key);
    }
  free(key);
}

// takes playlist and length; sorts by genre alphabetically; returns null
void sort_genre(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  sort_artist(arr,size);
  int i, j;
  int n = size;
  char *key = calloc(BUFFER_SIZE,1);
    for (i = 1; i < n; i++) {
        strcpy(key, arr[i].genre);
        j = i - 1;
        while (j >= 0 && strcmp(arr[j].genre, key)>0) {
            struct song_info *temp = &arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = *temp;
            j--;
            free(temp);
        }
        strcpy(arr[j + 1].genre, key);
    }
  free(key);
}

// takes playlist and length; sorts by duration in increasing order; returns null
void sort_duration(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  int i, j;
  int n = size;
  float key = 0;
    for (i = 1; i < n; i++) {
        key = arr[i].seconds;
        j = i - 1;
        while (j >= 0 && arr[j].seconds > key) {
            struct song_info *temp = &arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = *temp;
            j--;
        }
        arr[j+1].seconds = key;
    }
}

// takes playlist, file stream, and int pointer; reads file stream into playlist; returns null
void f2p(struct song_info *playlist, FILE *file, int *i) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  fseek(file, 0, SEEK_END);
  int bytsiz = ftell(file);
  fseek(file, 0, SEEK_SET); // seek end to know last byte to know size in bytes

  struct song_info *tmp = calloc(sizeof(struct song_info), 1);
  for (*i = 0; *i < BUFFER_SIZE && ftell(file) != bytsiz; (*i)++) { // loop until finished or at last byte
    fread(tmp->path, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->artist, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->title, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->genre, BUFFER_SIZE, sizeof(char), file);
    fread(&(tmp->seconds), sizeof(float), 1, file);
    playlist[*i] = *tmp; // adds every struct to playlist
  }

  free(tmp);

}

// takes a string buffer, playlist, file stream, and int pointer; prompts user for how to sort and sorts; returns null
void sort(char *buffer, struct song_info *playlist, FILE *file, int *i) {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  printf("Sort by 'title', 'artist', 'genre', or 'duration', or 'exit': \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE); 
  char *rm2 = strrchr(buffer, '\n');
  rm2[0] = '\0';

  if (!strcmp(buffer,"exit")) {
    fclose(file);
    free(buffer);
  }
  if (!strcmp(buffer, "title")) {
    sort_title(playlist, *i);
  }
  else if (!strcmp(buffer, "artist")) {
    sort_artist(playlist, *i);
  }
  else if (!strcmp(buffer, "genre")) {
    sort_genre(playlist, *i);
  }
  else if (!strcmp(buffer, "duration")) {
    sort_duration(playlist, *i);
  }
  else {
    printf("No such sorting type!\n");
    exit(-1);
  }

}

// takes null; sorts playlist permanently; returns null
void sort_write() {

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer); // prompts for playlist name

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb"); // opens for binary reading
  if (file == NULL) {
    printf("Error opening playlist: %sn", strerror(errno));
  }

  struct song_info playlist[BUFFER_SIZE]; 
  int i;
  f2p(playlist, file, &i); // reads file into playlist

  sort(buffer, playlist, file, &i); // sorts playlist as user asks

  fseek(file, 0, SEEK_SET); // sets back to beginning

  p2f(playlist, file, &i); // writes playlist to file (overwrites current bytes) (length remains the same)

  fclose(file);
  free(buffer);

}

// takes null; creates semaphore; returns int
int create_sema(){

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  // create semaphore
  int v, r;
  int semd = semget(QUEUEKEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd == -1) {
    semd = semget(QUEUEKEY, 1, 0);
    v = semctl(semd, 0, GETVAL, 0);
  }
  else {
    union semun us;
    us.val = 1;
    r = semctl(semd, 0, SETVAL, us);
  }
  return semd;
}

// takes file path; adds song on path to queue; returns int
int add_song_to_queue(const char * path){

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  char cppath[BUFFER_SIZE];
  strcpy(cppath, path);
  struct song_info *tmp = calloc(1, sizeof(struct song_info));
  get_song_info(tmp, cppath);

  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  semop(semd, &sb, 1); //preform operation

  int queue = open("queue", O_WRONLY | O_APPEND | O_CREAT, 0664);
  write(queue, tmp->path, BUFFER_SIZE);
  write(queue, tmp->artist, BUFFER_SIZE);
  write(queue, tmp->title, BUFFER_SIZE);
  write(queue, tmp->genre, BUFFER_SIZE);
  write(queue, &(tmp->seconds), sizeof(tmp->seconds));
  printf("Added %s to queue\n", tmp->title);
  sb.sem_op = 1; //set the operation to up;
  semop(semd, &sb, 1); //preform operation

  free(tmp);

  return 0;
}

// takes playlist name; adds all playlist songs to queue; returns int
int add_playlist_to_queue(const char * name){

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  semop(semd, &sb, 1); //preform operation
  // NOTE you cannot use append mode when using sendfile();
  int queue = open("queue", O_WRONLY | O_APPEND | O_CREAT, 0664);
  int playlist = open(name, O_RDONLY);
  struct stat st;
  stat(name, &st);
  int playlist_size = st.st_size;
  stat("queue", &st);
  int queue_size = st.st_size;
  char x[playlist_size];
  printf("Read %zd bytes\n", read(playlist, x, playlist_size));
  printf("Wrote %zd bytes\n", write(queue, x, playlist_size));

  printf("Added playlist %s to queue\n", name);
  sb.sem_op = 1; //set the operation to up;
  semop(semd, &sb, 1); //preform operation

  close(playlist);

  return 0;
}

// takes null; clears the queue; returns int
int clear_queue(){

  signal(SIGINT, &sighandler);
  signal(SIGSTOP, &sighandler);

  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  semop(semd, &sb, 1); //preform operation
  FILE * hold = fopen("queue", "w");
  fclose(hold);
  printf("Cleared Queue\n");
  sb.sem_op = 1; //set the operation to up;
  semop(semd, &sb, 1); //preform operation
  return 0;
}

// takes playlist and size of playlist; shuffles playlist; returns void
void shuffle(struct song_info *arr, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = rand() % n;
          struct song_info t = arr[j];
          arr[j] = arr[i];
          arr[i] = t;
        }
    }
}

// takes null; permanently shuffles playlist; returns null
void perma_shuffle() {

  char *buffer;
  get_pname(buffer); // prompts for playlist name

  char *filename = calloc(BUFFER_SIZE, sizeof(char));
  strcpy(filename, buffer);

  FILE *file = fopen(buffer, "rb");
  if (file == NULL) {
    printf("Error opening playlist: %s\n", strerror(errno));
  }
  int i;
  struct song_info playlist[BUFFER_SIZE];
  f2p(playlist, file, &i); // copies file into playlist

  shuffle(playlist, i); // shuffles playlist

  remove(filename); // removes old file

  file = fopen(filename, "wb"); // creates new file
  if (file == NULL) {
    printf("Error opening playlist: %s\n", strerror(errno));
  }

  i = 0;
  p2f(playlist, file, &i); // writes playlist to file

  printf("Playlist shuffled!\n");
}
