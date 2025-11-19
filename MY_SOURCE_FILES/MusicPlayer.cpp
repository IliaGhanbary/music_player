#include "MusicPlayer.h"

MusicPlayer::MusicPlayer() : currentlyPlaying(nullptr) {
    allSongs.push_back(new Song("track 10", "artist B"));
    allSongs.push_back(new Song("track 2", "artist D"));
    allSongs.push_back(new Song("track 5", "artist A"));
    allSongs.push_back(new Song("track 1", "artist C"));
    allSongs.push_back(new Song("track 8", "artist E"));
}

MusicPlayer::~MusicPlayer() {
    for (auto song : allSongs) delete song;
    for (auto playlist : playlists) delete playlist;
    while (!playbackStack.empty()) {
        delete playbackStack.top();
        playbackStack.pop();
    }
}

void MusicPlayer::clearInputStream() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int MusicPlayer::getValidInteger(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInputStream();
            return value;
        }
        cout << "enter an integer" << endl;
        clearInputStream();
    }
}

void MusicPlayer::displayCurrentlyPlaying() {
    if (currentlyPlaying) {
        cout << "\nsong playing: \"" << currentlyPlaying->name << "\"" << endl;
        cout << "source: \"";
        if (currentSourceType == QUEUE_TYPE) {
            cout << "queue\"" << endl;
            cout << ">. next" << endl;
        } else {
            cout << currentSourceName << "\"" << endl;
            cout << ">. next" << endl;
            cout << "<. prev" << endl;
        }
        cout << endl;
    }
}

Playlist* MusicPlayer::findPlaylist(const string& name) {
    for (auto playlist : playlists) {
        if (playlist->name == name) return playlist;
    }
    return nullptr;
}

void MusicPlayer::playNextFromPlaylist(PlaybackState* state) {
    Playlist* playlist = static_cast<Playlist*>(state->source);
    
    if (playlist->song_playlist.head == nullptr) {
        currentlyPlaying = nullptr;
        delete playbackStack.top();
        playbackStack.pop();
        
        if (!playbackStack.empty()) {
            PlaybackState* nextState = playbackStack.top();
            if (nextState->type == PLAYLIST_TYPE) {
                playNextFromPlaylist(nextState);
            } else {
                playNextFromQueue(nextState);
            }
        }
        return;
    }
    
    if (state->currentNode == nullptr) {
        state->currentNode = playlist->song_playlist.head;
    } else {
        state->currentNode = state->currentNode->next;
    }
    
    int index = 0;
    Node* temp = playlist->song_playlist.head;
    do {
        if (temp == state->currentNode) break;
        index++;
        temp = temp->next;
    } while (temp != playlist->song_playlist.head);
    
    state->playedFlags[index] = true;
    
    bool allPlayed = true;
    for (bool flag : state->playedFlags) {
        if (!flag) {
            allPlayed = false;
            break;
        }
    }
    
    if (allPlayed) {
        delete playbackStack.top();
        playbackStack.pop();
        
        if (!playbackStack.empty()) {
            PlaybackState* nextState = playbackStack.top();
            if (nextState->type == PLAYLIST_TYPE) {
                playNextFromPlaylist(nextState);
            } else {
                playNextFromQueue(nextState);
            }
        } else {
            currentlyPlaying = nullptr;
        }
        return;
    }
    
    currentlyPlaying = state->currentNode->song;
    currentSourceType = PLAYLIST_TYPE;
    currentSourceName = playlist->name;
}

void MusicPlayer::playNextFromQueue(PlaybackState* state) {
    if (state->queueCopy == nullptr || state->queueCopy->empty()) {
        delete playbackStack.top();
        playbackStack.pop();
        
        if (!playbackStack.empty()) {
            PlaybackState* nextState = playbackStack.top();
            if (nextState->type == PLAYLIST_TYPE) {
                playNextFromPlaylist(nextState);
            } else {
                playNextFromQueue(nextState);
            }
        } else {
            currentlyPlaying = nullptr;
        }
        return;
    }
    
    currentlyPlaying = state->queueCopy->front();
    state->queueCopy->pop();
    currentSourceType = QUEUE_TYPE;
    currentSourceName = "queue";
}

void MusicPlayer::handleSkipNext() {
    if (!currentlyPlaying || playbackStack.empty()) return;
    
    PlaybackState* currentState = playbackStack.top();
    
    if (currentState->type == QUEUE_TYPE) {
        playNextFromQueue(currentState);
    } else {
        playNextFromPlaylist(currentState);
    }
}

