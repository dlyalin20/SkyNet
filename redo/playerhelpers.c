#include "includes.h"

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
        printf("err triggered\n");
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
          fgets(buffer, sizeof(buffer), stdin);
        }
        printf("%s\n", buffer);
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

    //SDL_EnableUNICODE(1);

    /* int err = 0;
    SDL_Event event;
    while (!err) {
      printf("here\n");
       err = SDL_WaitEventTimeout(&event, duration_in_milliseconds);
       if (err) {
          printf("event!\n");
         if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE) {
           printf("space!\n");
           if (SDL_GetAudioDeviceStatus(deviceID) == SDL_AUDIO_PLAYING) {
             printf("unpausing\n");
             SDL_PauseAudioDevice(deviceID, 0);
           }
           else {
             printf("pausing!\n");
             SDL_PauseAudioDevice(deviceID, 1);
           }

       }
       err = 0;
      }
      else {
        break;
      }
    } */

    //SDL_Delay(duration_in_milliseconds);

    SDL_CloseAudioDevice(deviceID);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();

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
