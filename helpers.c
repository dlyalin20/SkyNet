#include "headers.h"

int play_song(char *song) {

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