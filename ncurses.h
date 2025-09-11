#pragma once

#include <ncurses.h>
#define VK_ESCAPE 27


// Клавиши
int GetKeyState(int key);

// Курсор
void SetCursorPosition(int x, int y);

// Инициализация и завершение консоли
void InitConsole();
void CloseConsole();
void Sleep(int a);
