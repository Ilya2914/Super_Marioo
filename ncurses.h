#pragma once

#include <ncurses.h>
#define VK_ESCAPE 27
#define VK_LEFT   KEY_LEFT
#define VK_RIGHT  KEY_RIGHT
#define VK_UP     KEY_UP
#define VK_DOWN   KEY_DOWN
#define VK_SPACE  ' '

// Клавиши
int GetKeyState(int key, int ch);

// Курсор
void SetCursorPosition(int x, int y);

// Инициализация и завершение консоли
void InitConsole();
void CloseConsole();
void Sleep(int a);
