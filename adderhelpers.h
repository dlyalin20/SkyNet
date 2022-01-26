#ifndef adderhelpers_h
#define adderhelpers_h "defined"

// custom function declaration
int clear_queue(); // takes null; clears the queue; returns int
int create_sema(); // takes null; creates semaphore; returns int
void sort_write(); // takes null; sorts playlist permanently; returns null
int make_playlist(); // takes null; makes a playlist from directory; returns int
int add_to_playlist(); // takes null; adds song to playlist; returns int
void perma_shuffle(); // takes null; permanently shuffles playlist; returns null
int make_playlist_simple(); // takes null; makes playlist by prompting for songs; returns int
void remove_from_playlist(); // takes null; removes song from playlist; returns null
void get_pname(char *buffer); // takes string buffer; prompts for playlist name; returns null
void get_sname(char *buffer); // takes string buffer; prompts for song name; returns null
float get_duration(char *filename); // takes file name; returns duration of song as float
void s2f(char *buffer, FILE *file); // takes char buffer and file stream; writes buffer contents to file; returns null
static void sighandler(int sigint); // takes int; handles signal by killing jobs; returns void
int make_playlist(const char * path); // takes file path; makes playlist from wavs in directory; returns int
void shuffle(struct song_info *arr, size_t n); // takes playlist and size of playlist; shuffles playlist; returns void
int add_song_to_queue(const char * path); // takes file path; adds song on path to queue; returns int
int delete_playlist(char *playlist_name); // takes playlist name as string; deletes playlist file; returns int
int add_playlist_to_queue(const char * name); // takes playlist name; adds all playlist songs to queue; returns int
void sort_genre(struct song_info *arr, int size); // takes playlist and length; sorts by genre alphabetically; returns null
void sort_title(struct song_info *arr, int size); // takes playlist and length; sorts by title alphabetically; returns null
void sort_artist(struct song_info *arr, int size); // takes playlist and length; sorts by artist alphabetically; returns null
void sort_duration(struct song_info *arr, int size); // takes playlist and length; sorts by duration in increasing order; returns null
void f2p(struct song_info *playlist, FILE *file, int *i); // takes playlist, file stream, and int pointer; reads file stream into playlist; returns null
void p2f(struct song_info *playlist, FILE *file, int *i); // takes playlist, file stream, and int pointer; reads all of playlist into file; returns null
struct song_info * get_song_info(struct song_info *info, char *PATH); // takes song_info struct pointer and filepath; reads file metadata into struct; returns struct
void sort(char *buffer, struct song_info *playlist, FILE *file, int *i); // takes a string buffer, playlist, file stream, and int pointer; prompts user for how to sort and sorts; returns null

void shuffle_test();

#endif