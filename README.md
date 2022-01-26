# C-Potify
Systems Level Programming Final Project. Buyer beware!

**Names:** Daniel Lyalin, Shane Decatur, Jason Jiang
**Period:** 5

# Project Description
Our project, most generally, is a music playing application that allows for terminal
interaction with .wav files (and only .wav files!). Via the SDL2 library, users can
play individual audio files ('songs'), create and interact with playlists, create and
interact with song queues, and set up servers to listen to songs/playlists over multiple devices.

# Required Libraries + Installation Instructions:
* SDL2:
    * OSX (may vary by Mac, especially steps 2 & 3):
        1. brew install sdl2
        2. XCode: Build Settings->All->Search Paths->Header Search Paths->/usr/local/include
        3. Xcode: General->add Frameworks and Libraries->sdl2.framework
        4. #include <SDL2/SDL.h> in code (already included in our .h files)
        5. When compiling, include: -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -lSDL2
    * Unix:
        1. apt-get install libsdl2-dev (alternatively: yum install SDL2-devel)
        2. When compiling, include: -I/usr/include player.o -lSDL2
    * Windows:
        1. Godspeed!
* exiftool:
    * OSX:
        1. Download from here: https://exiftool.org/index.html
        2. Follow the instructions.
    * Unix:
        1. Reference Mac instructions
    * Windows:
        1. Godspeed!
* cJSON
    * .h and .c files already included
    * OSX:
        1. When compiling, include -lcJSON
    * Unix: No additional steps for Unix machines
    * Windows:
        1. Godspeed!

# Project Instructions:

Because there are two large component programs to our project, we have included guides on how to interact with each one separately. The two components are adder (used for manipulating data structures) and player (used for playing music). Initially, we planned to also have a server for multi-machine audio playing and sharing, but, due to time constraints, that portion was cut out. Instructions are below and also included in the instructions.txt file.

## Warnings:

1. Before using anything, be forewarned that SDL (seemingly) only works with '.wav' audio files, so this project will only execute audio files that use that compression method.
2. When you are being prompted for song titles, we ALWAYS expect/assume that the song will be inputted with the proper extensions; we are NEVER simply asking for <name>, but rather always for <name>.wav.
3. Playlist names are NOT .txt files or anything of that sort; they have no extension. Please do not include one!
4. When we prompt for input, we expect for there to be no whitespace outside of new-line ("\n") characters; this means you should be entering single-word answers for all prompts, which is what they all ask for. This is not Spotify: no need for quirky, five sentence playlist names!
5. The maximum characters any string buffer in our program can store is 100; hence, please make sure all inputs are <= 100 characters.
6. Permanently is taken to mean that the previous state of something is made irretrievable, not that it cannot be changed again and is immutable.
7. Once a playlist is playing, it cannot be directly stopped except by skipping through every song one by one. Only individual songs can be quit or paused/unpaused. It is possible Ctrl-C or Ctrl-Z may do the job, but expected results are not guaranteed due to SDL volatility.
8. Commands CANNOT be stacked; this means trying to execute more than one command or flag at a time will result in an warning.
9. All initialized playlists must have unique names; if a new playlist is created with the same name, it will overwrite the other one.
10. When compiling, the 'makefile' is targeted at OSX-running machines. 'makefilelin' is targeted at Linux machines (theoretically including Ubuntu).

## Using Adder

Adder is used for manipulating playlists and the queue. Below are all possible uses of adder described:

* `./adder -makeps` (Make Playlist from Songs):
    1. Prompts user for a playlist name
    2. Loops through prompting user for songs and adding songs to playlist
    3. Loop exited with command "exit"
*  `./adder -makep [path]` (Make Playlist from Path):
    1. Takes command line argument of file path
    2. Automatically creates a playlist of all songs held in the directory specified by that path
* `./adder -add2p` (Add Song to Playlist):
    1. Prompts user for a playlist to add the song to
    2. Prompts user for song to add
    3. Adds song to playlist
* `./adder -ds` (Delete Song from Playlist):
    1. Prompts user for playlist
    2. Prompts user for song to delete
    3. Removes that song from playlist
* `./adder -permashuffle` (Permanently Shuffle Playlist):
    1. Prompts user for playlist
    2. Permanently changes playlist order to random new order
* `./adder -savesort` (Save a Sorted Playlist):
    1. Prompts user for playlist to sort
    2. Prompts user for sort type to use
    3. Permanently sorts the playlist the prompted way
* `./adder -addp2q [playlist name]` (Add Playlist to Queue):
    1. Takes playlist name as argument
    2. Adds that entire playlist to the queue of songs to play
* `./adder -s2q [path]` (Add Song to Queue):
    1. Takes as argument the song to add
    2. Adds song to queue
* `./adder -dq` (Delete Queue):
    1. Clears the queue of all songs

## Using Player:

Player is used for playing songs, playlists, and queues. Below are all possible uses of player described:

* `./player -p [song path]` (Play Song):
    1. Takes as argument the song path
    2. Plays that one song
* `./player -pp [playlist name]` (Play Playlist):
    1. Takes as argument playlist name
    2. Plays all the songs on the playlist in the order they're read
* `./player -pq` (Play Queue):
    1. Plays the queue
* `./player -playsort` (Play Sorted Playlist):
    1. Prompts user for playlist
    2. Prompts user for sort type
    3. Temporarily sorts playlist
    4. Plays playlist in newly sorted order
* `./player -playshuffle` (Play Shuffled Playlist):
    1. Prompts user for playlist
    2. Temporarily shuffles playlist
    3. Plays playlist in temporarily shuffled order

# Sample Usage:

We've included five sample .wav files (ex1.wav, ex2.wav, ex3.wav, wav.wav, CantinaBand3.wav) for easy test usage. Try running the code below to get an idea of how the program works. It is intended that both adder and player are in the same directory as the audio files and are run from separate terminal prompts (not that this would make any functional difference). Obviously, the example usage below does not include all possible cases, but is a nice start.

1. ./player -p ex1.wav
2. ./adder -makeps
3. ./player -pp
4. ./adder -add2p
5. ./adder -ds
6. ./adder -permashuffle
7. ./player -pp
8. ./adder -addp2q "playlist name"
9. ./player -pq
10. ./player -playsort "playlist name"
11. ./adder -pp ModelSongs
12. ./player -pp "playlist name"
