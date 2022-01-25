#include "includes.h"
#include "adderhelpers.h"
#include "playerhelpers.h"
/*
Holds functions that relate to song storage
*/

float get_duration(char *filename) {

    FILE *fp = fopen(filename, "rb");

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    fread(wav_header, 1, sizeof(struct WAV), fp);

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds;

    fclose(fp);
    free(wav_header);

    return duration_in_milliseconds;

}

struct song_info * get_song_info(struct song_info *info, char *PATH) {
    char *cmd = "exiftool";
    char *args[] = {cmd, "-charset", "filename=utf8", "-charset", "exif=utf8", "-charset", "iptc=utf8", "-use", "mwg", "-m", "-G0:1", "-wm", "wcg", "-a", "-j", "-struct", "-w!", "\%d\%f.json", "-All", PATH, NULL};
    int pid = fork();
    if (!pid) { // child process
        execvp(cmd, args);
        exit(0);
    }
    else {
        wait(NULL);
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
    fread(buffer, 1, 1, fd);
    fread(buffer, 1, stats.st_size-3, fd);
    cJSON *song_json = cJSON_Parse(buffer);

    // jason workaround ----
    cJSON *cTitle = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Title");
    char unknownstring[] = "Unknown";
    if (cJSON_IsString(cTitle) && (cTitle->valuestring != NULL)) {
        printf("Song Title: %s\n", cTitle->valuestring);
        strcpy(info->title, cTitle->valuestring);
    }else{
      strcpy(info->title, unknownstring);
      printf("Song Title: %s\n", info->title);

    }
    strcpy(info->path, PATH);
    cJSON *cArtist = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Artist");
    if (cJSON_IsString(cArtist) && (cArtist->valuestring != NULL)) {
        printf("Song Artist: %s\n", cArtist->valuestring);
    }else{
      strcpy(info->artist, unknownstring);
      printf("Song Artist: %s\n", info->artist);
    }

    cJSON *cGenre = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Genre");
    if (cJSON_IsString(cGenre) && (cGenre->valuestring != NULL)) {
      printf("Song Genre: %s\n", cGenre->valuestring);
    } else{
      strcpy(info->genre, unknownstring);
      printf("Song Genre: %s\n", info->genre);
    }
    info->seconds = get_duration(PATH);

    // first verson --------------
   //  cJSON *cTitle = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Title");
   // cJSON *cArtist = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Artist");
   // cJSON *cGenre = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Genre");
   //  strcpy(info->artist, cArtist->valuestring);
   // strcpy(info->path, PATH);
   // strcpy(info->title, cTitle->valuestring);
   // strcpy(info->genre, cGenre->valuestring);
   // info->seconds = get_duration(PATH);


    remove(pathcopy);
    free(pathcopy);
    free(buffer);

    return info;

}

struct song_info * find_files(struct song_info *song_data, char * path) {
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

