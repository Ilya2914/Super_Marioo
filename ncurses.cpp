#include "ncurses.h"


int GetKeyState(int key) {
    static bool initialized = false;
    if (!initialized) {
        initscr();
        //cbreak();
        noecho();
        //keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE); 
        initialized = true;
    }
    int ch = getch();
    return (ch == key) ? -1 : 0;
}

void SetCursorPosition(int x, int y) {
    move(y, x);  
    refresh();
}

void InitConsole() {
    initscr();
    //cbreak();
    noecho();
    //keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
}
void Sleep(int a){
    napms(a);
}
void CloseConsole() {
    endwin();
}

