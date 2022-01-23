#include "includes.h"

// TO-DO: ORGANIZATIONAL: SEARCHNIG BY TITLE, ARTIST

float get_duration(char *filename) {

    FILE *fp = fopen(filename, "rb");

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    fread(wav_header, 1, sizeof(struct WAV), fp);

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds;

    return duration_in_milliseconds;

}

struct song_info * get_song_info(struct song_info *info, char *PATH) {

    printf("Began get_song_info for ex1.wav\n");
    printf("Info: %p\n", info);
    char *cmd = "exiftool";
    char *args[] = {cmd, "-charset", "filename=utf8", "-charset", "exif=utf8", "-charset", "iptc=utf8", "-use", "mwg", "-m", "-G0:1", "-wm", "wcg", "-a", "-j", "-struct", "-w!", "\%d\%f.json", "-All", PATH, NULL};

    int pid = fork();

    if (!pid) { // child process

        execvp(cmd, args);

    }

    else {

        wait(NULL);
        // dup2(backup_stdout, STDOUT_FILENO);

    }

    struct stat stats;
    
    char *file_name = "ex1.json";

    stat(file_name, &stats);

    printf("File Mode From Stat: %hu\n", stats.st_mode);

    char *buffer = calloc(stats.st_size, sizeof(char));

    FILE *fd = fopen(file_name, "r");

    fread(buffer, 1, 1, fd);

    fread(buffer, 1, stats.st_size-3, fd);

    //printf("Buffer: %s\n", buffer);

    // printf("here\n");

    cJSON *song_json = cJSON_Parse(buffer);

    cJSON *cTitle = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Title");

    if (cJSON_IsString(cTitle) && (cTitle->valuestring != NULL)) {

        printf("Song Title: %s\n", cTitle->valuestring);

    }

    cJSON *cArtist = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Artist");

    if (cJSON_IsString(cArtist) && (cArtist->valuestring != NULL)) {

        printf("Song Artist: %s\n", cArtist->valuestring);

    }

    cJSON *cGenre = cJSON_GetObjectItemCaseSensitive(song_json, "RIFF:Genre");

    if (cJSON_IsString(cGenre) && (cGenre->valuestring != NULL)) {

      printf("Song Genre: %s\n", cGenre->valuestring);

    }

    printf("PATH: %s\n", PATH);
    //printf("%s\n", info->path);
   //printf("%d\n", info->artist);
    strcpy(info->artist, cArtist->valuestring);
    printf("Added Artist: %s\n", info->artist);
    strcpy(info->path, PATH);
    printf("Added Path: %s\n", info->path);
    strcpy(info->title, cTitle->valuestring);
    printf("Added Title: %s\n", info->title);
    strcpy(info->genre, cGenre->valuestring);
    printf("Added Genre: %s\n", info->genre);
    info->seconds = get_duration(PATH);
    printf("Added Duration: %f\n", info->seconds);

    printf("Path: %s\nArtist: %s\nTitle: %s\nGenre: %s\nDuration: %f\n", info->path, info->artist, info->title, info->genre, info->seconds);

    //printf("%s by %s\n", song_info[1], song_info[0]);

    remove(file_name);

    // int fd = open("song_data.json", O_CREAT | O_WRONLY);

    // int backup_stdout = dup( STDOUT_FILENO );

    // dup2(fd, STDOUT_FILENO);

    // system("ls > song_data.txt");

    return info;

}

struct song_info ** find_files(struct song_info **song_data, char * path) {

  printf("Starting song search\n");
  DIR * d = opendir(path);
  struct dirent *entry;

  int i = 0;
  while ((entry = readdir(d))){
    if (entry->d_type == DT_REG){
      printf("%s\n", entry->d_name);
      char * ext = strrchr(entry->d_name,'.');
      printf("%s\n", ext);
      if (ext != NULL && !strcmp(ext,".wav")){
        struct song_info *tmp = calloc(1, sizeof(struct song_info));
        printf("Tmp: %p\n", tmp);
        get_song_info(tmp, entry->d_name);
        printf("Made it past get_song_info\n");
        printf("Path: %s\nArtist: %s\nTitle: %s\nGenre: %s\nDuration: %f\n", tmp->path, tmp->artist, tmp->title, tmp->genre, tmp->seconds);
        song_data[i] = tmp;
        printf("Path: %s\nArtist: %s\nTitle: %s\nGenre: %s\nDuration: %f\n", song_data[i]->path, song_data[i]->artist, song_data[i]->title, song_data[i]->genre, song_data[i]->seconds);
        i++;
      }
    }
  }
  printf("Exited finding while loop\n");
  printf("%s\n", song_data[0]->title);
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