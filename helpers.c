#include "headers.h"

// worry about parsing later
int logic_controller(char *buffer) {

    char **array = calloc(5, sizeof(char *));

    split(buffer, array);

    if (!strcmp(array[0], "-play")) {

        const char *extension = strrchr(array[1], ".");



    }

    return 0;

}

int play_wav(char *song) {

    FILE *fp = fopen(song, "rb");

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    fread(wav_header, 1, sizeof(struct WAV), fp);

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds;

    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8 *wavBuffer;

    SDL_LoadWAV(song, &wavSpec, &wavBuffer, &wavLength);

    SDL_AudioDeviceID deviceID = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    printf("Audio Device ID: %d\n", deviceID);
    printf("%s\n", SDL_GetError());

    int success = SDL_QueueAudio(deviceID, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceID, 0);

    SDL_Delay(duration_in_milliseconds);

    SDL_CloseAudioDevice(deviceID);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();

    return 0;
}


int initialize(int argc, char **argv) {

    int fd = open("data.txt", O_CREAT | O_EXCL | O_WRONLY | O_APPEND);
    char *path = calloc(BUFFER_SIZE, sizeof(char));
    strcpy(path, "AUDPATH=");
    if (argc == 4 && !strcmp(argv[2], "-path")) {
        strcat(path, argv[3]);
        strcat(path, "\n");
        write(fd, path, strlen(path));
    }
    else {
        char *tmp = calloc(BUFFER_SIZE, sizeof(char));
        getcwd(tmp, BUFFER_SIZE);
        strcat(path, tmp);
        strcat(path, "\n");
        write(fd, path, strlen(path));
    }

    return 0;

}

char ** find_files(char * path){
  char ** file_names = calloc(BUFFER_SIZE,BUFFER_SIZE);

  DIR * d = opendir(path);
  struct dirent *entry;

  int i = 0;
  while ((entry = readdir(d))){
    if (entry->d_type == DT_REG){
      char * ext = strrchr(entry->d_name,'.');
      if (!strcmp(ext,".wav")){
        strcpy(file_names[i],entry->d_name);
      }
    }
    i++;
  }
  return file_names;
}
