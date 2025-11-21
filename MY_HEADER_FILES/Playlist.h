#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "Helper.h"

class PlaylistDLL{
public:
    Node *head;
    int size;

    PlaylistDLL(): head(nullptr) , size(0) {}
    ~PlaylistDLL(){
        if (head == nullptr) return;
        
        Node* tail = head->prev;
        tail->next = nullptr;
        
        Node* current = head;
        while(current != nullptr){
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        head = nullptr;
    }

    void addSong(Song* song, int position);
    
    void removeSong(int position);
    
    Song* getSong(int position) const ;
    
    bool contains(Song* song) const ;
    
    void display() const ;
    
    Node* getNode(int position) const ;

};

class Playlist {
public:
    string name;
    PlaylistDLL song_playlist;
    
    Playlist(string n) : name(n) {}
};

#endif