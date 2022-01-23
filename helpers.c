#include "includes.h"

/* static void sighandler(int sig) {

  if (sig == SIGINT) {

    int shmdID = shmget(IDKey, sizeof(SDL_AudioDeviceID), 0640);
    SDL_AudioDeviceID *deviceID;
    deviceID = shmat(shmdID, 0, 0);

    SDL_PauseAudioDevice(*deviceID, 1);

    printf("Paused audio device (hopefully without killing a speaker, sigh)\n");

    shmdt(deviceID);

    exit(0);


  }

  if (sig == SIGINT) {

    printf("Caught!\n");

    int shmdBuf = shmget(bufKey, sizeof(Uint8 *), 0640);
    Uint8 *wavBuffer;
    wavBuffer = shmat(shmdBuf, 0, 0);

    int shmdID = shmget(IDKey, sizeof(SDL_AudioDeviceID), 0640);
    SDL_AudioDeviceID *deviceID;
    deviceID = shmat(shmdID, 0, 0);

    printf("Made it past initializations\n");

    SDL_CloseAudioDevice(*deviceID);
    printf("Issue is not with deviceID\n");
    //SDL_FreeWAV(wavBuffer);
    printf("Issue is not with wavBuffer\n");
    SDL_Quit();

    printf("made it past closing audio\n");

    //sleep(3);

    shmdt(wavBuffer);
    
    printf("hopefully we have closed audio!\n");

    int *seconds;
    int shmdSec = shmget(secKey, sizeof(int), IPC_CREAT | 0640);
    seconds = shmat(shmdSec, 0, 0);

    printf("Milliseconds played: %d\n", *seconds);

    shmdt(seconds);

    printf("Error: %s\n", strerror(errno));

    exit(0);

  }

} */

// worry about parsing later
int logic_controller(char *buffer) {

    //signal(SIGINT, sighandler);

    char **array = calloc(5, sizeof(char *));

    split(buffer, array);

    // printf("0: %s\t1: %s\n", array[0], array[1]);

    if (!strcmp(array[0], "-play")) {

      char *extension = strrchr(array[1], '.');

      // printf("ext: %s\n", extension+1);

      if (!strcmp(extension+1, "wav")) {
        play_wav(array[1]);
      }

    }

    return 0;

}

/* int launch() {

    shmget(secKey, sizeof(int), IPC_CREAT | 0640);
    shmget(specKey, sizeof(SDL_AudioSpec), IPC_CREAT | 0640);
    shmget(lenKey, sizeof(Uint32), IPC_CREAT | 0640);
    shmget(bufKey, sizeof(Uint8 *), IPC_CREAT | 0640);
    shmget(IDKey, sizeof(SDL_AudioDeviceID), IPC_CREAT | 0640);
    shmget(maxKey, sizeof(int), IPC_CREAT | 0640);

    return 0;
} */

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

/* int play_wav(char *song) {

    signal(SIGINT, sighandler);

    FILE *fp = fopen(song, "rb");

    struct WAV *wav_header = (struct WAV *) calloc(1, sizeof(struct WAV));

    fread(wav_header, 1, sizeof(struct WAV), fp);

    float duration_in_seconds = (float) wav_header->chunk_size / wav_header->byte_rate;
    float duration_in_milliseconds = 1000 * duration_in_seconds;

    SDL_Init(SDL_INIT_AUDIO);

    int shmdSec = shmget(secKey, sizeof(int), 0640);
    int shmdSpec = shmget(specKey, sizeof(SDL_AudioSpec), 0640);
    int shmdLen = shmget(lenKey, sizeof(Uint32), 0640);
    int shmdBuf = shmget(bufKey, sizeof(Uint8 *), 0640);
    int shmdID = shmget(IDKey, sizeof(SDL_AudioDeviceID), 0640);
    int shmdMax = shmget(maxKey, sizeof(int), 0640);

    int *seconds;
    SDL_AudioSpec *wavSpec;
    Uint32 *wavLength;
    Uint8 *wavBuffer;
    SDL_AudioDeviceID *deviceID;
    int *millimax;

    seconds = shmat(shmdSec, 0, 0);
    wavSpec = shmat(shmdSpec, 0, 0);
    wavLength = shmat(shmdLen, 0, 0);
    printf("Made it through three\n");
    wavBuffer = shmat(shmdBuf, 0, 0);
    printf("Buffer\n");
    deviceID = shmat(shmdID, 0, 0);
    millimax = shmat(shmdMax, 0, 0);

    *millimax = duration_in_milliseconds;
    

    SDL_LoadWAV(song, wavSpec, &wavBuffer, wavLength);

    *deviceID = SDL_OpenAudioDevice(NULL, 0, wavSpec, NULL, 0);

    printf("Audio Device ID: %d\n", *deviceID);
    printf("%s\n", SDL_GetError());

    int success = SDL_QueueAudio(*deviceID, wavBuffer, *wavLength);
    SDL_PauseAudioDevice(*deviceID, 0);

    for (*seconds = 0; *seconds < duration_in_milliseconds; (*seconds)++) {
      
      SDL_Delay(1);

    }
    //SDL_Delay(duration_in_milliseconds);

    SDL_CloseAudioDevice(*deviceID);
    SDL_FreeWAV(wavBuffer);
    SDL_Quit();
    
    shmdt(seconds); // MAKE THESE VARS GLOBAL SO YOU CAN ALWAYS ACCESS AND DELETE
    shmdt(wavSpec);
    shmdt(wavLength);
    shmdt(wavBuffer);
    shmdt(deviceID);

    return 0;
} */


