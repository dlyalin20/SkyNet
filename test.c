#include "includes.h"


int main() {

    char *cmd = "exiftool";
    char *PATH = "jude.mp3";
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
    
    char *file_name = "jude.json";

    stat(file_name, &stats);

    char *buffer = calloc(stats.st_size, sizeof(char));

    FILE *fd = fopen(file_name, "r");

    fread(buffer, 1, 1, fd);

    fread(buffer, 1, stats.st_size-3, fd);

    // printf("%s\n", buffer);

    // printf("here\n");

    cJSON *song_json = cJSON_Parse(buffer);

    if (song_json == NULL) {

        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {

            printf("Error before: %s\n", error_ptr);

        }

    }

    cJSON *title = cJSON_GetObjectItemCaseSensitive(song_json, "ID3:ID3v2_3:Title");

    if (cJSON_IsString(title) && (title->valuestring != NULL)) {

        printf("Song Title: %s\n", title->valuestring);

    }

    cJSON *artist = cJSON_GetObjectItemCaseSensitive(song_json, "ID3:ID3v2_3:Artist");

    if (cJSON_IsString(artist) && (artist->valuestring != NULL)) {

        printf("Song Artist: %s\n", artist->valuestring);

    }

    // int fd = open("song_data.json", O_CREAT | O_WRONLY);

    // int backup_stdout = dup( STDOUT_FILENO );

    // dup2(fd, STDOUT_FILENO);

    // system("ls > song_data.txt");

}

// find .  -maxdepth 1 -type p -delete