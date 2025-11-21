#include "MusicPlayer.h"

MusicPlayer::MusicPlayer() : currentlyPlaying(nullptr) {
    allSongs.push_back(new Song("My Way Of Life ", "Frank Sinatra"));
    allSongs.push_back(new Song("The World We Knew", "Frank Sinatra"));
    allSongs.push_back(new Song("Hopelessly Devoted To You", "Olivia Newton-John"));
    allSongs.push_back(new Song("Save Your Tears", "The Weekend"));
    allSongs.push_back(new Song("A Man Without Love", "Engelbert"));
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

void MusicPlayer::pauseForMessage() {
    cout << Color::DIM << "\nPress Enter to continue..." << Color::RESET;
    cin.get();
}

int MusicPlayer::getValidInteger(const string& prompt) {
    int value;
    while (true) {
        cout << Color::CYAN << prompt << Color::RESET;
        if (cin >> value) {
            clearInputStream();
            return value;
        }
        cout << Color::RED << "⚠ Please enter a valid integer" << Color::RESET << endl;
        clearInputStream();
    }
}

void MusicPlayer::displayCurrentlyPlaying() {
    if (currentlyPlaying) {
        cout << "\n" << Color::BG_MAGENTA << Color::WHITE << Color::BOLD;
        cout << " ♫ NOW PLAYING " << Color::RESET << "\n";
        cout << Color::MAGENTA << " ┌────────────────────────────────────┐\n";
        cout << " │ " << Color::WHITE << Color::BOLD << left << setw(34) << currentlyPlaying->name << Color::MAGENTA << " │\n";
        cout << " │ " << Color::DIM << "Source: " << Color::RESET;
        if (currentSourceType == QUEUE_TYPE) {
            cout << Color::YELLOW << left << setw(26) << "Queue" << Color::MAGENTA << " │\n";
        } else {
            cout << Color::CYAN << left << setw(26) << currentSourceName << Color::MAGENTA << " │\n";
        }
        cout << " └────────────────────────────────────┘" << Color::RESET << "\n";
        cout << Color::DIM << "   Controls: " << Color::GREEN << "[>] Next";
        if (currentSourceType != QUEUE_TYPE) {
            cout << "  " << Color::GREEN << "[<] Prev";
        }
        cout << Color::RESET << "\n\n";
    }
}

Playlist* MusicPlayer::findPlaylist(const string& name) {
    for (auto playlist : playlists) {
        if (playlist->name == name) return playlist;
    }
    return nullptr;
}

void MusicPlayer::playNextFromPlaylist(PlaybackState* state, bool resume) {
    Playlist* playlist = static_cast<Playlist*>(state->source);
    
    if (playlist->song_playlist.head == nullptr) {
        currentlyPlaying = nullptr;
        delete playbackStack.top();
        playbackStack.pop();
        if (!playbackStack.empty()) {
            PlaybackState* nextState = playbackStack.top();
            if (nextState->type == PLAYLIST_TYPE) playNextFromPlaylist(nextState, true); 
            else playNextFromQueue(nextState, true); 
        }
        return;
    }

    if (!resume) {
        if (state->currentNode == nullptr) state->currentNode = playlist->song_playlist.head;
        else state->currentNode = state->currentNode->next;

        int index = 0;
        Node* temp = playlist->song_playlist.head;
        do {
            if (temp == state->currentNode) break;
            index++;
            temp = temp->next;
        } while (temp != playlist->song_playlist.head);
        
        bool allPlayed = true;
        for (bool flag : state->playedFlags) {
            if (!flag) { allPlayed = false; break; }
        }
        
        if (index < (int)state->playedFlags.size()) state->playedFlags[index] = true;

        if (allPlayed) {
            delete playbackStack.top();
            playbackStack.pop();
            if (!playbackStack.empty()) {
                PlaybackState* nextState = playbackStack.top();
                if (nextState->type == PLAYLIST_TYPE) playNextFromPlaylist(nextState, true);
                else playNextFromQueue(nextState, true);
            } else currentlyPlaying = nullptr;
            return;
        }
    }
    
    currentlyPlaying = state->currentNode->song;
    currentSourceType = PLAYLIST_TYPE;
    currentSourceName = playlist->name;
}

void MusicPlayer::playNextFromQueue(PlaybackState* state, bool resume) {
    if (songQueue.empty()) {
        delete playbackStack.top();
        playbackStack.pop();
        if (!playbackStack.empty()) {
            PlaybackState* nextState = playbackStack.top();
            if (nextState->type == PLAYLIST_TYPE) playNextFromPlaylist(nextState, true);  
            else playNextFromQueue(nextState, true); 
        } else currentlyPlaying = nullptr;
        return;
    }
    
    if (!resume) {
        songQueue.pop();
        if (songQueue.empty()) {
            delete playbackStack.top();
            playbackStack.pop();
            if (!playbackStack.empty()) {
                PlaybackState* nextState = playbackStack.top();
                if (nextState->type == PLAYLIST_TYPE) playNextFromPlaylist(nextState, true);
                else playNextFromQueue(nextState, true);
            } else currentlyPlaying = nullptr;
            return;
        }
    }
    
    currentlyPlaying = songQueue.front();
    currentSourceType = QUEUE_TYPE;
    currentSourceName = "queue";
}

void MusicPlayer::startQueuePlayback() {
    if (songQueue.empty()) return;
    PlaybackState* newState = new PlaybackState(QUEUE_TYPE, nullptr, nullptr, 0);
    playbackStack.push(newState);
    playNextFromQueue(newState, true);
    cout << Color::GREEN << "▶ Now playing from queue" << Color::RESET << endl;
    pauseForMessage();
}

void MusicPlayer::handleSkipNext() {
    if (!currentlyPlaying || playbackStack.empty()) return;
    PlaybackState* currentState = playbackStack.top();
    if (currentState->type == QUEUE_TYPE) playNextFromQueue(currentState, false);
    else playNextFromPlaylist(currentState, false);
}

void MusicPlayer::handleSkipPrev() {
    if (!currentlyPlaying || playbackStack.empty()) return;
    PlaybackState* currentState = playbackStack.top();
    if (currentState->type == QUEUE_TYPE) return;
    
    Playlist* playlist = static_cast<Playlist*>(currentState->source);
    if (playlist->song_playlist.head == nullptr || currentState->currentNode == nullptr) return;
    
    currentState->currentNode = currentState->currentNode->prev;
    
    int index = 0;
    Node* temp = playlist->song_playlist.head;
    do {
        if (temp == currentState->currentNode) break;
        index++;
        temp = temp->next;
    } while (temp != playlist->song_playlist.head);
    
    bool allPlayed = true;
    if (!playbackStack.empty()) {
        for (bool flag : playbackStack.top()->playedFlags) {
            if (!flag) { allPlayed = false; break; }
        }
    }

    if (allPlayed) {
        delete playbackStack.top();
        playbackStack.pop();
        if (!playbackStack.empty()) {
            PlaybackState* nextState = playbackStack.top();
            if (nextState->type == PLAYLIST_TYPE) playNextFromPlaylist(nextState, true);
            else playNextFromQueue(nextState, true);
        } else currentlyPlaying = nullptr;
        return;
    }

    if (index < (int)currentState->playedFlags.size()) currentState->playedFlags[index] = true;
    currentlyPlaying = currentState->currentNode->song;
}

void MusicPlayer::run() { mainPage(); }

void MusicPlayer::mainPage() {
    while (true) {
        system("clear");
        displayCurrentlyPlaying();
        printHeader("♪ MUSIC PLAYER ♪");
        printMenuItem("1", "🎵 Songs", Color::GREEN);
        printMenuItem("2", "📁 Playlists", Color::CYAN);
        printMenuItem("3", "📋 Song Queue", Color::YELLOW);
        printDivider();
        printMenuItem("0", "Exit", Color::RED);
        
        cout << "\n" << Color::BOLD << "Select: " << Color::RESET;
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") handleSkipNext();
        else if (input == "<") handleSkipPrev();
        else if (input == "1") songsPage();
        else if (input == "2") playlistsPage();
        else if (input == "3") queuePage();
        else if (input == "0") {
            cout << Color::MAGENTA << "\n♪ Thanks for listening! Goodbye! ♪\n" << Color::RESET;
            break;
        }
    }
}

void MusicPlayer::songsPage() {
    while (true) {
        system("clear");
        displayCurrentlyPlaying();
        printHeader("🎵 ALL SONGS");
        
        for (size_t i = 0; i < allSongs.size(); i++) {
            printSongItem(i + 1, allSongs[i]->getAllInfo());
        }
        printDivider();
        printMenuItem("0", "Back", Color::RED);
        
        cout << "\n" << Color::BOLD << "Select song: " << Color::RESET;
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") handleSkipNext();
        else if (input == "<") handleSkipPrev();
        else if (input == "0") return;
        else {
            try {
                int choice = stoi(input);
                if (choice >= 1 && choice <= (int)allSongs.size()) songDetailsPage(allSongs[choice - 1]);
            } catch (...) {}
        }
    }
}

void MusicPlayer::songDetailsPage(Song* song) {
    while (true) {
        system("clear");
        displayCurrentlyPlaying();
        
        cout << Color::BOLD << Color::CYAN << "\n  ┌─────────────────────────────────┐\n";
        cout << "  │ " << Color::WHITE << "🎵 " << left << setw(28) << song->name << Color::CYAN << " │\n";
        cout << "  │ " << Color::DIM << "   by " << Color::YELLOW << left << setw(24) << song->artist << Color::CYAN << " │\n";
        cout << "  └─────────────────────────────────┘" << Color::RESET << "\n\n";
        
        vector<Playlist*> containingPlaylists;
        for (auto playlist : playlists) {
            if (playlist->song_playlist.contains(song)) containingPlaylists.push_back(playlist);
        }
        
        if (!containingPlaylists.empty()) {
            cout << Color::DIM << "  In playlists:\n" << Color::RESET;
            for (size_t i = 0; i < containingPlaylists.size(); i++) {
                cout << Color::MAGENTA << "    " << (i+1) << ". " << containingPlaylists[i]->name << Color::RESET << "\n";
            }
            cout << "\n";
        }
        
        printMenuItem("1", "View playlists containing this song", Color::CYAN);
        printMenuItem("2", "Add to playlist", Color::GREEN);
        printMenuItem("3", "Add to queue", Color::YELLOW);
        printDivider();
        printMenuItem("0", "Back", Color::RED);
        
        cout << "\n" << Color::BOLD << "Select: " << Color::RESET;
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") handleSkipNext();
        else if (input == "<") handleSkipPrev();
        else if (input == "0") return;
        else if (input == "2") addSongToPlaylist(song);
        else if (input == "3") { addSongToQueue(song); return; }
        else if (input == "1" && !containingPlaylists.empty()) {
            cout << Color::CYAN << "Select playlist number: " << Color::RESET;
            string plInput;
            cin >> plInput;
            clearInputStream();
            try {
                int plChoice = stoi(plInput);
                if (plChoice >= 1 && plChoice <= (int)containingPlaylists.size()) {
                    playlistPage(containingPlaylists[plChoice - 1]);
                } else {
                    cout << Color::RED << "⚠ Please enter a valid number (1-" << containingPlaylists.size() << ")" << Color::RESET << endl;
                    pauseForMessage();
                }
            } catch (...) {
                cout << Color::RED << "⚠ Please enter a valid integer" << Color::RESET << endl;
                pauseForMessage();
            }
        }
    }
}

void MusicPlayer::addSongToPlaylist(Song* song) {
    cout << Color::CYAN << "\nEnter playlist name: " << Color::RESET;
    string playlistName;
    getline(cin, playlistName);
    
    Playlist* playlist = findPlaylist(playlistName);
    if (playlist == nullptr) {
        cout << Color::RED << "✗ Playlist not found" << Color::RESET << endl;
        pauseForMessage();
        return;
    }
    if (playlist->song_playlist.contains(song)) {
        cout << Color::YELLOW << "⚠ Song already in playlist" << Color::RESET << endl;
        pauseForMessage();
        return;
    }
    
    cout << Color::DIM << "\nCurrent playlist contents:\n" << Color::RESET;
    playlist->song_playlist.display();
    int position = getValidInteger("Position to insert: ");
    
    if (!playbackStack.empty() && playbackStack.top()->type == PLAYLIST_TYPE) {
        PlaybackState* state = playbackStack.top();
        if (state->source == playlist) {
            if (position < 1) position = 0;
            if (position > playlist->song_playlist.size) position = playlist->song_playlist.size;
            state->playedFlags.insert(state->playedFlags.begin() + position, false);
        }
    }
    
    playlist->song_playlist.addSong(song, position);
    cout << Color::GREEN << "✓ Song added to playlist!" << Color::RESET << endl;
    pauseForMessage();
}

void MusicPlayer::addSongToQueue(Song* song) {
    songQueue.push(song);
    cout << Color::GREEN << "✓ Song added to queue!" << Color::RESET << endl;
    pauseForMessage();
}

void MusicPlayer::playlistsPage() {
    while (true) {
        system("clear");
        displayCurrentlyPlaying();
        printHeader("📁 PLAYLISTS");
        
        if (playlists.empty()) {
            cout << Color::DIM << "  No playlists yet. Create one!\n" << Color::RESET;
        } else {
            for (size_t i = 0; i < playlists.size(); i++) {
                cout << Color::CYAN << " " << setw(2) << (i+1) << ". " << Color::WHITE << playlists[i]->name << Color::RESET << "\n";
            }
        }
        printDivider();
        printMenuItem("-1", "Create new playlist", Color::GREEN);
        printMenuItem("0", "Back", Color::RED);
        
        cout << "\n" << Color::BOLD << "Select: " << Color::RESET;
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") handleSkipNext();
        else if (input == "<") handleSkipPrev();
        else if (input == "0") return;
        else if (input == "-1") createNewPlaylist();
        else {
            try {
                int choice = stoi(input);
                if (choice >= 1 && choice <= (int)playlists.size()) playlistPage(playlists[choice - 1]);
            } catch (...) {}
        }
    }
}

void MusicPlayer::createNewPlaylist() {
    string name;
    while (true) {
        cout << Color::CYAN << "\nEnter playlist name: " << Color::RESET;
        getline(cin, name);
        if (findPlaylist(name) != nullptr) {
            cout << Color::RED << "✗ Playlist already exists" << Color::RESET << endl;
        } else {
            playlists.push_back(new Playlist(name));
            cout << Color::GREEN << "✓ Playlist \"" << name << "\" created!" << Color::RESET << endl;
            pauseForMessage();
            return;
        }
    }
}

void MusicPlayer::playlistPage(Playlist* playlist) {
    while (true) {
        system("clear");
        displayCurrentlyPlaying();
        
        cout << Color::BOLD << Color::MAGENTA << "\n╔═══════════════════════════════════════╗\n";
        cout << "║  📁 " << left << setw(33) << playlist->name << "║\n";
        cout << "╚═══════════════════════════════════════╝" << Color::RESET << "\n\n";
        
        if (playlist->song_playlist.head == nullptr) {
            cout << Color::DIM << "  Empty playlist. Add some songs!\n" << Color::RESET;
        } else {
            playlist->song_playlist.display();
        }
        
        cout << "\n";
        printMenuItem("-2", "Play playlist", Color::GREEN);
        printMenuItem("-1", "Delete a song", Color::YELLOW);
        printDivider();
        printMenuItem("0", "Back", Color::RED);
        
        cout << "\n" << Color::BOLD << "Select: " << Color::RESET;
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") handleSkipNext();
        else if (input == "<") handleSkipPrev();
        else if (input == "0") return;
        else if (input == "-2") startPlaylistPlayback(playlist);
        else if (input == "-1") deleteSongFromPlaylist(playlist);
        else {
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
    
    int startPos = getValidInteger("Start from position: ");
    if (startPos < 1) startPos = 1;
    if (startPos > playlist->song_playlist.size) startPos = playlist->song_playlist.size;
    
    Node* startNode = playlist->song_playlist.getNode(startPos);
    if (startNode == nullptr) return;
    
    PlaybackState* newState = new PlaybackState(PLAYLIST_TYPE, playlist, nullptr, startPos);
    newState->playedFlags.resize(playlist->song_playlist.size, false);
    newState->currentNode = startNode->prev;
    
    playbackStack.push(newState);
    playNextFromPlaylist(newState, false);
    cout << Color::GREEN << "▶ Now playing from \"" << playlist->name << "\"" << Color::RESET << endl;
    pauseForMessage();
}

void MusicPlayer::deleteSongFromPlaylist(Playlist* playlist) {
    if (playlist->song_playlist.head == nullptr) return;
    
    int position = getValidInteger("Song number to delete: ");
    if (position < 1) position = 1;
    if (position > playlist->song_playlist.size) position = playlist->song_playlist.size;
    
    bool needsAutoSkip = false;

    if (!playbackStack.empty() && playbackStack.top()->type == PLAYLIST_TYPE) {
        PlaybackState* state = playbackStack.top();
        if (state->source == playlist && currentlyPlaying != nullptr) {
            Node* nodeToDelete = playlist->song_playlist.getNode(position);
            if (state->currentNode == nodeToDelete) {
                if (playlist->song_playlist.size == 1) {
                    state->currentNode = nullptr;
                    currentlyPlaying = nullptr;
                } else {
                    state->currentNode = nodeToDelete->prev;
                    needsAutoSkip = true;
                }
            }
            if (position - 1 < (int)state->playedFlags.size())
                state->playedFlags.erase(state->playedFlags.begin() + (position - 1));
        }
    }
    
    playlist->song_playlist.removeSong(position);
    cout << Color::GREEN << "✓ Song deleted" << Color::RESET << endl;
    pauseForMessage();
    if (needsAutoSkip) handleSkipNext();
}

void MusicPlayer::queuePage() {
    while (true) {
        system("clear");
        displayCurrentlyPlaying();
        printHeader("📋 SONG QUEUE");
        
        if (songQueue.empty()) {
            cout << Color::DIM << "  Queue is empty. Add some songs!\n" << Color::RESET;
        } else {
            queue<Song*> tempQueue = songQueue;
            int index = 1;
            while (!tempQueue.empty()) {
                printSongItem(index++, tempQueue.front()->getAllInfo());
                tempQueue.pop();
            }
        }
        
        cout << "\n";
        printMenuItem("-1", "Play queue", Color::GREEN);
        printDivider();
        printMenuItem("0", "Back", Color::RED);
        
        cout << "\n" << Color::BOLD << "Select: " << Color::RESET;
        string input;
        cin >> input;
        clearInputStream();
        
        if (input == ">") handleSkipNext();
        else if (input == "<") handleSkipPrev();
        else if (input == "0") return;
        else if (input == "-1") startQueuePlayback();
        else {
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