void MusicPlayer::handleSkipPrev() {
    if (!currentlyPlaying || playbackStack.empty()) return;
    
    PlaybackState* currentState = playbackStack.top();
    
    if (currentState->type == QUEUE_TYPE) {
        return;
    }
    
    Playlist* playlist = static_cast<Playlist*>(currentState->source);
    
    if (playlist->song_playlist.head == nullptr) return;
    
    if (currentState->currentNode == nullptr) return;
    
    currentState->currentNode = currentState->currentNode->prev;
    
    int index = 0;
    Node* temp = playlist->song_playlist.head;
    do {
        if (temp == currentState->currentNode) break;
        index++;
        temp = temp->next;
    } while (temp != playlist->song_playlist.head);
    
    currentState->playedFlags[index] = true;
    currentlyPlaying = currentState->currentNode->song;
}

void MusicPlayer::run() {
    mainPage();
}

void MusicPlayer::mainPage() {
    while (true) {
        displayCurrentlyPlaying();
        cout << "main page" << endl;
        cout << "1. songs" << endl;
        cout << "2. playlists" << endl;
        cout << "3. song queue" << endl;
        cout << "0. exit" << endl;
        
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") {
            handleSkipNext();
        } else if (input == "<") {
            handleSkipPrev();
        } else if (input == "1") {
            songsPage();
        } else if (input == "2") {
            playlistsPage();
        } else if (input == "3") {
            queuePage();
        } else if (input == "0") {
            break;
        }
    }
}

void MusicPlayer::songsPage() {
    while (true) {
        displayCurrentlyPlaying();
        cout << "list of songs" << endl;
        for (size_t i = 0; i < allSongs.size(); i++) {
            cout << (i + 1) << ". " << allSongs[i]->getAllInfo() << endl;
        }
        cout << "0. back" << endl;
        
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") {
            handleSkipNext();
        } else if (input == "<") {
            handleSkipPrev();//
        } else if (input == "0") {
            return;
        } else {
            try {
                int choice = stoi(input);
                if (choice >= 1 && choice <= (int)allSongs.size()) {
                    songDetailsPage(allSongs[choice - 1]);
                }
            } catch (...) {}
        }
    }
}

void MusicPlayer::songDetailsPage(Song* song) {
    while (true) {
        displayCurrentlyPlaying();
        cout << song->getAllInfo() << endl;
        
        vector<Playlist*> containingPlaylists;
        for (auto playlist : playlists) {
            if (playlist->song_playlist.contains(song)) {
                containingPlaylists.push_back(playlist);
            }
        }
        
        cout << "1. list of playlists" << endl;
        if (!containingPlaylists.empty()) {
            for (size_t i = 0; i < containingPlaylists.size(); i++) {
                cout << "   " << (i + 1) << ". " << containingPlaylists[i]->name << endl;
            }
        }
        cout << "2. add to play list" << endl;
        cout << "3. add to queue" << endl;
        cout << "0. back" << endl;
        
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") {
            handleSkipNext();
        } else if (input == "<") {
            handleSkipPrev();
        } else if (input == "0") {
            return;
        } else if (input == "2") {
            addSongToPlaylist(song);
        } else if (input == "3") {
            addSongToQueue(song);
            return;
        } else if (input == "1") {
            if (!containingPlaylists.empty()) {
                cout << "Select playlist: ";
                string plInput;
                cin >> plInput;
                clearInputStream();
                try {
                    int plChoice = stoi(plInput);
                    if (plChoice >= 1 && plChoice <= (int)containingPlaylists.size()) {
                        playlistPage(containingPlaylists[plChoice - 1]);
                    }
                } catch (...) {}
            }
        }
    }
}

void MusicPlayer::addSongToPlaylist(Song* song) {
    cout << "enter playlist name: ";
    string playlistName;
    getline(cin, playlistName);
    
    Playlist* playlist = findPlaylist(playlistName);
    if (playlist == nullptr) {
        cout << "playlist not found" << endl;
        return;
    }
    
    if (playlist->song_playlist.contains(song)) {
        cout << "song already exists" << endl;
        return;
    }
    
    playlist->song_playlist.display();
    int position = getValidInteger("where do you want to add it: ");
    
    playlist->song_playlist.addSong(song, position);
    cout << "song added to playlist" << endl;
}

void MusicPlayer::addSongToQueue(Song* song) {
    songQueue.push(song);
    cout << "song added to queue" << endl;
}

