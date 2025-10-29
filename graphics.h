#pragma once
#include <windows.h>
#include <string>
using namespace std;

void setColor(int textColor, int bgColor = 0);
void gotoxy(int x, int y);
void drawBox(int x, int y, int width, int height, int borderColorAttr);
void fillRectColor(int x, int y, int width, int height, WORD textColor, WORD bgColor);
void printCentered(int x, int y, int width, const string &s, WORD textColor, WORD bgColor);
void clearScreen();