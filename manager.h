#pragma once
#include <iostream>
#include <cstring>
#include <limits>
#include <iomanip>
#include <algorithm>

using namespace std;

// ================================
// I. VẬT TƯ (AVL TREE)
// ================================
struct VATTU {
    char MAVT[11];
    char TENVT[51];
    char DVT[11];
    int SoLuongTon;
    int height = 1;
};

struct nodeVT {
    VATTU vt;
    nodeVT *left = NULL;
    nodeVT *right = NULL;
};
typedef nodeVT* treeVT;


// ================================
// II. CHI TIẾT HÓA ĐƠN
// ================================
struct CT_HOADON {
    char MAVT[11];
    int SoLuong;
    float DonGia;
    float VAT;
};

struct nodeCTHD {
    CT_HOADON cthd;
    nodeCTHD* next = NULL;
};
typedef nodeCTHD* PTRCTHD;


// ================================
// III. HÓA ĐƠN (NỘI BỘ)
// ================================
struct NGAY {
    int ngay;
    int thang;
    int nam;
};

struct HOADON {
    char SoHD[21];
    NGAY NgayLap;
    char Loai;     // 'X' hoặc 'N'
    PTRCTHD dscthd = NULL;
};

struct nodeHD {
    HOADON hd;
    nodeHD* next = NULL;     // Dùng cho danh sách hóa đơn của NHÂN VIÊN
};
typedef nodeHD* PTRHD;


// ================================
// IV. DANH SÁCH HÓA ĐƠN TOÀN CÔNG TY
// ================================
struct HD_NODE {
    PTRHD pHD;          // trỏ về nodeHD thật
    HD_NODE* next = NULL;
};
typedef HD_NODE* PTRHD_NODE;


// ================================
// V. NHÂN VIÊN (MẢNG CON TRỎ)
// ================================
const int MAX_NV = 500;

struct NHANVIEN {
    char MANV[11];
    char HO[31];
    char TEN[21];
    char PHAI[4];
    PTRHD dshd = NULL;      // danh sách hóa đơn của từng nhân viên
};

struct DS_NHANVIEN {
    int n = 0;
    NHANVIEN* nodes[MAX_NV];
};


// ================================
// VI. STRUCT TẠM CHO THỐNG KÊ TOP 10
// ================================
struct VT_TK {
    char MAVT[11];
    float doanhThu;
};


// ================================
// VII. KHAI BÁO HÀM
// ================================

// INPUT & VALIDATION
void clearCinBuffer();
bool isAlpha(const char* s);
bool isNumeric(const char* s);
bool isAlphaNumeric(const char* s);
bool isMixedAlphaNumeric(const char* s);

bool readSafeString(const char* prompt, char* buffer, size_t size);
void getValidatedStringInput(const char* prompt, char* out,
                             size_t maxLen,
                             bool(*validator)(const char*),
                             const char* err);
void getValidatedFloatInput(const char* prompt, float& out, const char* err);
void getValidatedIntInput(const char* prompt, int& out, const char* err);
bool confirmAction(const char* prompt);

// INIT + FREE
void initListNhanVien(DS_NHANVIEN &dsnv);
void giaiPhongTreeVatTu(treeVT &root);
void giaiPhongListChiTietHoaDon(PTRCTHD &head);
void giaiPhongListHoaDon(PTRHD &head);
void giaiPhongListNhanVien(DS_NHANVIEN &dsnv);

// AVL & VẬT TƯ
treeVT timVatTu(treeVT root, const char MAVT[]);
treeVT themVatTu(treeVT root, const VATTU &vtMoi, bool &success);
treeVT xoaVatTu(treeVT root, const char MAVT[], bool &success);
void inOrderVatTu(treeVT root);
void menuVatTu(treeVT &root, DS_NHANVIEN &dsnv);

// NHÂN VIÊN
void themNhanVien(DS_NHANVIEN &dsnv);
void xoaNhanVien(DS_NHANVIEN &dsnv);
void hieuChinhNhanVien(DS_NHANVIEN &dsnv);
void inDSNhanVien(DS_NHANVIEN &dsnv);
void menuNhanVien(DS_NHANVIEN &dsnv, treeVT &root);

// TÌM NV
int timNhanVien(DS_NHANVIEN &dsnv, const char MANV[]);

// HÓA ĐƠN
int soSanhNgay(NGAY a, NGAY b);

void chenHD_Tong(HD_NODE* &first, PTRHD hd);

bool checkDuplicateSoHD(DS_NHANVIEN &dsnv, const char SoHD[]);

void xoaCTHDTamThoi(PTRHD &pHD);

void themCTHoaDon(PTRHD &pHD, treeVT &rootVT, char loai);

void themHoaDon(DS_NHANVIEN &dsnv, treeVT &rootVT, HD_NODE* &firstHD);

PTRHD timHoaDonTheoSo(DS_NHANVIEN &dsnv, const char SoHD[], int &idx);

void inMotHoaDon(PTRHD pHD, NHANVIEN* nv, treeVT rootVT);

void inHoaDonTheoSo(DS_NHANVIEN &dsnv, treeVT rootVT);

bool isNgayTrongKhoang(NGAY d, NGAY from, NGAY to);

float tinhTriGiaHoaDon(PTRHD pHD);

void inHoaDonKhoangNgay(DS_NHANVIEN &dsnv, treeVT rootVT, HD_NODE* firstHD);

void duyetAVL_LuuVT(treeVT root, VT_TK arr[], int &n);

void heapify(VT_TK a[], int n, int i);
void buildMaxHeap(VT_TK a[], int n);
void inTop10(VT_TK a[], int n);

void thongKeTop10_TheoKhoang(treeVT rootVT, DS_NHANVIEN &dsnv, HD_NODE* firstHD);

void thongKeDoanhThuTheoThang(DS_NHANVIEN &dsnv, treeVT rootVT, HD_NODE* firstHD);

void menuHoaDon(DS_NHANVIEN &dsnv, treeVT &rootVT, HD_NODE* &firstHD);
