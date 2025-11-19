#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include "Playlist.h"
#include <algorithm>
#include <limits>

enum PlaybackSourceType {
    PLAYLIST_TYPE,
    QUEUE_TYPE
};

struct PlaybackState {
    PlaybackSourceType type;
    void* source;
    Node* currentNode;
    vector<bool> playedFlags;
    int startPosition;
    queue<Song*>* queueCopy;
    
    PlaybackState(PlaybackSourceType t, void* s, Node* n, int start) 
        : type(t), source(s), currentNode(n), startPosition(start), queueCopy(nullptr) {}
    
    ~PlaybackState() {
        if (queueCopy != nullptr) {
            delete queueCopy;
        }
    }
};

class MusicPlayer {
private:
    vector<Song*> allSongs;
    vector<Playlist*> playlists;
    queue<Song*> songQueue;
    stack<PlaybackState*> playbackStack;
    
    Song* currentlyPlaying;
    PlaybackSourceType currentSourceType;
    string currentSourceName;
    
    void displayCurrentlyPlaying();
    void handleSkipNext();
    void handleSkipPrev();
    void playNextFromPlaylist(PlaybackState* state);
    void playNextFromQueue(PlaybackState* state);
    void checkAndPopCompletedPlayback();
    Playlist* findPlaylist(const string& name);
    int getValidInteger(const string& prompt);
    void clearInputStream();
    
public:
    MusicPlayer();
    ~MusicPlayer();
    
    void run();
    void mainPage();
    void songsPage();
    void songDetailsPage(Song* song);
    void playlistsPage();
    void playlistPage(Playlist* playlist);
    void queuePage();
    
    void addSongToPlaylist(Song* song);
    void addSongToQueue(Song* song);
    void createNewPlaylist();
    void startPlaylistPlayback(Playlist* playlist);
    void startQueuePlayback();
    void deleteSongFromPlaylist(Playlist* playlist);
};

#endif