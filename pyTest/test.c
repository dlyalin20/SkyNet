#include "includes.h"

float get_duration(char *filename) {

    FILE *fp = fopen(filename, "rb");

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    fread(wav_header, 1, sizeof(struct WAV), fp);

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds;

}

int main() {

    char *cmd = "exiftool";
    char *PATH = "ex1.wav";
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

    char *buffer = calloc(stats.st_size, sizeof(char));

    FILE *fd = fopen(file_name, "r");

    fread(buffer, 1, 1, fd);

    fread(buffer, 1, stats.st_size-3, fd);

    // printf("%s\n", buffer);

    // printf("here\n");

    cJSON *song_json = cJSON_Parse(buffer);

    cJSON *cTitle = cJSON_GetObjectItemCaseSensitive(song_json, "ID3:ID3v2_3:Title");

    /* if (cJSON_IsString(title) && (title->valuestring != NULL)) {

        printf("Song Title: %s\n", title->valuestring);

    } */

    cJSON *cArtist = cJSON_GetObjectItemCaseSensitive(song_json, "ID3:ID3v2_3:Artist");

    /* if (cJSON_IsString(artist) && (artist->valuestring != NULL)) {

        printf("Song Artist: %s\n", artist->valuestring);

    } */

    cJSON *cGenre = cJSON_GetObjectItemCaseSensitive(song_json, "ID3:ID3v1:Genre");

    struct song_info *info = calloc(1, sizeof(struct song_info));

    strcpy(info->artist, cArtist->valuestring);
    strcpy(info->title, cTitle->valuestring);
    strcpy(info->genre, cGenre->valuestring);
    info->seconds = get_duration(PATH);

    //printf("%s by %s\n", song_info[1], song_info[0]);

    remove(file_name);

    // int fd = open("song_data.json", O_CREAT | O_WRONLY);

    // int backup_stdout = dup( STDOUT_FILENO );

    // dup2(fd, STDOUT_FILENO);

    // system("ls > song_data.txt");

    return info;

}

// find .  -maxdepth 1 -type p -delete