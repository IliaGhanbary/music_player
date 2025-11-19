#ifndef HELPER_H
#define HELPER_H


#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <vector>
using namespace std;

struct Song {
    string name;
    string artist;
    
    Song(string n, string a) : name(n), artist(a) {}
    
    string getAllInfo();
};


struct Node {
    Song* song;
    Node* next;
    Node* prev;
    
    Node(Song* s) : song(s), next(nullptr), prev(nullptr) {}
};


#endif