from lyricsgenius import Genius

def find_lyrics(name, title):
    genius = Genius("8c3w4v1lPEwjwAFKYN4538zCzfHAGBgYRYER4f1oa6shNYX-Wmj7z-3xxrhTDxfX")
    song = genius.search_song(title, name)
    try:
        #print(song.lyrics)
        return song.lyrics
    except:
        print("Song not found")

#find_lyrics("Beatles, the", "Hey Jude")
