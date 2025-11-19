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

        Node* current = head;
        Node* delete_node;
        while(current -> next != head){
            delete_node = current;
            current = current -> next;
            delete delete_node;
        }
        delete head;
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