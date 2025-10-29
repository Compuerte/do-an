	
#include "mainMenu.h"
#include "graphics.h"

#include <iostream>
using namespace std;

void drawMainUI() {
    clearScreen();
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursor;
    GetConsoleCursorInfo(h, &cursor);
    cursor.bVisible = FALSE;
    SetConsoleCursorInfo(h, &cursor);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(h, &info);
    int cols = info.dwSize.X;
    int rows = info.dwSize.Y;
    // Kích thước cửa sổ chính (để có lề 1)
    int winX = 2, winY = 1;
    int winW = cols - 4;
    int winH = rows - 3;

    // vẽ khung
    drawBox(winX, winY, winW, winH, 11); // viền màu (màu sáng)

    // header: chiều cao header = 3
    int headerH = 3;
    fillRectColor(winX + 1, winY + 1, winW - 2, headerH - 1, 15, 1); // chữ trắng, nền xanh dương
    printCentered(winX + 1, winY + 1, winW - 2, "QUAN LY VAT TU - CONSOLE UI", 15, 1);

    // menu trái: width 28
    int menuW = 28;
    int contentX = winX + menuW + 1;
    int contentW = (winX + winW - 1) - contentX;

    // tô vùng menu nền đậm
    fillRectColor(winX + 1, winY + headerH, menuW, winH - headerH - 1, 15, 4); // chữ trắng, nền đỏ (thay số màu nếu muốn)
    // in các mục menu
    setColor(15);
    gotoxy(winX + 3, winY + headerH + 1); cout << "1. Danh sach vat tu";
    gotoxy(winX + 3, winY + headerH + 3); cout << "2. Nhap kho";
    gotoxy(winX + 3, winY + headerH + 5); cout << "3. Xuat kho";
    gotoxy(winX + 3, winY + headerH + 7); cout << "4. Bao cao";
    gotoxy(winX + 3, winY + headerH + 9); cout << "0. Thoat";

    // vùng nội dung (giữ nền trắng)
    fillRectColor(contentX, winY + headerH, contentW, winH - headerH - 1, 0, 15); // chữ đen, nền trắng
    printCentered(contentX + 1, winY + headerH + 1, contentW - 2, "Thong tin se hien thi o day", 0, 15);

    // đặt con trỏ trở về vị trí hợp lý (ở menu)
    gotoxy(winX + 3, winY + headerH + 12);
    setColor(7);
    // hiện con trỏ lại nếu cần (ở đây mình tắt, bạn có thể bật)
    // cursor.bVisible = TRUE; SetConsoleCursorInfo(h,&cursor);

    // tạm dừng để bạn xem
    gotoxy(0, rows - 1);
    cout << "Nhan phim bat ky de tiep tuc...";
    system("pause>nul");
    
    
}
