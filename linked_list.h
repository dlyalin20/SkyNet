#ifndef LINKED_LIST
#define LINKED_LIST "defined"

// structs
struct song_info {

    char path[BUFFER_SIZE];
	char artist[BUFFER_SIZE];
	char title[BUFFER_SIZE];
	char genre[BUFFER_SIZE];
	float seconds;

};

// functions
float get_duration(char *filename);
struct song_info * get_song_info(struct song_info *info, char *PATH);
void print_info(struct song_info *song);
int delete_playlist(char *playlist_name);
struct song_info * find_files(struct song_info *song_data, char * path);
int make_playlist();
int play_playlist(char *playlist_name);
int add_to_playlist();
void play_sorted();
void sort_title(struct song_info *arr, int size);
void sort_artist(struct song_info *arr, int size);
void sort_genre(struct song_info *arr, int size);

#endif
