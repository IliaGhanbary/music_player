#ifndef HELPER_H
#define HELPER_H


#include <iostream>
#include <stack>
#include <queue>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;


namespace Color {
    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string DIM = "\033[2m";
    
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    
    const string BG_BLUE = "\033[44m";
    const string BG_MAGENTA = "\033[45m";
}


void printHeader(const string& title) ;

void printDivider() ;

void printMenuItem(const string& key, const string& label, const string& color = Color::WHITE) ;

void printSongItem(int num, const string& info) ;


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