#include "manager.h"
#include <iostream>
using namespace std;

int main() {

    // ============================
    // 1. KHỞI TẠO CẤU TRÚC DỮ LIỆU
    // ============================
    treeVT rootVT = NULL;      // Cây vật tư
    DS_NHANVIEN dsnv;          // Danh sách nhân viên
    initListNhanVien(dsnv);    // Khởi tạo mảng nhân viên (n=0)

    HD_NODE* firstHD = NULL;   // Danh sách HĐ toàn công ty (sorted)

    cout << "Khoi tao du lieu moi (Chua co chuc nang luu/tai).\n";

    // ============================
    // 2. MENU CHÍNH
    // ============================
    int choice;

    do {
        cout << "\n==================================================\n";
        cout << "        CHAO MUNG DEN CHUONG TRINH QUAN LY\n";
        cout << "==================================================\n";
        cout << "1. Quan ly Vat Tu\n";
        cout << "2. Quan ly Nhan Vien\n";
        cout << "3. Quan ly Hoa Don & Bao Cao\n";
        cout << "0. Thoat chuong trinh\n";
        cout << "--------------------------------------------------\n";

        getValidatedIntInput("Lua chon cua ban: ", choice, "Lua chon phai la so!");

        switch (choice) {

        case 1:
            menuVatTu(rootVT, dsnv);
            break;

        case 2:
            menuNhanVien(dsnv, rootVT);
            break;

        case 3:
            menuHoaDon(dsnv, rootVT, firstHD);
            break;

        case 0:
            cout << "\nDang thoat chuong trinh...\n";
            break;

        default:
            cout << "Lua chon khong hop le! Vui long chon lai.\n";
        }

    } while (choice != 0);


    // ============================
    // 3. GIẢI PHÓNG BỘ NHỚ
    // ============================

    giaiPhongTreeVatTu(rootVT);
    giaiPhongListNhanVien(dsnv);

    // giải phóng danh sách hóa đơn toàn công ty
    HD_NODE* p = firstHD;
    while (p) {
        HD_NODE* temp = p;
        p = p->next;
        delete temp;
    }

    cout << "Da thoat chuong trinh.\n";
    return 0;
}