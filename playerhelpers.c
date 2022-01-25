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

    if (!deviceID) {
      printf("Error Opening Audio Device: %s\n", SDL_GetError());
      exit(-1);
    }
    int success = SDL_QueueAudio(deviceID, wavBuffer, wavLength);
    if (success < 0)  {
      printf("Error Queueing Audio: %s\n", SDL_GetError());
      exit(-1);
    }
    SDL_PauseAudioDevice(deviceID, 0);

    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    FD_ZERO(&read_fds);

    FD_SET(STDIN_FILENO, &read_fds);

    struct timeval *timer = calloc(1, sizeof(struct timeval));

    timer->tv_sec = duration_in_seconds;

    printf("Type a space to pause or unpause; type 'exit' to quit\n");

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
            printf("Type a space to pause or unpause; type 'exit' to quit\n");

        }
        if (!strcmp("exit\n", buffer)) {
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
    free(wav_header);
    fclose(fp);

    return 0;
}

void play_queue(){
  struct stat stats;
  int i;
  char path[BUFFER_SIZE], artist[BUFFER_SIZE], title[BUFFER_SIZE], genre[BUFFER_SIZE];
  float seconds;
  FILE *file;
  while (1) {
    file = fopen("queue", "r");
    fseek(file, 0, SEEK_SET);
    if (file == NULL) {
      printf("Error Opening Queue: %s\n", strerror(errno));
    }
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_flg = SEM_UNDO;
    sb.sem_op = -1; //setting the operation to down

    int freadsize = fread(&path, sizeof(char), BUFFER_SIZE, file);
    if (freadsize < BUFFER_SIZE){
      sb.sem_op = 1; //set the operation to up
      printf("Queue Empty Awaiting More Songs\n");
      sleep(1);
      continue;
    }
    fread(&artist, sizeof(char),BUFFER_SIZE , file);
    fread(&title, sizeof(char), BUFFER_SIZE, file);
    fread(&genre, sizeof(char), BUFFER_SIZE, file);
    fread(&seconds, sizeof(float), 1, file);
    int size_of_song = ftell(file);
    // calculating the size of the rest of the file
    fseek(file, 0, SEEK_END);
    int filesize = ftell(file);
    int rest_of_file_size = filesize - size_of_song;
    // read the queue minus the first song
    fseek(file, size_of_song, SEEK_SET);
    void * x = malloc(rest_of_file_size);
    fread(x, sizeof(char), rest_of_file_size, file);
    // overwrite the file
    fclose(file);
    file = fopen("queue", "w+");
    fseek(file, 0, SEEK_SET);
    fwrite(x, sizeof(char), rest_of_file_size, file);
    fclose(file);
    free(x);
    sb.sem_op = 1; //set the operation to up

    struct song_info *tmp = calloc(1, sizeof(struct song_info));
    strcpy(tmp->path, path);
    strcpy(tmp->artist, artist);
    strcpy(tmp->title, title);
    strcpy(tmp->genre, genre);
    tmp->seconds = seconds;
    printf("Playing %s by %s\n", tmp->title, tmp->artist);
    printf("Duration: %f seconds\n", tmp->seconds);
    printf("PATH: %s\n", tmp->path);
    play_wav(tmp->path);
    free(tmp);
  }
}

int play_playlist(const char *playlist_name) {
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
