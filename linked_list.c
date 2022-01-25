#include "includes.h"

// TO-DO: ORGANIZATIONAL: SEARCHNIG BY TITLE, ARTIST

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

    cJSON *cTitle = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Title");

    /* if (cJSON_IsString(cTitle) && (cTitle->valuestring != NULL)) {

        printf("Song Title: %s\n", cTitle->valuestring);
        strcpy(info->title, cTitle->valuestring);

    } */

    cJSON *cArtist = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Artist");

    /* if (cJSON_IsString(cArtist) && (cArtist->valuestring != NULL)) {

        printf("Song Artist: %s\n", cArtist->valuestring);

    } */

    cJSON *cGenre = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Genre");

    /* if (cJSON_IsString(cGenre) && (cGenre->valuestring != NULL)) {

      printf("Song Genre: %s\n", cGenre->valuestring);

    } */
    strcpy(info->artist, cArtist->valuestring);
    strcpy(info->path, PATH);
    strcpy(info->title, cTitle->valuestring);
    strcpy(info->genre, cGenre->valuestring);
    info->seconds = get_duration(PATH);


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

void print_info(struct song_info *song) {

  printf("Playing %s by %s\n", song->title, song->artist);

}

int make_playlist() { // in progress

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

int play_playlist(char *playlist_name) {

  struct stat stats;

  FILE *file = fopen(playlist_name, "rb+");

  if (file == NULL) {
    printf("Error Opening Playlist: %s\n", strerror(errno));
  }

  fseek(file, 0, SEEK_END);
  int byte_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  //printf("Past here\n");

  int i;
  char path[BUFFER_SIZE], artist[BUFFER_SIZE], title[BUFFER_SIZE], genre[BUFFER_SIZE];
  float seconds;
  for (i = 0; i < BUFFER_SIZE; i++) {

    if (ftell(file) == byte_size)
      break;
    fread(&path, BUFFER_SIZE, sizeof(char), file);
    fread(&artist, BUFFER_SIZE, sizeof(char), file);
    fread(&title, BUFFER_SIZE, sizeof(char), file);
    fread(&genre, BUFFER_SIZE, sizeof(char), file);
    fread(&seconds, 1, sizeof(float), file);

    struct song_info *tmp = calloc(1, sizeof(struct song_info));
    strcpy(tmp->path, path);
    strcpy(tmp->artist, artist);
    strcpy(tmp->title, title);
    strcpy(tmp->genre, genre);
    tmp->seconds = seconds;

    printf("Playing %s by %s\n", tmp->title, tmp->artist);
    play_wav(tmp->path);
    free(tmp);

  }

  fclose(file);

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

void sort_write() {

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  printf("Which playlist would you like to sort and save, or exit: \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);

  char *rm = strrchr(buffer, '\n');
  rm[0] = '\0';

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb");
  if (file == NULL) {
    printf("Error opening playlist: %sn", strerror(errno));
  }

  fseek(file, 0, SEEK_END);
  int bytsiz = ftell(file);
  fseek(file, 0, SEEK_SET);

  struct song_info playlist[BUFFER_SIZE]; // consider adding a function to read into a struct
  int i;
  struct song_info *tmp = calloc(sizeof(struct song_info), 1);
  for (i = 0; i < BUFFER_SIZE && ftell(file) != bytsiz; i++) {
    fread(tmp->path, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->artist, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->title, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->genre, BUFFER_SIZE, sizeof(char), file);
    fread(&(tmp->seconds), sizeof(float), 1, file);
    playlist[i] = *tmp;
  }

  free(tmp);

  printf("Sort by 'title', 'artist', 'genre', or 'duration', or 'exit': \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  char *rm2 = strrchr(buffer, '\n');
  rm2[0] = '\0';

  if (!strcmp(buffer,"exit")) {
    fclose(file);
    free(buffer);
  }
  if (!strcmp(buffer, "title")) {
    sort_title(playlist, i);
  }
  else if (!strcmp(buffer, "artist")) {
    sort_artist(playlist, i);
  }
  else if (!strcmp(buffer, "genre")) {
    sort_genre(playlist, i);
  }
  else if (!strcmp(buffer, "duration")) {
    sort_duration(playlist, i);
  }
  else {
    printf("No such sorting type!\n");
    fclose(file);
    free(buffer);
    exit(-1);
  }

  fseek(file, 0, SEEK_SET);

  int n;
  for (n = 0; n < i; n++) {
    fwrite(playlist[i].path, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[i].artist, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[i].title, BUFFER_SIZE, sizeof(char), file);
    fwrite(playlist[i].genre, BUFFER_SIZE, sizeof(char), file);
    fwrite(&(playlist[i].seconds), BUFFER_SIZE, sizeof(char), file);
  }

  fclose(file);
  free(buffer);

}

void play_sorted() { // add sort and save too

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  printf("Which playlist would you like to sort and play, or exit: \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);

  char *rm = strrchr(buffer, '\n');
  rm[0] = '\0';

  if (!strcmp(buffer, "exit")) {
    free(buffer);
    exit(0);
  }

  FILE *file = fopen(buffer, "rb");
  if (file == NULL) {
    printf("Error opening playlist: %sn", strerror(errno));
  }

  fseek(file, 0, SEEK_END);
  int bytsiz = ftell(file);
  fseek(file, 0, SEEK_SET);

  struct song_info playlist[BUFFER_SIZE]; // consider adding a function to read into a struct
  int i;
  struct song_info *tmp = calloc(sizeof(struct song_info), 1);
  for (i = 0; i < BUFFER_SIZE && ftell(file) != bytsiz; i++) {
    fread(tmp->path, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->artist, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->title, BUFFER_SIZE, sizeof(char), file);
    fread(tmp->genre, BUFFER_SIZE, sizeof(char), file);
    fread(&(tmp->seconds), sizeof(float), 1, file);
    playlist[i] = *tmp;
  }

  free(tmp);

  printf("Sort by 'title', 'artist', 'genre', or 'duration': \n");
  read(STDIN_FILENO, buffer, BUFFER_SIZE);
  char *rm2 = strrchr(buffer, '\n');
  rm2[0] = '\0';

  if (!strcmp(buffer, "title")) {
    sort_title(playlist, i);
  }
  else if (!strcmp(buffer, "artist")) {
    sort_artist(playlist, i);
  }
  else if (!strcmp(buffer, "genre")) {
    sort_genre(playlist, i);
  }
  else if (!strcmp(buffer, "duration")) {
    sort_duration(playlist, i);
  }
  else {
    printf("No such sorting type!\n");
    fclose(file);
    free(buffer);
    exit(-1);
  }

  int n;
  for (n = 0; n < i; n++) {
    printf("Playing %s by %s\n", playlist[n].title, playlist[n].artist);
    play_wav(playlist[n].path);
  }

  fclose(file);
  free(buffer);

}

void sort_title(struct song_info *arr, int size) {

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

void remove_from_playlist() {

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));

  while (1) {

    printf("Enter playlist to remove songs from, or 'exit': \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *rm = strrchr(buffer, '\n');
    rm[0] = '\0';

    if (!strcmp(buffer, "exit")) {
      break;
    }
    FILE *file = fopen(buffer, "rb");

    char *filename = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(filename, buffer);

    printf("Enter song title to remove: \n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *rm2 = strrchr(buffer, '\n');
    rm2[0] = '\0';

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

    for (i = 0; i < n; i++) {
      fwrite(playlist[i].path, BUFFER_SIZE, sizeof(char), file);
      fwrite(playlist[i].artist, BUFFER_SIZE, sizeof(char), file);
      fwrite(playlist[i].title, BUFFER_SIZE, sizeof(char), file);
      fwrite(playlist[i].genre, BUFFER_SIZE, sizeof(char), file);
      fwrite(&(playlist[i].seconds), sizeof(float), sizeof(char), file);
    }

    fclose(file);
    free(filename);

    printf("Would you like to modify another playlist, 'y' or 'n'?\n");
    read(STDIN_FILENO, buffer, BUFFER_SIZE);
    char *rm3 = strrchr(buffer, '\n');
    rm3[0] = '\0';
    if (!strcmp(buffer, "y"))
      continue;
    else
      break;
  }

  free(buffer);

}

void sort_duration(struct song_info **arr, int size){
  int i, j;
  int n = size;
  float key = 0;
    for (i = 1; i < n; i++) {
        key = arr[i]->seconds;
        j = i - 1;
        while (j >= 0 && arr[j]->seconds > key) {
            struct song_info *temp = arr[j+1];
            arr[j + 1] = arr[j];
            arr[j] = temp;
            j--;
        }
        arr[j+1]->seconds = key;
    }
}

// removing songs froms playlists; add going back in playlists
// change so that command line inputs are taken instead of prompts

