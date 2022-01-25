#ifndef adderhelpers_h
#define adderhelpers_h hi

void shuffle(char **array, size_t n);
// shuffles the queue file

int logic_controller(char *buffer);
int initialize(int argc, char **argv);
int make_playlist();
float get_duration(char *filename);
struct song_info * get_song_info(struct song_info *info, char *PATH);
void print_info(struct song_info *song);
int delete_playlist(char *playlist_name);
struct song_info * find_files(struct song_info *song_data, char * path);
int make_playlist(const char * path);
int make_playlist_simple();
int add_to_playlist();
void sort_title(struct song_info *arr, int size);
void sort_artist(struct song_info *arr, int size);
void sort_genre(struct song_info *arr, int size);
// simple playlist only
int add_playlist_to_queue(const char * name);
int clear_queue();
int create_sema();
int add_song_to_queue(const char * path);
void sort_write();
void remove_from_playlist();
void sort_duration(struct song_info *arr, int size);
static void sighandler(int sigint);
void play_sorted();
void s2f(char *buffer, FILE *file);
void f2p(struct song_info *playlist, FILE *file, int *i);
void p2f(struct song_info *playlist, FILE *file, int *i);
void get_pname(char *buffer);
void sort(char *buffer, struct song_info *playlist, FILE *file, int *i);
void get_sname(char *buffer);

#endif
