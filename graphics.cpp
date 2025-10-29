#include "graphics.h"
#include <iostream>
// 218 là dấu góc trên trái
//  196 là dấu _
//  191 dấu góc trên phải
//   179 dấu |
// 192 dấu L 217 dấu L ngược

void setColor(int textColor, int bgColor) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, bgColor * 16 + textColor);
}

void gotoxy(int x, int y) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(h, pos);
}

void drawBox(int x, int y, int width, int height, int borderColorAttr) {
    setColor(borderColorAttr);
    gotoxy(x, y);
    cout << char(218); 
    for (int i = 0; i < width - 2; i++) cout << char(196);
    cout << char(191);

    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        cout << char(179);
        gotoxy(x + width - 1, y + i);
        cout << char(179);
    }

    gotoxy(x, y + height - 1);
    cout << char(192);
    for (int i = 0; i < width - 2; i++) cout << char(196);
    cout << char(217);
    setColor(7); 
}

void fillRectColor(int x, int y, int width, int height, WORD textColor, WORD bgColor) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    DWORD written;
    WORD attr = bgColor * 16 + textColor;
    for (int r = 0; r < height; ++r) {
        pos = { (SHORT)x, (SHORT)(y + r) };
        FillConsoleOutputAttribute(h, attr, width, pos, &written);
        FillConsoleOutputCharacter(h, ' ', width, pos, &written);
    }
}
void printCentered(int x, int y, int width, const string &s, WORD textColor, WORD bgColor) {
    int start = x + (width - (int)s.size()) / 2;
    setColor(bgColor * 16 + textColor);
    gotoxy(start, y);
    cout << s;
    setColor(7);
}

void clearScreen() {
    system("cls");
}
