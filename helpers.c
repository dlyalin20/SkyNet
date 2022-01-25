#include "includes.h"

// worry about parsing later
int logic_controller(char *buffer) {


    char **array = calloc(5, sizeof(char *));

    split(buffer, array);

    if (!strcmp(array[0], "-play")) {

      char *extension = strrchr(array[1], '.');

      if (!strcmp(extension+1, "wav")) {
        play_wav(array[1]);
      }

    }

    free(array);

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

    int success = SDL_QueueAudio(deviceID, wavBuffer, wavLength);
    if (success) {
      printf("Error Queueing Audio: %s\n", SDL_GetError());
    }
    SDL_PauseAudioDevice(deviceID, 0);

    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    FD_ZERO(&read_fds);

    FD_SET(STDIN_FILENO, &read_fds);

    struct timeval *timer = calloc(1, sizeof(struct timeval));

    timer->tv_sec = duration_in_seconds;

    printf("Type a space to pause or unpause; type e to quit\n");

    int err = 0;
    while (!err) {

      err = select(STDIN_FILENO+1, &read_fds, NULL, NULL, timer);
      if (err) { //select triggered
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
          fgets(buffer, sizeof(buffer), stdin);
        }
        if (!strcmp(buffer, " \n")) { // space pressed

          if (SDL_GetAudioDeviceStatus(deviceID) == SDL_AUDIO_PLAYING)
            SDL_PauseAudioDevice(deviceID, 1);
          else
            SDL_PauseAudioDevice(deviceID, 0);
            printf("Type a space to pause or unpause; type e to quit\n");

        }
        if (!strcmp("e\n", buffer)) {
          break;
        }
        err = 0;
      }
      else 
        err = 1;

    }

    SDL_CloseAudioDevice(deviceID);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();

    fclose(fp);
    free(timer);
    free(wav_header);

    return 0;
}

int initialize(int argc, char **argv) {
    printf("Starting initialization\n");
    FILE *file = fopen("data.txt", "wb+");
    char *path = calloc(BUFFER_SIZE, sizeof(char));
    getcwd(path, BUFFER_SIZE);
    struct song_info *song_data = calloc(sizeof(struct song_info), BUFFER_SIZE);
    find_files(song_data, path);
    int i;
    int num_full = 0;
    for (i = 0; i < BUFFER_SIZE; i++) {
      if (strcmp(song_data[i].path, "")) {
        fwrite(song_data[i].path, BUFFER_SIZE, 1, file);
        fwrite(song_data[i].artist, BUFFER_SIZE, 1, file);
        fwrite(song_data[i].title, BUFFER_SIZE, 1, file);
        fwrite(song_data[i].genre, BUFFER_SIZE, 1, file);
        fwrite(&(song_data[i].seconds), sizeof(song_data[i].seconds), 1, file);
        num_full++;
      }
      else
        break;
    }
    free(song_data);
    fclose(file);
    free(path);
    printf("Finished initializing\n");
    return 0;
      // NOTES FOR MYSELF BELOW
    /* fseek(file, 0, SEEK_SET);
    struct song_info cpy[BUFFER_SIZE];
    for (i = 0; i < num_full; i++) {
      char path[BUFFER_SIZE];
      fread(&path, BUFFER_SIZE, 1, file);
      printf("Path Read: %s\n", path);
      char artist[BUFFER_SIZE];
      fread(&artist, BUFFER_SIZE, 1, file);
      char title[BUFFER_SIZE];
      fread(&title, BUFFER_SIZE, 1, file);
      char genre[BUFFER_SIZE];
      fread(&genre, BUFFER_SIZE, 1, file);
      float seconds = 0;
      fread(&seconds, sizeof(float), 1, file);
      struct song_info *tmp = calloc(sizeof(struct song_info), 1);
      strcpy(tmp->path, path);
      strcpy(tmp->artist, artist);
      strcpy(tmp->title, title);
      strcpy(tmp->genre, genre);
      tmp->seconds = seconds;
      cpy[i] = *tmp;
    }
    printf("S1: %f\n", cpy[0].seconds);
    printf("T1: %s\n", cpy[0].title);
    printf("S3: %f\n", cpy[2].seconds);
    printf("G3: %s\n", cpy[2].genre);
    if (err)
      printf("Error Reading: %s\n", strerror(errno));
    else
      printf("Finished reading\n"); */
}

void shuffle(char **array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
          size_t j = i + rand() % n;
          char * t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}