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

// removing songs froms playlists; add going back in playlists

// remember to allow for modifying playlists and reorganizing playlists 