#include "includes.h"
#include "adderhelpers.h"
#include "playerhelpers.h"
/*
Holds functions that relate to song storage
*/

static void sighandler(int sigint) {

  if (sigint == SIGINT) {
    printf("Ctrl+C sensed; Quitting\n");
    kill(0, SIGKILL);
    exit(0);
  }
  if (sigint == SIGSTOP) {
    printf("Ctrl+Z sensed; Quitting\n");
    kill(0, SIGKILL);
    exit(0);
  }

}

float get_duration(char *filename) {

    signal(SIGINT, &sighandler);

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
      printf("Error Opening File for Duration: %s\n", strerror(errno));
      exit(-1);
    }

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    int err = fread(wav_header, 1, sizeof(struct WAV), fp);
    if (err != sizeof(struct WAV)) {
      printf("Error Reading Wav_Header: %s\n", strerror(errno));
      exit(-1);
    }

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds;

    fclose(fp);
    free(wav_header);

    return duration_in_milliseconds;

}

struct song_info * get_song_info(struct song_info *info, char *PATH) {

    signal(SIGINT, &sighandler);

    int err;
    char *cmd = "exiftool";
    char *args[] = {cmd, "-charset", "filename=utf8", "-charset", "exif=utf8", "-charset", "iptc=utf8", "-use", "mwg", "-m", "-G0:1", "-wm", "wcg", "-a", "-j", "-struct", "-w!", "\%d\%f.json", "-All", PATH, NULL};
    int wstatus;
    int pid = fork();
    if (!pid) { // child process
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
          exit(-1);
        }
    }

    struct stat stats;
    char *pathcopy = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(pathcopy, PATH);
    char *lastExt = strrchr(pathcopy, '.');
    *lastExt = '\0';
    strcat(pathcopy, ".json");
    stat(pathcopy, &stats);
    char *buffer = calloc(stats.st_size, sizeof(char));
    FILE *fd = fopen(pathcopy, "r");
    if (fd == NULL) {
      printf("Error opening path for song_info: %s\n", strerror(errno));
      exit(-1);
    }
    fread(buffer, 1, 1, fd);
    fread(buffer, 1, stats.st_size-3, fd);
    cJSON *song_json = cJSON_Parse(buffer);

    // jason workaround ----
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

struct song_info * find_files(struct song_info *song_data, char * path) {

  signal(SIGINT, &sighandler);

  DIR * d = opendir(path);
  struct dirent *entry;

  int i = 0;
  while ((entry = readdir(d))){
    if (entry->d_type == DT_REG){
      char * ext = strrchr(entry->d_name,'.');
      if (ext != NULL && !strcmp(ext,".wav")){
        struct song_info *tmp = calloc(1, sizeof(struct song_info));
        get_song_info(tmp, entry->d_name);
        song_data[i] = *tmp;
        i++;
      }
    }
  }
  return song_data;
}

int delete_playlist(char *playlist_name) {

    signal(SIGINT, &sighandler);
    return remove(playlist_name);

}

void s2f(char *buffer, FILE *file) {

  struct song_info *tmp = calloc(1, sizeof(struct song_info));
  get_song_info(tmp, buffer);
  fwrite(tmp->path, BUFFER_SIZE, 1, file);
  fwrite(tmp->artist, BUFFER_SIZE, 1, file);
  fwrite(tmp->title, BUFFER_SIZE, 1, file);
  fwrite(tmp->genre, BUFFER_SIZE, 1, file);
  fwrite(&(tmp->seconds), sizeof(tmp->seconds), 1, file);
  free(tmp);

}

