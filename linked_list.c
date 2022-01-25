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

void print_song(struct songs *song) {

    printf("Song Title: %s\n", song->name);

}

void print_playlist(struct songs *HEAD) {
    struct songs *tmp = HEAD;
    while (tmp != NULL) {

        print_song(tmp);
        tmp = tmp->next;

    }
}

struct songs * create_song(char *title) {

    struct songs *song = calloc(1, sizeof(struct songs));
    strcpy(song->name, title);
    song->next = NULL;


    free(song);
    return song;

}

struct songs * insert_song_end(struct songs *to_add, struct songs *HEAD) {

    struct songs *tmp = HEAD;

    while (tmp->next != NULL) {
        tmp = tmp->next;
    }

    tmp->next = to_add;

    return HEAD;

}

struct songs * remove_song(char *title, struct songs *HEAD) {

    if (!strcmp(HEAD->name, title)) {
        return HEAD->next;
    }

    struct songs *tmp = HEAD;

    while (tmp->next) {

        if (!strcmp(tmp->next->name, title)) {
            tmp->next = tmp->next->next;
            return HEAD;
        }
        else
            tmp = tmp->next;

    }

    return HEAD;

}

int delete_playlist(char *playlist_name) {

    return remove(playlist_name);

}