void MusicPlayer::playlistsPage() {
    while (true) {
        displayCurrentlyPlaying();
        cout << "list of playlists" << endl;
        for (size_t i = 0; i < playlists.size(); i++) {
            cout << (i + 1) << ". " << playlists[i]->name << endl;
        }
        cout << "-1. add playlist" << endl;
        cout << "0. back" << endl;
        
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") {
            handleSkipNext();
        } else if (input == "<") {
            handleSkipPrev();
        } else if (input == "0") {
            return;
        } else if (input == "-1") {
            createNewPlaylist();
        } else {
            try {
                int choice = stoi(input);
                if (choice >= 1 && choice <= (int)playlists.size()) {
                    playlistPage(playlists[choice - 1]);
                }
            } catch (...) {}
        }
    }
}

void MusicPlayer::createNewPlaylist() {
    string name;
    while (true) {
        cout << "enter a name for the new playlist: ";
        getline(cin, name);
        
        if (findPlaylist(name) != nullptr) {
            cout << "playlist already exists" << endl;
        } else {
            playlists.push_back(new Playlist(name));
            cout << "new playlist added to list" << endl;
            return;
        }
    }
}

void MusicPlayer::playlistPage(Playlist* playlist) {
    while (true) {
        displayCurrentlyPlaying();
        cout << playlist->name << endl;
        playlist->song_playlist.display();
        cout << "-2. play" << endl;
        cout << "-1. delete a song" << endl;
        cout << "0. back" << endl;
        
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") {
            handleSkipNext();
        } else if (input == "<") {
            handleSkipPrev();
        } else if (input == "0") {
            return;
        } else if (input == "-2") {
            startPlaylistPlayback(playlist);
        } else if (input == "-1") {
            deleteSongFromPlaylist(playlist);
        } else {
            try {
                int choice = stoi(input);
                if (choice >= 1 && choice <= playlist->song_playlist.size) {
                    Song* song = playlist->song_playlist.getSong(choice);
                    if (song) songDetailsPage(song);
                }
            } catch (...) {}
        }
    }
}

void MusicPlayer::startPlaylistPlayback(Playlist* playlist) {
    if (playlist->song_playlist.head == nullptr) return;
    
    int startPos = getValidInteger("start from position: ");
    
    if (startPos < 1) startPos = 1;
    if (startPos > playlist->song_playlist.size) startPos = playlist->song_playlist.size;
    
    Node* startNode = playlist->song_playlist.getNode(startPos);
    if (startNode == nullptr) return;
    
    PlaybackState* newState = new PlaybackState(PLAYLIST_TYPE, playlist, startNode->prev, startPos);
    newState->playedFlags.resize(playlist->song_playlist.size, false);
    
    playbackStack.push(newState);
    playNextFromPlaylist(newState);
}

void MusicPlayer::deleteSongFromPlaylist(Playlist* playlist) {
    int position = getValidInteger("which song: ");
    
    if (playlist->song_playlist.head == nullptr) return;
    
    playlist->song_playlist.removeSong(position);
    cout << "song deleted" << endl;
}

void MusicPlayer::queuePage() {
    while (true) {
        displayCurrentlyPlaying();
        cout << "songs in queue" << endl;
        
        queue<Song*> tempQueue = songQueue;
        int index = 1;
        while (!tempQueue.empty()) {
            cout << index++ << ". " << tempQueue.front()->getAllInfo() << endl;
            tempQueue.pop();
        }
        
        cout << "-1. play" << endl;
        cout << "0. back" << endl;
        
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") {
            handleSkipNext();
        } else if (input == "<") {
            handleSkipPrev();
        } else if (input == "0") {
            return;
        } else if (input == "-1") {
            startQueuePlayback();
        } else {
            try {
                int choice = stoi(input);
                queue<Song*> tempQ = songQueue;
                Song* selectedSong = nullptr;
                for (int i = 1; i <= choice && !tempQ.empty(); i++) {
                    if (i == choice) selectedSong = tempQ.front();
                    tempQ.pop();
                }
                if (selectedSong) songDetailsPage(selectedSong);
            } catch (...) {}
        }
    }
}

void MusicPlayer::startQueuePlayback() {
    if (songQueue.empty()) return;
    
    PlaybackState* newState = new PlaybackState(QUEUE_TYPE, nullptr, nullptr, 0);
    
    newState->queueCopy = new queue<Song*>(songQueue);
    
    playbackStack.push(newState);
    playNextFromQueue(newState);
}