#include "ncurses.h"


int GetKeyState(int key, int ch) {
    return (ch == key) ? -1 : 0;
}

void SetCursorPosition(int x, int y) {
    move(y, x);  
    refresh();
}

void InitConsole() {
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
}
void Sleep(int a){
    napms(a);
}
void CloseConsole() {
    endwin();
}

