#include "Helper.h"

string Song :: getAllInfo(){

    return "name: " + name +" ,artist: "+ artist;
}

void printHeader(const string& title) {
    cout << "\n" << Color::BOLD << Color::CYAN;
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║ " << left << setw(38) << title << " ║\n";
    cout << "╚════════════════════════════════════════╝" << Color::RESET << "\n\n";
}

void printDivider() {
    cout << Color::DIM << "──────────────────────────────────────────" << Color::RESET << "\n";
}

void printMenuItem(const string& key, const string& label, const string& color ) {
    cout << Color::YELLOW << " [" << key << "] " << Color::RESET << color << label << Color::RESET << "\n";
}

void printSongItem(int num, const string& info) {
    cout << Color::GREEN << " " << setw(2) << num << ". " << Color::RESET << info << "\n";
}