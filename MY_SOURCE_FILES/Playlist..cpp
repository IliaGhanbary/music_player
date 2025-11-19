#include "Playlist.h"

void PlaylistDLL :: addSong(Song* song,int position)  {
    Node* newNode = new Node(song);
    
    if (head == nullptr) {
        head = newNode;
        head->next = head;
        head->prev = head;
        size = 1;
        return;
    }
    
    if (position < 1) position = 0;
    if (position > size) position = size;
    
    if (position == 0) {
        Node* tail = head->prev;
        newNode->next = head;
        newNode->prev = tail;
        tail->next = newNode;
        head->prev = newNode;
        head = newNode;
    } else {
        Node* current = head;
        for (int i = 0; i < position; i++) {
            current = current->next;
        }
        Node* prevNode = current->prev;
        newNode->next = current;
        newNode->prev = prevNode;
        prevNode->next = newNode;
        current->prev = newNode;
    }
    
    size++;
}
    
void PlaylistDLL :: removeSong(int position)
{
    if (head == nullptr || size == 0) return;
    
    if (position < 1) position = 1;
    if (position > size) position = size;
    
    Node* toDelete = head;
    for (int i = 1; i < position; i++) {
        toDelete = toDelete->next;
    }
    
    if (size == 1) {
        head = nullptr;
    } else {
        toDelete->prev->next = toDelete->next;
        toDelete->next->prev = toDelete->prev;
        
        if (toDelete == head) {
            head = toDelete->next;
        }
    }
    
    delete toDelete;
    size--;
}

Song* PlaylistDLL ::  getSong(int position) const
{
    if (head == nullptr) return nullptr;
    if (position < 1) position = 1;
    if (position > size) position = size;

    Node* current = head;
    for (int i = 1; i < position; i++) {
        current = current->next;
    }
    return current->song;
}

bool PlaylistDLL :: contains(Song* song) const {
    if (head == nullptr) return false;
    
    Node* current = head;
    do {
        if (current->song == song) return true;
        current = current->next;
    } while (current != head);
    
    return false;
}

void PlaylistDLL :: display() const {
    if (head == nullptr) return;
    
    Node* current = head;
    int index = 1;
    do {
        cout << index++ << ". " << current->song->getAllInfo() << endl;
        current = current->next;
    } while (current != head);
}

Node* PlaylistDLL :: getNode(int position) const {
    if (head == nullptr || position < 1 || position > size) return nullptr;
    
    Node* current = head;
    for (int i = 1; i < position; i++) {
        current = current->next;
    }
    return current;
}