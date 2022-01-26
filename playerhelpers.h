#ifndef playerhelpers_h
#define playerhelpers_h "defined"

// functions
void play_queue(); // takes null; plays all songs in queue; returns null
void play_sorted(); // takes null; plays sorted playlist; returns null
int play_wav(char *song); // takes song name; plays song; returns int
static void sighandler(int sigint); // takes int; handles signals, kills jobs; returns null
int play_playlist(const char *playlist_name); // takes playlist name; plays all songs in playlist; returns int

#endif
