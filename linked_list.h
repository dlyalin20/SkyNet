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

struct songs {

	char name[BUFFER_SIZE];
	struct songs *next;

};

// functions
float get_duration(char *filename);
struct song_info * get_song_info(struct song_info *info, char *PATH);
void print_song(struct songs *song);
void print_playlist(struct songs *HEAD);
struct songs * create_song(char *title);
struct songs * insert_song_end(struct songs *to_add, struct songs *HEAD);
struct songs * remove_song(char *title, struct songs *HEAD);
int delete_playlist(char *playlist_name);
struct song_info * find_files(struct song_info *song_data, char * path);

#endif