    return remove(playlist_name);

}

int make_playlist_simple() { // test

  printf("Please input playlist name: \n");
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  char *rem;
  rem = strrchr(buffer, '\n');
  rem[0] = '\0';
  FILE *file = fopen(buffer, "wb+");

  if (file == NULL) {
    printf("Error: %s\n", strerror(errno));
    exit(-1);
  }

  printf("Please input a song to add, or type exit: \n");
  struct stat stats;
  while (1) {
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *end;
    end = strrchr(buffer, '\n');
    end[0] = '\0';
    char * ext = strrchr(buffer,'.');
    if (!strcmp(buffer, "exit"))
      break;
    if (!stat(buffer, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
      struct song_info *tmp = calloc(1, sizeof(struct song_info));
      get_song_info(tmp, buffer);
      fwrite(tmp->path, BUFFER_SIZE, 1, file);
      fwrite(tmp->artist, BUFFER_SIZE, 1, file);
      fwrite(tmp->title, BUFFER_SIZE, 1, file);
      fwrite(tmp->genre, BUFFER_SIZE, 1, file);
      fwrite(&(tmp->seconds), sizeof(tmp->seconds), 1, file);
    }
    else {
      printf("%s doesn't exist\n", buffer);
    }
    printf("Please input a song to add, or type exit: \n");
  }

  free(buffer);
  fclose(file);

  printf("Playlist Created\n");

  return 0;

}

void print_info(struct song_info *song) {

  printf("Playing %s by %s\n", song->title, song->artist);

}

int make_playlist(const char * path) {
  // copy target path
  char *filelocation = calloc(BUFFER_SIZE, sizeof(char));
  strcpy(filelocation, path);
  printf("Please input playlist name: \n");
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  char *rem;
  rem = strrchr(buffer, '\n');
  rem[0] = '\0';
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
  printf("Please input a song to add, or type exit: \n");
  struct stat stats;
  while (1) {
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *end;
    end = strrchr(buffer, '\n');
    end[0] = '\0';
    strcpy(filelocation, path);
    strcat(filelocation, "/");
    strcat(filelocation, buffer);
    printf("%s\n", filelocation);
    // filelocation is now [path]/song'/0'
    char * ext = strrchr(filelocation,'.');
    if (!strcmp(buffer, "exit"))
      break;
    if (!stat(filelocation, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
      struct song_info *tmp = calloc(1, sizeof(struct song_info));
      get_song_info(tmp, filelocation);
      printf("%s\n", "made it");
      fwrite(tmp->path, BUFFER_SIZE, 1, file);
      fwrite(tmp->artist, BUFFER_SIZE, 1, file);
      fwrite(tmp->title, BUFFER_SIZE, 1, file);
      fwrite(tmp->genre, BUFFER_SIZE, 1, file);
      fwrite(&(tmp->seconds), sizeof(tmp->seconds), 1, file);
    }
    else {
      printf("%s doesn't exist\n", filelocation);
    }
    printf("Please input a song to add, or type exit: \n");
  }
  free(buffer);
  free(filelocation);
  fclose(file);
  // save location of playlist in current directory
  // FILE *holds_playlist_path = fopen(playlist_name, "wb+");
  // fwrite(path, strlen(path),1, holds_playlist_path);
  printf("Playlist Created\n");
  return 0;
}

int add_to_playlist() {
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  while (1) {
    printf("Type playlist name to add to or exit to quit: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *end;
    end = strrchr(buffer, '\n');
    end[0] = '\0';
    if (!strcmp(buffer, "exit")) {
      break;
    }
    FILE *file = fopen(buffer, "wb+");
    if (file == NULL) {
      printf("Error opening playlist: %s\n", strerror(errno));
      continue;
    }
    fseek(file, 0, SEEK_END);
    printf("Type song name to add to playlist or exit to quit: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *rm;
    rm = strrchr(buffer, '\n');
    rm[0] = '\0';
    struct stat stats;
    char *ext = strrchr(buffer, '.');
    if (!stat(buffer, &stats) && ext != NULL && !strcmp(ext, ".wav")) {
      struct song_info *tmp = calloc(1, sizeof(struct song_info));
      get_song_info(tmp, buffer);
      fwrite(tmp->path, BUFFER_SIZE, 1, file);
      fwrite(tmp->artist, BUFFER_SIZE, 1, file);
      fwrite(tmp->title, BUFFER_SIZE, 1, file);
      fwrite(tmp->genre, BUFFER_SIZE, 1, file);
      fwrite(&(tmp->seconds), sizeof(tmp->seconds), 1, file);
      free(tmp);
    }
    else {
      printf("Error finding that song!\n");
      continue;
    }
    fclose(file);
  }
  free(buffer);
  return 0;

}

void sort_title(struct song_info **arr, int size){
  int i, j;
  int n = size;
  char *key = calloc(BUFFER_SIZE,1);
    for (i = 1; i < n; i++) {
        strcpy(key,arr[i]->title);
        j = i - 1;
        while (j >= 0 && strcmp(arr[j]->title, key)>0) {
            struct song_info *temp = arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = temp;
            j--;
        }
        strcpy(arr[j + 1]->title, key);
    }
}

void sort_artist(struct song_info **arr, int size){
  sort_title(arr,size);
  int i, j;
  int n = size;
  char *key = calloc(BUFFER_SIZE,1);
    for (i = 1; i < n; i++) {
        strcpy(key,arr[i]->artist);
        j = i - 1;
        while (j >= 0 && strcmp(arr[j]->artist, key)>0) {
            struct song_info *temp = arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = temp;
            j--;
        }
        strcpy(arr[j + 1]->artist, key);
    }
}

void sort_genre(struct song_info **arr, int size){
  sort_artist(arr,size);
  int i, j;
  int n = size;
  char *key = calloc(BUFFER_SIZE,1);
    for (i = 1; i < n; i++) {
        strcpy(key,arr[i]->genre);
        j = i - 1;
        while (j >= 0 && strcmp(arr[j]->genre, key)>0) {
            struct song_info *temp = arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = temp;
            j--;
        }
        strcpy(arr[j + 1]->genre, key);
    }
}

int create_sema(){
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
}
int add_playlist_to_queue(const char * name){
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
  printf("Read %d bytes\n", read(playlist, x, playlist_size));
  printf("Wrote %d bytes\n", write(queue, x, playlist_size));

  printf("Added playlist %s to queue\n", name);
  sb.sem_op = 1; //set the operation to up
}

int clear_queue(){
  int semd = create_sema();
  struct sembuf sb;
  sb.sem_num = 0;
  sb.sem_flg = SEM_UNDO;
  sb.sem_op = -1; //setting the operation to down
  FILE * hold = fopen("queue", "w");
  fclose(hold);
  printf("Cleared Queue\n");
  sb.sem_op = 1; //set the operation to up
}