/* int unpause_wav() {

  //signal(SIGINT, sighandler);

  int shmdID = shmget(IDKey, sizeof(SDL_AudioDeviceID), 0640);
  SDL_AudioDeviceID *deviceID;
  deviceID = shmat(shmdID, 0, 0);

  printf("deviceID obtained\n");

  int shmdMax = shmget(maxKey, sizeof(int), 0640);
  int *maxMilli;
  maxMilli = shmat(shmdMax, 0, 0);

  printf("maxMilli obtained: %d\n", *maxMilli);

  int shmdSec = shmget(secKey, sizeof(int), 0640);
  int *seconds;
  seconds = shmat(shmdSec, 0, 0);

  printf("seconds obtained: %d\n", *seconds);

  SDL_PauseAudioDevice(*deviceID, 0);

  printf("unpaused\n");

  for ( ; (*seconds) < (*maxMilli); (*seconds)++) {

    SDL_Delay(1);

  }

  printf("done playing\n");

  int shmdBuf = shmget(bufKey, sizeof(Uint8 *), 0640);
  Uint8 *wavBuffer;
  wavBuffer = shmat(shmdBuf, 0, 0);

  SDL_CloseAudioDevice(*deviceID);
  SDL_FreeWAV(wavBuffer);

  shmdt(seconds);
  shmdt(maxMilli);
  shmdt(wavBuffer);
  shmdt(deviceID);

  return 0;

} */

/* int shut_down() {


    int shmdSec = shmget(secKey, sizeof(int), 0640);
    int shmdSpec = shmget(specKey, sizeof(SDL_AudioSpec), 0640);
    int shmdLen = shmget(lenKey, sizeof(Uint32), 0640);
    int shmdBuf = shmget(bufKey, sizeof(Uint8 *), 0640);
    int shmdID = shmget(IDKey, sizeof(SDL_AudioDeviceID), 0640);

    shmctl(shmdSec, IPC_RMID, 0);
    shmctl(shmdSpec, IPC_RMID, 0);
    shmctl(shmdLen, IPC_RMID, 0);
    shmctl(shmdBuf, IPC_RMID, 0);
    shmctl(shmdID, IPC_RMID, 0);

    return 0;

} */

int initialize(int argc, char **argv) {
    printf("Starting initialization\n");
    int fd = open("data.txt", O_CREAT | O_RDWR | O_APPEND);
    char *path = calloc(BUFFER_SIZE, sizeof(char));
    char *tmp = calloc(BUFFER_SIZE, sizeof(char));
    getcwd(tmp, BUFFER_SIZE);
    printf("%s\n", tmp);
    strcpy(path, tmp);
    // write(fd, path, strlen(path));
    printf("%s\n", path);
    struct song_info *song_data[BUFFER_SIZE];
    find_files(song_data, path);
    printf("Exited find_files()\n");
    printf("%s\n", song_data[0]->title);
    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {

      if (song_data[i]->seconds == 0) 
        break;
      else 

    }
    write(fd, song_data, BUFFER_SIZE * sizeof(song_data));
    struct song_info *cpy[BUFFER_SIZE];
    read(fd, cpy, BUFFER_SIZE * sizeof(song_data));
    printf("%s\n", cpy[0]->title);
    return 0;
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

int make_playlist() { // in progress

  printf("Please input playlist nane: \n");
  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  fread(buffer, sizeof(char), BUFFER_SIZE, stdin);
  int fd = open(buffer, O_CREAT | O_EXCL | O_WRONLY | O_APPEND);

  struct songs *front = NULL;


}