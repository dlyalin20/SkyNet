# SkyNet
Systems Level Programming Final Project. Buyer beware!

**Names:** Daniel Lyalin, Shane Decatur, Jason Jiang
**Period:** 5

# Problem Statement

    The goal of our project—affectionately titled *SkyNet*, or *C-Potify*— is to create a lightweight music-playing application à la Spotify. The core features will be the capability to, having initialized the program in a directory containing music files, play any of the files from the command line (also in shuffle order), create playlists (data structures) relating the order in which the added songs should be played, play playlists, pause songs, sort songs by standard categories (e.g. name , artist, title), and (hopefully) display lyrics. Having established this base functionality, we intend to make it possible to sync multiple devices over a server and transfer song files over the server, to allow group listening sessions (where individual devices can add songs to the queue). Optional features we'd like to design include allowing games of musical trivia across the server (i.e. guess the melody)and virtual karaoke.

# UI

    The user will run the program from within a directory containing audio files, or run the program with the path name of such a directory provided; the program must be initialized to a specific path, after which it can only be used within the scope of that path until it is reinitialized. Then, the user can specify file names or playlist names, as well as flags for specific arguments (such as shuffle, or sort), to play specific songs. If they wish to sync with other devices, one user can start a server via command line and then provide the proper address to other local machines; using command line arguments, they can then specify which song files to transfer. The music trivia game would be a different argument, and will have the server block for data from either device (forking server style) until one of the devices wins, at which point it displays the song data and prompts for input to continue or stop. If users wish, the terminal can be used to print lyrics (if an API can be found -- perhaps Java can be added in some way).

# Technical Aspects

    ## Topics Covered in Class:
    
        - Files & file information: Used when manipulating audio files and getting data
        - Processes: Exec may be used for running a Java program to access a lyrics API; forking used by server and may be used by program to run songs while remaining open for further user actions
        - Signals: Used for pausing music, skipping songs, etc. via keyboard inputs 
        - Sockets: Used for developing a forking server which can transfer data and facilitate communication

    ## Work Breakdown

        - Daniel: Data structures, file manipulation, server setup

        - Jason: API access, game development (i.e. logic and data transfer)

        - Shane: Debugging, command line argument logic

    ## Data Structures:

        - Double-Linked List: Used for playlists. Will contain the titles of individual audio files, as well as data about them, such as run-time and artist, if such information can be obtained (for example, from their metadata).

# Timeline 

    1. Program initialization: cataloging songs, playing music by accessing speakers
    2. Advanced Musics: requesting specific songs via command line, organizing songs by various characteristics, parsing song metadata for features, API use for lyrics and data (especially if metadata fails)
    3. Playlists: Forming playlists, organizing playlists, playing playlists (essentially: fancy linked lists!)
    4. Networking: server setup with forking and simultaneous communication, data transfer between devices 
    5. More networking: Syncing devices to play music together
    6. (optional) Games: Music trivia, karaoke