void get_pname(char *buffer) {
  printf("Please input playlist name, or 'exit': \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  char *rem;
  rem = strrchr(buffer, '\n');
  rem[0] = '\0';
}

void get_sname(char *buffer) {

    printf("Please input a song to add, or type exit: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *end;
    end = strrchr(buffer, '\n');
    end[0] = '\0';

}

int make_playlist_simple() { 

  signal(SIGINT, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer);

  FILE *file = fopen(buffer, "wb+");

  if (file == NULL) {
    printf("Error: %s\n", strerror(errno));
    exit(-1);
  }

  struct stat stats;
  while (1) {
    get_sname(buffer);
    char * ext = strrchr(buffer,'.');
    if (!strcmp(buffer, "exit"))
      break;
    if (!stat(buffer, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
      s2f(buffer, file);
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

int make_playlist(const char * path) {

  signal(SIGINT, &sighandler);

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

int add_to_playlist() {

  signal(SIGINT, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer);
  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }
  FILE *file = fopen(buffer, "wb+");
  if (file == NULL) {
    printf("Error opening playlist: %s\n", strerror(errno));
    free(buffer);
    exit(-1);
  }
  fseek(file, 0, SEEK_END);
  get_sname(buffer);
  struct stat stats;
  char *ext = strrchr(buffer, '.');
  if (!stat(buffer, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
    s2f(buffer, file);
  }
  else {
    printf("Error finding that song!\n");
  }
  fclose(file);
  free(buffer);
  return 0;

}

void p2f(struct song_info *playlist, FILE *file, int *i) {

  int n;
  for (n = 0; n < *i; n++) {
    fwrite(playlist[n].path, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[n].artist, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[n].title, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[n].genre, BUFFER_SIZE, sizeof(char), file);
    fwrite(&(playlist[n].seconds), sizeof(float), sizeof(char), file);
  }

}

void remove_from_playlist() {

  signal(SIGINT, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));

  get_pname(buffer);

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb");

  char *filename = calloc(BUFFER_SIZE, sizeof(char));
  strcpy(filename, buffer);

  get_sname(buffer);

  fseek(file, 0, SEEK_END);
  int bytsiz = ftell(file);
  fseek(file, 0, SEEK_SET);

  struct song_info playlist[BUFFER_SIZE];

  int i, n;
  struct song_info *tmp = calloc(sizeof(struct song_info), 1);
  for (i = 0; i < BUFFER_SIZE && bytsiz != ftell(file); i++) {

    fread(tmp->path, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->artist, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->title, BUFFER_SIZE, sizeof(char), file);
    if (!strcmp(buffer, tmp->title)) {
      fseek(file, BUFFER_SIZE + sizeof(float), SEEK_CUR);
      continue;
    }
    fread(tmp->genre, BUFFER_SIZE, sizeof(char), file);
    fread(&(tmp->seconds), sizeof(float), 1, file);
    playlist[n] = *tmp;
    n++;
  }

  free(tmp);
  fclose(file);
  remove(filename);

  file = fopen(filename, "wb");

  p2f(playlist, file, &n);

  fclose(file);
  free(filename);
  free(buffer);

}

void sort_title(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);

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

void sort_artist(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);

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

void sort_genre(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);

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

void sort_duration(struct song_info *arr, int size) {

  signal(SIGINT, &sighandler);

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

void f2p(struct song_info *playlist, FILE *file, int *i) {

  fseek(file, 0, SEEK_END);
  int bytsiz = ftell(file);
  fseek(file, 0, SEEK_SET);

  struct song_info *tmp = calloc(sizeof(struct song_info), 1);
  for (*i = 0; *i < BUFFER_SIZE && ftell(file) != bytsiz; (*i)++) {
    fread(tmp->path, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->artist, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->title, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->genre, BUFFER_SIZE, sizeof(char), file);
    fread(&(tmp->seconds), sizeof(float), 1, file);
    playlist[*i] = *tmp;
  }

  free(tmp);

}

void sort(char *buffer, struct song_info *playlist, FILE *file, int *i) {

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

void sort_write() {

  signal(SIGINT, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer);

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb");
  if (file == NULL) {
    printf("Error opening playlist: %sn", strerror(errno));
  }

  struct song_info playlist[BUFFER_SIZE]; // consider adding a function to read into a struct
  int i;
  f2p(playlist, file, &i);

  sort(buffer, playlist, file, &i);

  fseek(file, 0, SEEK_SET);

  p2f(playlist, file, &i);

  fclose(file);
  free(buffer);

}

void play_sorted() { // add sort and save too

  signal(SIGINT, &sighandler);

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  get_pname(buffer);

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb");
  if (file == NULL) {
    printf("Error opening playlist: %sn", strerror(errno));
  }

  struct song_info playlist[BUFFER_SIZE]; // consider adding a function to read into a struct
  int i;
  f2p(playlist, file, &i);

  sort(buffer, playlist, file, &i);

  int n;
  for (n = 0; n < i; n++) {
    printf("Playing %s by %s\n", playlist[n].title, playlist[n].artist);
    play_wav(playlist[n].path);
  }

  fclose(file);
  free(buffer);

}

int create_sema(){

  signal(SIGINT, &sighandler);

  // create semaphore
  int v, r;
  int semd = semget(QUEUEKEY, 1, IPC_CREAT | IPC_EXCL | 0644);
  if (semd == -1) {
    // printf("error %d: %s\n", errno, strerror(errno));
    semd = semget(QUEUEKEY, 1, 0);
    v = semctl(semd, 0, GETVAL, 0);
    // printf("semctl returned: %d\n", v);
  }
  else {
    union semun us;
    us.val = 1;
    r = semctl(semd, 0, SETVAL, us);
    // printf("semctl returned: %d\n", r);
  }
  return semd;
}

int add_song_to_queue(const char * path){

  signal(SIGINT, &sighandler);

  char cppath[BUFFER_SIZE];
  strcpy(cppath, path);
  struct song_info *tmp = calloc(1, sizeof(struct song_info));
  get_song_info(tmp, cppath);

  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  int queue = open("queue", O_WRONLY | O_APPEND | O_CREAT, 0664);
  write(queue, tmp->path, BUFFER_SIZE);
  write(queue, tmp->artist, BUFFER_SIZE);
  write(queue, tmp->title, BUFFER_SIZE);
  write(queue, tmp->genre, BUFFER_SIZE);
  write(queue, &(tmp->seconds), sizeof(tmp->seconds));
  printf("Added %s to queue\n", tmp->title);
  sb.sem_op = 1; //set the operation to up

  free(tmp);

  return 0;
}

int add_playlist_to_queue(const char * name){

  signal(SIGINT, &sighandler);

  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
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
  sb.sem_op = 1; //set the operation to up

  close(playlist);

  return 0;
}

int clear_queue(){

  signal(SIGINT, &sighandler);

  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  FILE * hold = fopen("queue", "w");
  fclose(hold);
  printf("Cleared Queue\n");
  sb.sem_op = 1; //set the operation to up
  return 0;
}
