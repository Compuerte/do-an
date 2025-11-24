#include "manager.h"
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <limits>


using namespace std;

// ===================================================
// I. TRIỂN KHAI VALIDATION & INPUT
// ===================================================

void clearCinBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// --- Hàm kiểm tra ---
bool isAlpha(const char* str) {
    if (!str || str[0] == '\0') return false;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (!isalpha((unsigned char)str[i]) && str[i] != ' ') return false;
    }
    return true;
}

bool isNumeric(const char* str) {
    if (!str || str[0] == '\0') return false;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (!isdigit((unsigned char)str[i])) return false;
    }
    return true;
}

bool isAlphaNumeric(const char* str) {
    if (!str || str[0] == '\0') return false;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (!isalnum((unsigned char)str[i])) return false;
    }
    return true;
}
bool isMixedAlphaNumeric(const char* str) {
    if (!str || str[0] == '\0') return false;
    
    // Kiểm tra độ dài
    if (strlen(str) < 2) return false; 

    bool hasAlpha = false;
    bool hasDigit = false;

    for (int i = 0; str[i] != '\0'; ++i) {
        if (!isalnum((unsigned char)str[i])) {
            return false; 
        }

        if (isalpha((unsigned char)str[i])) {
            hasAlpha = true;
        }
        if (isdigit((unsigned char)str[i])) {
            hasDigit = true;
        }
    }
    return hasAlpha && hasDigit; 
}
NGAY getCurrentDate() {
    time_t t = time(0);
    tm *ltm = localtime(&t);
    
    NGAY n;
    n.ngay = ltm->tm_mday;
    n.thang = 1 + ltm->tm_mon;
    n.nam = 1900 + ltm->tm_year;
    return n;
}

// Hàm hỗ trợ đọc chuỗi an toàn
// Trả về true nếu đọc thành công (không overflow), false nếu overflow (bị cắt)
bool readSafeString(const char* prompt, char* buffer, size_t bufferSize)
{
    if (prompt) 
        cout << prompt;

    if (fgets(buffer, (int)bufferSize, stdin) == NULL)
        return false;

    // Xóa '\n'
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
    else
    {
        // Nếu không có '\n' → buffer bị tràn → dọn phần còn lại
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    return true;
}


// Hàm nhập chuỗi đã xác thực
void getValidatedStringInput(const char* prompt, char* output, size_t maxLen,
                             bool (*validator)(const char*), const char* err)
{
    char buf[256];

    while (true)
    {
        readSafeString(prompt, buf, sizeof(buf));

        if (strlen(buf) == 0)
        {
            cout << "Khong duoc de trong.\n";
            continue;
        }

        if (strlen(buf) > maxLen)
        {
            cout << "Vuot qua chieu dai cho phep (" << maxLen << ").\n";
            continue;
        }

        if (validator && !validator(buf))
        {
            cout << err << "\n";
            continue;
        }

        strcpy(output, buf);
        return;
    }
}


// Hàm nhập số nguyên đã xác thực
void getValidatedIntInput(const char* prompt, int &output, const char* err)
{
    char buf[64];

    while (true)
    {
        readSafeString(prompt, buf, sizeof(buf));

        bool ok = true;
        for (int i = 0; buf[i]; i++)
        {
            if (!isdigit(buf[i]))
            {
                ok = false;
                break;
            }
        }

        if (!ok)
        {
            cout << err << "\n";
            continue;
        }

        output = atoi(buf);
        return;
    }
}

void getValidatedFloatInput(const char* prompt, float &output, const char* err)
{
    char buf[64];
    while (true)
    {
        readSafeString(prompt, buf, sizeof(buf));

        bool ok = true;
        bool hasDot = false;

        for (int i = 0; buf[i]; i++)
        {
            if (buf[i] == '.')
            {
                if (hasDot) { ok = false; break; }
                hasDot = true;
            }
            else if (!isdigit(buf[i]))
            {
                ok = false;
                break;
            }
        }

        if (!ok)
        {
            cout << err << "\n";
            continue;
        }

        output = atof(buf);
        return;
    }
}


bool confirmAction(const char* prompt) {
    char choice[16]; 
    while (true) {
        if (prompt && strlen(prompt) > 0) cout << prompt;
        // Sử dụng readSafeString để đảm bảo input an toàn
        bool ok = readSafeString("", choice, sizeof(choice));
        if (!ok) {
            // overflow -> invalid
            cout << "Lua chon khong hop le. Vui long nhap 'Y' (Yes) hoac 'N' (No).\n";
            continue;
        }
        
        // Chuyển về chữ hoa để so sánh dễ dàng
        for (int i = 0; choice[i]; i++) {
            choice[i] = (char)toupper((unsigned char)choice[i]);
        }
        
        if (strcmp(choice, "Y") == 0 || strcmp(choice, "YES") == 0) {
            return true;
        } else if (strcmp(choice, "N") == 0 || strcmp(choice, "NO") == 0) {
            return false;
        } else {
            cout << "Lua chon khong hop le. Vui long nhap 'Y' (Yes) hoac 'N' (No).\n";
        }
    }
}

// ===================================================
// II. TRIỂN KHAI VẬT TƯ (AVL Tree)
// ===================================================

// --- AVL Primitives ---
int getHeight(treeVT root) { 
    return (root == NULL) ? 0 : root->vt.height; 
}

void updateHeight(treeVT root) { 
    if (root != NULL) root->vt.height = 1 + max(getHeight(root->left), getHeight(root->right)); 
}

int getBalanceFactor(treeVT root) { 
    return (root == NULL) ? 0 : getHeight(root->left) - getHeight(root->right); 
}

treeVT rotateLeft(treeVT A) { 
    treeVT B = A->right; 
    A->right = B->left; 
    B->left = A; 
    updateHeight(A); 
    updateHeight(B); 
    return B; 
}

treeVT rotateRight(treeVT A) { 
    treeVT B = A->left; 
    A->left = B->right; 
    B->right = A; 
    updateHeight(A); 
    updateHeight(B); 
    return B; 
}

treeVT canBang(treeVT root) {
    if (root == NULL) return root;
    updateHeight(root);
    int balance = getBalanceFactor(root);

    if (balance > 1) { 
        if (getBalanceFactor(root->left) >= 0) return rotateRight(root);
        else { root->left = rotateLeft(root->left); return rotateRight(root); }
    }
    if (balance < -1) { 
        if (getBalanceFactor(root->right) <= 0) return rotateLeft(root);
        else { root->right = rotateRight(root->right); return rotateLeft(root); }
    }
    return root; 
}

// --- CRUD Vật Tư ---
treeVT timVatTu(treeVT root, const char MAVT[]) {
    if (root == NULL) return NULL;
    int cmp = strcmp(MAVT, root->vt.MAVT);
    if (cmp < 0) return timVatTu(root->left, MAVT);
    else if (cmp > 0) return timVatTu(root->right, MAVT);
    else return root;
}

treeVT themVatTu(treeVT root, const VATTU& vtMoi, bool& success) {
    if (root == NULL) {
        success = true;
        nodeVT* newNode = new nodeVT();
        newNode->vt = vtMoi;
        return newNode;
    }
    int cmp = strcmp(vtMoi.MAVT, root->vt.MAVT);
    
    if (cmp < 0) root->left = themVatTu(root->left, vtMoi, success);
    else if (cmp > 0) root->right = themVatTu(root->right, vtMoi, success);
    else {
        cout << "Loi: Ma Vat Tu da ton tai.\n";
        success = false;
        return root;
    }
    return canBang(root);
}

treeVT minValueNode(treeVT node) {
    treeVT current = node;
    while (current && current->left != NULL) current = current->left;
    return current;
}

treeVT xoaVatTu(treeVT root, const char MAVT[], bool& success) {
    if (root == NULL) { success = false; return root; }

    int cmp = strcmp(MAVT, root->vt.MAVT);
    
    if (cmp < 0) root->left = xoaVatTu(root->left, MAVT, success);
    else if (cmp > 0) root->right = xoaVatTu(root->right, MAVT, success);
    else {
        success = true;
        if ((root->left == NULL) || (root->right == NULL)) { 
            treeVT temp = root->left ? root->left : root->right;
            if (temp == NULL) { temp = root; root = NULL; } 
            else { *root = *temp; }
            delete temp;
        } else { 
            treeVT temp = minValueNode(root->right);
            root->vt = temp->vt; 
            root->right = xoaVatTu(root->right, temp->vt.MAVT, success);
        }
    }
    if (root == NULL) return root;
    return canBang(root);
}

void hieuChinhVatTu(treeVT root) {
    char maVTCanSua[11];
    getValidatedStringInput("Nhap Ma Vat Tu can hieu chinh (max 10): ", maVTCanSua, 10, isAlphaNumeric, "Ma chi duoc chua chu va so");

    treeVT p = timVatTu(root, maVTCanSua);
    if (p == NULL) { 
        cout << "Loi: Khong tim thay vat tu.\n"; 
        return; 
    }
    
    cout << "\n--- XAC NHAN HIEU CHINH ---\n";
    cout << "Ban co muon hieu chinh Vat Tu co Ma " << p->vt.MAVT << " khong? (Y/N): ";
    if (!confirmAction("")) {
        cout << "Da huy thao tac hieu chinh.\n";
        return;
    }
    
    int choice;
    do {
        cout << "\n--- HIEU CHINH VAT TU (Ma: " << p->vt.MAVT << ") ---\n";
        cout << "1. Hieu chinh Ten Vat Tu (Hien tai: " << p->vt.TENVT << ")\n";
        cout << "2. Hieu chinh Don Vi Tinh (Hien tai: " << p->vt.DVT << ")\n";
        cout << "3. Hieu chinh CA HAI truong\n"; 
        cout << "0. Quay lai\n";
        
        getValidatedIntInput("Lua chon truong can hieu chinh: ", choice, "Lua chon phai la so");

        switch (choice) {
            case 1: { // Chỉnh Ten VT
                getValidatedStringInput("Nhap Ten VT moi (max 50): ", p->vt.TENVT, 50, isAlpha, "Ten chi duoc chua chu cai va khoang trang");
                cout << "Hieu chinh Ten Vat Tu thanh cong.\n";
                choice = 0;
                break;
            }
            case 2: { // Chỉnh DVT
                getValidatedStringInput("Nhap DVT moi (max 10): ", p->vt.DVT, 10, isAlpha, "Don vi tinh chi duoc chua chu cai");
                cout << "Hieu chinh Don Vi Tinh thanh cong.\n";
                choice = 0;
                break;
            }
            case 3: { // Chỉnh Cả Hai
                getValidatedStringInput("Nhap Ten VT moi (max 50): ", p->vt.TENVT, 50, isAlpha, "Ten chi duoc chua chu cai va khoang trang");
                getValidatedStringInput("Nhap DVT moi (max 10): ", p->vt.DVT, 10, isAlpha, "Don vi tinh chi duoc chua chu cai");
                cout << "Hieu chinh CA HAI truong thanh cong.\n";
                choice = 0;
                break;
            }
            case 0:
                break;
            default:
                cout << "Lua chon khong hop le.\n";
        }
    } while (choice != 0);
}
bool isVatTuInAnyHoaDon(DS_NHANVIEN& dsnv, const char MAVT[]) {
    for (int i = 0; i < dsnv.n; ++i) { 
        PTRHD pHD = dsnv.nodes[i]->dshd;
        while (pHD != NULL) { 
            PTRCTHD pCTHD = pHD->hd.dscthd;
            while (pCTHD != NULL) { 
                if (strcmp(pCTHD->cthd.MAVT, MAVT) == 0) {
                    return true;
                }
                pCTHD = pCTHD->next;
            }
            pHD = pHD->next;
        }
    }
    return false;
}

void inOrderVatTu(treeVT root) {
    if (root == NULL) return;
    inOrderVatTu(root->left);
    cout << root->vt.MAVT << "\t" << root->vt.TENVT << "\t" 
         << root->vt.DVT << "\t" << root->vt.SoLuongTon << endl;
    inOrderVatTu(root->right);
}

void giaiPhongTreeVatTu(treeVT& root) {
    if (root == NULL) return;
    giaiPhongTreeVatTu(root->left);
    giaiPhongTreeVatTu(root->right);
    delete root;
    root = NULL;
}


void menuVatTu(treeVT& rootVT, DS_NHANVIEN& dsnv) {
    int choice;
    do {
        cout << "\n-- Quan ly Vat Tu --\n";
        cout << "1. Them Vat Tu\n";
        cout << "2. Hieu chinh Vat Tu\n";
        cout << "3. Xoa Vat Tu\n";
        cout << "4. In danh sach Vat Tu (Theo Ma VT)\n";
        cout << "0. Quay lai\n";
        
        getValidatedIntInput("Lua chon: ", choice, "Lua chon phai la so");

        switch (choice) {
            case 1: {
                VATTU vtMoi;
                bool success = false;
                
                while (true) {
                    getValidatedStringInput("Nhap Ma Vat Tu (max 10): ", vtMoi.MAVT, 10, isAlphaNumeric, "Ma chi duoc chua chu va so");
                    for (int i = 0; vtMoi.MAVT[i]; i++) { vtMoi.MAVT[i] = toupper((unsigned char)vtMoi.MAVT[i]); }
                    
                    if (!isMixedAlphaNumeric(vtMoi.MAVT)) {
                        cout << "=> LOI: Ma VT phai chua ca ky tu chu va ky tu so (vd: XM01).\n";
                        continue;
                    }

                    if (timVatTu(rootVT, vtMoi.MAVT) != NULL) {
                        cout << "Loi: Ma VT da ton tai. Vui long nhap ma khac.\n";
                    } else {
                        break;
                    }
                }
                
                getValidatedStringInput("Nhap Ten Vat Tu (max 50): ", vtMoi.TENVT, 50, isAlpha, "Ten chi duoc chua chu cai va khoang trang");
                getValidatedStringInput("Nhap Don Vi Tinh (max 10): ", vtMoi.DVT, 10, isAlpha, "Don vi tinh chi duoc chua chu cai");
                getValidatedIntInput("Nhap So Luong Ton ban dau (>= 0): ",vtMoi.SoLuongTon, "So luong phai la so nguyen khong am");
                
                rootVT = themVatTu(rootVT, vtMoi, success);
                if(success) cout << "Them vat tu thanh cong.\n";
                break;
            }
            case 2: {
                hieuChinhVatTu(rootVT);
                break;
            }
            case 3: {
                char maVTCanXoa[11];
                bool success = false;
                getValidatedStringInput("Nhap Ma Vat Tu can xoa (max 10): ", maVTCanXoa, 10, isAlphaNumeric, "Ma chi duoc chua chu va so");
                
                cout << "Ban co chac chan muon XOA Vat Tu co Ma " << maVTCanXoa << " khong? (Y/N): ";
                if (!confirmAction("")) {
                    cout << "Da huy thao tac xoa.\n";
                    break;
                }
                
                if (isVatTuInAnyHoaDon(dsnv, maVTCanXoa)) {
                    cout << "Loi: Khong the xoa Vat Tu da co trong Hoa Don.\n";
                    break;
                }
                
                rootVT = xoaVatTu(rootVT, maVTCanXoa, success);
                if(success) cout << "Xoa vat tu thanh cong.\n";
                else cout << "Loi: Khong tim thay ma vat tu.\n";
                break;
            }
            case 4: 
                cout << "\n--- DANH SACH VAT TU (THEO MA VT) ---\n";
                cout << "MA VT\tTEN VAT TU\tDVT\tTON KHO\n";
                cout << "---------------------------------------\n";
                if (rootVT == NULL) {
                    cout << "Danh sach rong.\n";
                } else {
                    inOrderVatTu(rootVT);
                }
                cout << "---------------------------------------\n";
                break;
            case 0:
                break;
            default:
                cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }
    } while (choice != 0);
}


// ===================================================
// II. TRIỂN KHAI NHÂN VIÊN/HÓA ĐƠN (Hàm hỗ trợ)
// ===================================================

void initListNhanVien(DS_NHANVIEN &dsnv) {
    dsnv.n = 0;
}

void giaiPhongListChiTietHoaDon(PTRCTHD& head) {
    PTRCTHD current = head;
    while(current != NULL) {
        PTRCTHD next = current->next;
        delete current;
        current = next;
    }
    head = NULL;
}

void giaiPhongListHoaDon(PTRHD& head) {
    PTRHD current = head;
    while(current != NULL) {
        PTRHD next = current->next;
        giaiPhongListChiTietHoaDon(current->hd.dscthd);
        delete current;
        current = next;
    }
    head = NULL;
}

void giaiPhongListNhanVien(DS_NHANVIEN& dsnv) {
    for (int i = 0; i < dsnv.n; ++i) {
        giaiPhongListHoaDon(dsnv.nodes[i]->dshd);
        delete dsnv.nodes[i];
    }
    dsnv.n = 0;
}


// III. TRIỂN KHAI MODULE NHÂN VIÊN (Mảng con trỏ)
// ===================================================

// Tìm kiếm nhân viên theo MANV (Trả về index hoặc -1)
int timNhanVien(DS_NHANVIEN& dsnv, const char MANV[]) {
    for (int i = 0; i < dsnv.n; ++i) {
        if (strcmp(dsnv.nodes[i]->MANV, MANV) == 0) {
            return i; 
        }
    }
    return -1;
}


// Sắp xếp danh sách nhân viên theo TÊN (tăng dần) rồi đến HỌ (tăng dần)
// Trả về < 0 nếu p1 nhỏ hơn p2 (cần đứng trước), > 0 nếu ngược lại, = 0 nếu bằng nhau.
int compareNhanVien(NHANVIEN *p1, NHANVIEN *p2) {
    int cmpTen = strcmp(p1->TEN, p2->TEN);
    if (cmpTen != 0) {
        return cmpTen;
    }
    return strcmp(p1->HO, p2->HO);
}
int partition(DS_NHANVIEN& dsnv, int low, int high) {
    NHANVIEN *pivot = dsnv.nodes[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (compareNhanVien(dsnv.nodes[j], pivot) <= 0) { 
            i++; 
            swap(dsnv.nodes[i], dsnv.nodes[j]);
        }
    }
    swap(dsnv.nodes[i + 1], dsnv.nodes[high]);
    return (i + 1);
}
void quickSortNhanVienRecursive(DS_NHANVIEN& dsnv, int low, int high) {
    if (low < high) {
        int pi = partition(dsnv, low, high); 
        quickSortNhanVienRecursive(dsnv, low, pi - 1);
        quickSortNhanVienRecursive(dsnv, pi + 1, high);
    }
}

void sapXepNhanVien(DS_NHANVIEN& dsnv) {
    if (dsnv.n <= 1) return;
    quickSortNhanVienRecursive(dsnv, 0, dsnv.n - 1);
}

void themNhanVien(DS_NHANVIEN& dsnv) {
    if (dsnv.n >= MAX_NV) {
        cout << "Loi: Danh sach da day (" << MAX_NV << " nhan vien).\n";
        return;
    }

    NHANVIEN *nvMoi = new NHANVIEN();
    
    while (true) {
        getValidatedStringInput("Nhap Ma NV (max 10, chu so): ", nvMoi->MANV, 10, isAlphaNumeric, "Ma chi duoc chua chu va so");
        for (int i = 0; nvMoi->MANV[i]; i++) { nvMoi->MANV[i] = toupper((unsigned char)nvMoi->MANV[i]); }
        
        if (!isMixedAlphaNumeric(nvMoi->MANV)) {
            cout << "=> LOI: Ma NV phai chua ca ky tu chu va ky tu so (vd: NV01). Vui long nhap lai.\n";
            continue; 
        }

        if (timNhanVien(dsnv, nvMoi->MANV) == -1) {
            break;
        }
        cout << "Loi: Ma NV da ton tai. Vui long nhap lai.\n";
    }

    getValidatedStringInput("Nhap Ho (max 30, chu cai): ", nvMoi->HO, 30, isAlpha, "Ho chi duoc chua chu cai va khoang trang");
    getValidatedStringInput("Nhap Ten (max 20, chu cai): ", nvMoi->TEN, 20, isAlpha, "Ten chi duoc chua chu cai");
    
    char phai[8];
    while(true) {
        getValidatedStringInput("Nhap Phai (Nam/Nu): ", phai, 7, isAlpha, "Phai chi duoc chua chu cai");
        for (int i = 0; phai[i]; i++) { phai[i] = toupper((unsigned char)phai[i]); }
        if (strcmp(phai, "NAM") == 0 || strcmp(phai, "NU") == 0) {
            strcpy(nvMoi->PHAI, phai);
            break;
        }
        cout << "Loi: Phai chi duoc nhap 'Nam' hoac 'Nu'. Vui long nhap lai.\n";
    }
    
    nvMoi->dshd = NULL;
    dsnv.nodes[dsnv.n] = nvMoi;
    dsnv.n++;
    sapXepNhanVien(dsnv);
    
    cout << "Them nhan vien " << nvMoi->TEN << " thanh cong. Tong so NV: " << dsnv.n << "\n";
}

void xoaNhanVien(DS_NHANVIEN& dsnv) {
    if (dsnv.n == 0) {
        cout << "Danh sach nhan vien rong.\n";
        return;
    }
    char maNVCanXoa[11];
    getValidatedStringInput("Nhap Ma NV can xoa (max 10): ", maNVCanXoa, 10, isAlphaNumeric, "Ma chi duoc chua chu va so");
    
    int index = timNhanVien(dsnv, maNVCanXoa);
    if (index == -1) {
        cout << "Loi: Khong tim thay nhan vien co Ma " << maNVCanXoa << ".\n";
        return;
    }
    
    NHANVIEN* nv = dsnv.nodes[index];
    
    if (nv->dshd != NULL) {
        cout << "Loi: Khong the xoa nhan vien da lap hoa don.\n";
        return;
    }
    
    cout << "\n--- XAC NHAN XOA ---\n";
    cout << "Ban co chac chan muon XOA NV: " << nv->HO << " " << nv->TEN << " khong? (Y/N): ";
    if (!confirmAction("")) {
        cout << "Da huy thao tac xoa.\n";
        return;
    }
    
    delete nv; 
    
    for (int i = index; i < dsnv.n - 1; ++i) {
        dsnv.nodes[i] = dsnv.nodes[i + 1];
    }
    dsnv.n--;
    
    cout << "Xoa nhan vien thanh cong. Tong so NV con lai: " << dsnv.n << "\n";
}

void hieuChinhNhanVien(DS_NHANVIEN& dsnv) {
    if (dsnv.n == 0) {
        cout << "Danh sach nhan vien rong.\n";
        return;
    }
    char maNVCanSua[11];
    getValidatedStringInput("Nhap Ma NV can hieu chinh (max 10): ", maNVCanSua, 10, isAlphaNumeric, "Ma chi duoc chua chu va so");
    
    int index = timNhanVien(dsnv, maNVCanSua);
    if (index == -1) {
        cout << "Loi: Khong tim thay nhan vien.\n";
        return;
    }
    
    NHANVIEN* nv = dsnv.nodes[index];

    cout << "\n--- XAC NHAN HIEU CHINH ---\n";
    cout << "Ban co muon hieu chinh NV: " << nv->HO << " " << nv->TEN << " khong? (Y/N): ";
    if (!confirmAction("")) {
        cout << "Da huy thao tac hieu chinh.\n";
        return;
    }

    int choice;
    do {
        cout << "\n--- HIEU CHINH NV (Ma: " << nv->MANV << ") ---\n";
        cout << "1. Hieu chinh Ho (Hien tai: " << nv->HO << ")\n";
        cout << "2. Hieu chinh Ten (Hien tai: " << nv->TEN << ")\n";
        cout << "3. Hieu chinh Phai (Hien tai: " << nv->PHAI << ")\n";
        cout << "0. Hoan tat hieu chinh\n";
        
        getValidatedIntInput("Lua chon truong can hieu chinh: ", choice, "Lua chon phai la so");

        switch (choice) {
            case 1: { 
                getValidatedStringInput("Nhap Ho moi (max 30): ", nv->HO, 30, isAlpha, "Ho chi duoc chua chu cai va khoang trang");
                sapXepNhanVien(dsnv);
                cout << "Hieu chinh Ho thanh cong.\n";
                break;
            }
            case 2: { 
                getValidatedStringInput("Nhap Ten moi (max 20): ", nv->TEN, 20, isAlpha, "Ten chi duoc chua chu cai");
                sapXepNhanVien(dsnv);
                cout << "Hieu chinh Ten thanh cong.\n";
                break;
            }
            case 3: { 
                char phai[8];
                while(true) {
                    getValidatedStringInput("Nhap Phai moi (Nam/Nu): ", phai, 7, isAlpha, "Phai chi duoc chua chu cai");
                    for (int i = 0; phai[i]; i++) { phai[i] = toupper((unsigned char)phai[i]); }
                    if (strcmp(phai, "NAM") == 0 || strcmp(phai, "NU") == 0) {
                        strcpy(nv->PHAI, phai);
                        break;
                    }
                    cout << "Loi: Phai chi duoc nhap 'Nam' hoac 'Nu'. Vui long nhap lai.\n";
                }
                cout << "Hieu chinh Phai thanh cong.\n";
                break;
            }
            case 0:
                break;
            default:
                cout << "Lua chon khong hop le.\n";
        }
    } while (choice != 0);
}

void inDSNhanVien(DS_NHANVIEN& dsnv) {
    if (dsnv.n == 0) {
        cout << "Danh sach nhan vien rong.\n";
        return;
    }
    
    sapXepNhanVien(dsnv); 
    
    cout << "\n--- DANH SACH NHAN VIEN (SAP XEP THEO TEN & HO) ---\n";
    cout << "STT | MA NV      | HO VA TEN             | PHAI\n";
    cout << "--------------------------------------------------------\n";
    
    for (int i = 0; i < dsnv.n; ++i) {
        NHANVIEN* nv = dsnv.nodes[i];
        cout << i + 1 << ". ";
        cout << nv->MANV << "\t";
        cout << nv->HO << " " << nv->TEN << "\t";
        cout << nv->PHAI << "\n";
    }
    cout << "--------------------------------------------------------\n";
    cout << "Tong so nhan vien: " << dsnv.n << "\n";
}

void menuNhanVien(DS_NHANVIEN& dsnv, treeVT& rootVT) {
    int choice;
    do {
        cout << "\n-- Quan ly Nhan Vien --\n";
        cout << "1. Them Nhan Vien\n";
        cout << "2. Hieu chinh Nhan Vien\n";
        cout << "3. Xoa Nhan Vien\n";
        cout << "4. In danh sach Nhan Vien (Sap xep theo Ten)\n";
        cout << "0. Quay lai\n";
        
        getValidatedIntInput("Lua chon: ", choice, "Lua chon phai la so");

        switch (choice) {
            case 1: themNhanVien(dsnv); break;
            case 2: hieuChinhNhanVien(dsnv); break;
            case 3: xoaNhanVien(dsnv); break;
            case 4: inDSNhanVien(dsnv); break;
            case 0: break;
            default: cout << "Lua chon khong hop le. Vui long chon lai.\n";
        }
    } while (choice != 0);
}

// IV. MODULE HÓA ĐƠN – DÙNG 2 LINKED LIST:
// 🔹 KIỂM TRA NGÀY
// ===============================
int soSanhNgay(NGAY a, NGAY b) {
    if (a.nam != b.nam) return a.nam - b.nam;
    if (a.thang != b.thang) return a.thang - b.thang;
    return a.ngay - b.ngay;
}

// ===============================
// 🔹 CHÈN HÓA ĐƠN VÀO LIST TOÀN CÔNG TY (SORTED)
// ===============================

void chenHD_Tong(HD_NODE* &first, PTRHD hd) {
    HD_NODE* node = new HD_NODE;
    node->pHD = hd;
    node->next = NULL;

    // list rỗng
    if (first == NULL) {
        first = node;
        return;
    }

    // hd < first
    if (soSanhNgay(hd->hd.NgayLap, first->pHD->hd.NgayLap) < 0) {
        node->next = first;
        first = node;
        return;
    }

    HD_NODE* p = first;
    while (p->next != NULL &&
           soSanhNgay(p->next->pHD->hd.NgayLap, hd->hd.NgayLap) <= 0) {
        p = p->next;
    }

    node->next = p->next;
    p->next = node;
}


// ===============================
// 🔹 KIỂM TRA SỐ HÓA ĐƠN TRÙNG
// ===============================
bool checkDuplicateSoHD(DS_NHANVIEN &dsnv, const char SoHD[]) {
    for (int i = 0; i < dsnv.n; i++) {
        PTRHD p = dsnv.nodes[i]->dshd;
        while (p != NULL) {
            if (strcmp(p->hd.SoHD, SoHD) == 0)
                return true;
            p = p->next;
        }
    }
    return false;
}


// ===============================
// 🔹 XÓA 1 CHI TIẾT TẠM THỜI
// ===============================
void xoaCTHDTamThoi(PTRHD &pHD) {
    if (!pHD || !pHD->hd.dscthd) {
        cout << "Khong co chi tiet.\n";
        return;
    }

    cout << "\n--- DANH SACH CT TAM THOI ---\n";
    PTRCTHD c = pHD->hd.dscthd;
    while (c) {
        cout << c->cthd.MAVT << " - SL: " << c->cthd.SoLuong << endl;
        c = c->next;
    }

    char ma[11];
    readSafeString("Nhap MAVT de xoa: ", ma, 10);
    for (int i=0; ma[i]; i++) ma[i] = toupper(ma[i]);

    PTRCTHD p = pHD->hd.dscthd;
    PTRCTHD prev = NULL;

    while (p) {
        if (strcmp(p->cthd.MAVT, ma) == 0) {
            if (!prev) pHD->hd.dscthd = p->next;
            else prev->next = p->next;

            delete p;
            cout << "Da xoa!\n";
            return;
        }
        prev = p;
        p = p->next;
    }

    cout << "Khong tim thay!\n";
}


// ===============================
// 🔹 NHẬP CHI TIẾT HÓA ĐƠN
// ===============================
void themCTHoaDon(PTRHD &pHD, treeVT &rootVT, char loai) {
    while (true) {
        char maVT[11];
        readSafeString("\nNhap MAVT (STOP/XOA): ", maVT, 10);
        for (int i=0; maVT[i]; i++) maVT[i] = toupper(maVT[i]);

        if (strcmp(maVT, "STOP") == 0) return;
        if (strcmp(maVT, "XOA") == 0) { xoaCTHDTamThoi(pHD); continue; }

        if (!isMixedAlphaNumeric(maVT)) {
            cout << "MAVT sai.\n"; continue;
        }

        treeVT vt = timVatTu(rootVT, maVT);
        if (!vt) {
            cout << "Ma VT khong ton tai.\n"; continue;
        }

        cout << "Ton kho hien tai: " << vt->vt.SoLuongTon << endl;

        int sl;
        getValidatedIntInput("Nhap SL: ", sl, "Sai!");
        if (sl <= 0) continue;

        // kiểm tra SL tạm đã nhập
        int daNhap = 0;
        for (PTRCTHD t = pHD->hd.dscthd; t; t = t->next)
            if (strcmp(t->cthd.MAVT, maVT) == 0)
                daNhap += t->cthd.SoLuong;

        if (loai == 'X' && sl + daNhap > vt->vt.SoLuongTon) {
            cout << "Ton kho khong du!\n"; 
            continue;
        }

        float dg, vat;
        getValidatedFloatInput("Don gia: ", dg, "Sai!");
        getValidatedFloatInput("VAT %: ", vat, "Sai!");

        PTRCTHD node = new nodeCTHD;
        strcpy(node->cthd.MAVT, maVT);
        node->cthd.SoLuong = sl;
        node->cthd.DonGia  = dg;
        node->cthd.VAT     = vat;
        node->next = pHD->hd.dscthd;
        pHD->hd.dscthd = node;

        if (!confirmAction("Nhap tiep? (Y/N): "))
            return;
    }
}


// ===============================
// 🔹 LẬP HÓA ĐƠN
// ===============================
void themHoaDon(DS_NHANVIEN &dsnv, treeVT &rootVT, HD_NODE* &firstHD) {

    char maNV[11];
    int idxNV = -1;

    // ---- NHẬP MÃ NV ----
    while (true) {
        getValidatedStringInput("Nhap MaNV: ", maNV, 10, isAlphaNumeric, "Sai!");
        for (int i=0; i<strlen(maNV); i++) maNV[i] = toupper(maNV[i]);

        idxNV = timNhanVien(dsnv, maNV);
        if (idxNV != -1) break;

        cout << "MaNV khong ton tai!\n";
    }

    // ---- NHẬP SỐ HÓA ĐƠN ----
    char soHD[21];
    while (true) {
        readSafeString("Nhap SoHD: ", soHD, 20);
        for (int i=0; i<strlen(soHD); i++) soHD[i] = toupper(soHD[i]);

        if (!isMixedAlphaNumeric(soHD)) continue;
        if (checkDuplicateSoHD(dsnv, soHD)) continue;

        break;
    }

    // ---- NHẬP LOẠI ----
    char loai = 'X';
    char buf[4];

    while (true) {
        readSafeString("Loai (X/N): ", buf, sizeof(buf));
        if (strlen(buf)==1 && (buf[0]=='X' || buf[0]=='N')) {
            loai = buf[0];
            break;
        }
        cout << "Sai! Chi duoc X hoac N.\n";
    }

    // ---- TẠO NODE HÓA ĐƠN (CHỈ BÂY GIỜ hd MỚI TỒN TẠI!) ----
    PTRHD hd = new nodeHD;
    strcpy(hd->hd.SoHD, soHD);
    hd->hd.Loai = loai;
    hd->hd.dscthd = NULL;
    hd->next = NULL;

    // ---- NGÀY LẬP ----
    cout << "\nNhap ngay lap:\n";
    getValidatedIntInput("Ngay: ", hd->hd.NgayLap.ngay,  "Sai!");
    getValidatedIntInput("Thang:", hd->hd.NgayLap.thang, "Sai!");
    getValidatedIntInput("Nam:  ", hd->hd.NgayLap.nam,   "Sai!");

    // ---- NHẬP CHI TIẾT HÓA ĐƠN ----
    themCTHoaDon(hd, rootVT, loai);

    if (!hd->hd.dscthd) {
        cout << "Khong co chi tiet. Huy HD!\n";
        delete hd;
        return;
    }

    // ---- XÁC NHẬN LƯU ----
    if (!confirmAction("Xac nhan lap HD? (Y/N): ")) {
        giaiPhongListChiTietHoaDon(hd->hd.dscthd);
        delete hd;
        return;
    }

    // ---- GẮN VÀO NHÂN VIÊN ----
    hd->next = dsnv.nodes[idxNV]->dshd;
    dsnv.nodes[idxNV]->dshd = hd;

    // ---- CẬP NHẬT TỒN KHO ----
    for (PTRCTHD ct = hd->hd.dscthd; ct; ct = ct->next) {
        treeVT vt = timVatTu(rootVT, ct->cthd.MAVT);
        if (!vt) continue;

        if (loai=='N') vt->vt.SoLuongTon += ct->cthd.SoLuong;
        else vt->vt.SoLuongTon -= ct->cthd.SoLuong;
    }

    // ---- CHÈN VÀO DANH SÁCH TOÀN CÔNG TY ----
    chenHD_Tong(firstHD, hd);

    cout << "=> LAP HOA DON THANH CONG!\n";
}

// ===============================
// 🔹 TÌM HÓA ĐƠN THEO SỐ
// ===============================
PTRHD timHoaDonTheoSo(DS_NHANVIEN &dsnv, const char SoHD[], int &idxNV) {
    idxNV = -1;
    for (int i = 0; i < dsnv.n; i++) {
        PTRHD p = dsnv.nodes[i]->dshd;
        while (p) {
            if (strcmp(p->hd.SoHD, SoHD) == 0) {
                idxNV = i;
                return p;
            }
            p = p->next;
        }
    }
    return NULL;
}


// ===============================
// 🔹 IN 1 HÓA ĐƠN
// ===============================
void inMotHoaDon(PTRHD pHD, NHANVIEN* nv, treeVT rootVT) {
    if (!pHD) return;

    cout << "\n========== HOA DON ==========\n";
    cout << "SoHD: " << pHD->hd.SoHD << endl;
    cout << "Ngay: "
         << pHD->hd.NgayLap.ngay << "/"
         << pHD->hd.NgayLap.thang << "/"
         << pHD->hd.NgayLap.nam << endl;
    cout << "Loai: " << (pHD->hd.Loai=='X' ? "Xuat" : "Nhap") << endl;
    cout << "Nhan vien lap: " << nv->HO << " " << nv->TEN << endl;

    cout << "---------------------------------------------\n";
    cout << left << setw(10) << "MAVT"
         << setw(10) << "SL"
         << setw(12) << "DonGia"
         << setw(6)  << "VAT"
         << setw(12) << "ThanhTien" << endl;

    float tong = 0;

    for (PTRCTHD ct = pHD->hd.dscthd; ct; ct = ct->next) {
        float tt = ct->cthd.SoLuong *
                   ct->cthd.DonGia *
                   (1 + ct->cthd.VAT/100.0f);

        tong += tt;

        cout << left
             << setw(10) << ct->cthd.MAVT
             << setw(10) << ct->cthd.SoLuong
             << setw(12) << fixed << setprecision(2) << ct->cthd.DonGia
             << setw(6)  << ct->cthd.VAT
             << setw(12) << fixed << setprecision(2) << tt
             << endl;
    }

    cout << "---------------------------------------------\n";
    cout << "Tong: " << fixed << setprecision(2) << tong << endl;
}

// ===============================
// 🔹 IN HÓA ĐƠN THEO SỐ
// ===============================
void inHoaDonTheoSo(DS_NHANVIEN &dsnv, treeVT rootVT) {
    char soHD[21];
    readSafeString("Nhap SoHD: ", soHD, 20);
for (int i=0; soHD[i]; i++)
    soHD[i] = toupper((unsigned char)soHD[i]);

    int idx;
    PTRHD p = timHoaDonTheoSo(dsnv, soHD, idx);

    if (!p) {
        cout << "Khong tim thay hoa don!\n";
        return;
    }

    inMotHoaDon(p, dsnv.nodes[idx], rootVT);
}


// ===============================
// 🔹 KIỂM TRA KHOẢNG NGÀY
// ===============================
bool isNgayTrongKhoang(NGAY d, NGAY from, NGAY to) {
    return soSanhNgay(d, from) >= 0 &&
           soSanhNgay(d, to) <= 0;
}


// ===============================
// 🔹 TÍNH TRỊ GIÁ HÓA ĐƠN
// ===============================
float tinhTriGiaHoaDon(PTRHD p) {
    float sum = 0;
    for (PTRCTHD ct = p->hd.dscthd; ct; ct = ct->next) {
        sum += ct->cthd.SoLuong *
               ct->cthd.DonGia *
               (1 + ct->cthd.VAT/100.0f);
    }
    return sum;
}


// ===============================
// 🔹 IN HÓA ĐƠN TRONG KHOẢNG (G)
// ===============================
void inHoaDonKhoangNgay(DS_NHANVIEN &dsnv, treeVT rootVT, HD_NODE* firstHD) {
    NGAY from, to;

    cout << "\nNhap NGAY BAT DAU:\n";
    getValidatedIntInput("Ngay: ", from.ngay, "Sai!");
    getValidatedIntInput("Thang:", from.thang,"Sai!");
    getValidatedIntInput("Nam:  ", from.nam, "Sai!");

    cout << "\nNhap NGAY KET THUC:\n";
    getValidatedIntInput("Ngay: ", to.ngay, "Sai!");
    getValidatedIntInput("Thang:", to.thang,"Sai!");
    getValidatedIntInput("Nam:  ", to.nam, "Sai!");
     if (soSanhNgay(to, from) < 0) {
        cout << "Loi: Ngay KET THUC phai lon hon hoac bang NGAY BAT DAU!\n";
        return;
    }

    cout << "\n===== HOA DON TRONG KHOANG NGAY =====\n";

    HD_NODE* p = firstHD;
    bool found = false;

    while (p) {
        PTRHD hd = p->pHD;

        if (isNgayTrongKhoang(hd->hd.NgayLap, from, to)) {

            int idx;
            timHoaDonTheoSo(dsnv, hd->hd.SoHD, idx);

            float triGia = tinhTriGiaHoaDon(hd);

            cout << left << setw(12) << hd->hd.SoHD
     << setw(10)
     << (to_string(hd->hd.NgayLap.ngay) + "/" +
         to_string(hd->hd.NgayLap.thang) + "/" +
         to_string(hd->hd.NgayLap.nam))
     << setw(8) << (hd->hd.Loai=='X' ? "Xuat" : "Nhap")
     << setw(20) << (string(dsnv.nodes[idx]->HO) + " " + dsnv.nodes[idx]->TEN)
     << setw(12) << fixed << setprecision(2) << triGia
     << endl;

            found = true;
        }

        p = p->next;
    }

    if (!found) cout << "Khong co hoa don nao trong khoang!\n";
}


// ===============================
// 🔹 TOP 10 (H)
// ===============================
void duyetAVL_LuuVT(treeVT root, VT_TK arr[], int &n) {
    if (!root) return;
    duyetAVL_LuuVT(root->left, arr, n);

    strcpy(arr[n].MAVT, root->vt.MAVT);
    arr[n].doanhThu = 0;
    n++;

    duyetAVL_LuuVT(root->right, arr, n);
}

void heapify(VT_TK a[], int n, int i) {
    int largest = i;
    int L = 2*i, R = 2*i+1;

    if (L <= n && a[L].doanhThu > a[largest].doanhThu) largest = L;
    if (R <= n && a[R].doanhThu > a[largest].doanhThu) largest = R;

    if (largest != i) {
        swap(a[i], a[largest]);
        heapify(a, n, largest);
    }
}

void buildMaxHeap(VT_TK a[], int n) {
    for (int i = n/2; i >= 1; i--)
        heapify(a, n, i);
}

void inTop10(VT_TK a[], int n) {
    cout << "\n===== TOP 10 VAT TU DOANH THU =====\n";
    int k = min(10, n-1);

    for (int i = 1; i <= k; i++) {
        cout << setw(10) << a[1].MAVT
             << setw(15) << fixed << setprecision(2) << a[1].doanhThu << endl;

        swap(a[1], a[n-1]);
        n--;
        heapify(a, n-1, 1);
    }
}

void thongKeTop10_TheoKhoang(treeVT rootVT, DS_NHANVIEN &dsnv, HD_NODE* firstHD) {

    NGAY from, to;

    cout << "\nNhap NGAY BAT DAU:\n";
    getValidatedIntInput("Ngay: ", from.ngay, "Sai!");
    getValidatedIntInput("Thang:", from.thang,"Sai!");
    getValidatedIntInput("Nam:  ", from.nam, "Sai!");

    cout << "\nNhap NGAY KET THUC:\n";
    getValidatedIntInput("Ngay: ", to.ngay, "Sai!");
    getValidatedIntInput("Thang:", to.thang,"Sai!");
    getValidatedIntInput("Nam:  ", to.nam, "Sai!");

    VT_TK arr[500];
    int nVT = 0;
    duyetAVL_LuuVT(rootVT, arr, nVT);

    for (int i=0;i<nVT;i++) arr[i].doanhThu = 0;

    HD_NODE* p = firstHD;

    while (p) {
        PTRHD hd = p->pHD;

        if (hd->hd.Loai=='X' && isNgayTrongKhoang(hd->hd.NgayLap, from, to)) {

            for (PTRCTHD ct = hd->hd.dscthd; ct; ct = ct->next) {

                for (int k=0;k<nVT;k++) {
                    if (strcmp(arr[k].MAVT, ct->cthd.MAVT)==0) {
                        arr[k].doanhThu += ct->cthd.SoLuong *
                                           ct->cthd.DonGia *
                                           (1 + ct->cthd.VAT/100.0f);
                        break;
                    }
                }
            }
        }

        p = p->next;
    }

    // đổi mảng thành dạng heap (index 1-based)
    for (int i=nVT; i>=1; i--)
        arr[i] = arr[i-1];

    nVT++;

    buildMaxHeap(arr, nVT);

    inTop10(arr, nVT);
}


// ===============================
// 🔹 THỐNG KÊ THÁNG TRONG NĂM (I)
// ===============================
void thongKeDoanhThuTheoThang(DS_NHANVIEN &dsnv, treeVT rootVT, HD_NODE* firstHD) {

    int nam;
    getValidatedIntInput("Nhap nam can thong ke: ", nam, "Sai!");

    float dt[13] = {0}; // 1–12

    HD_NODE* p = firstHD;
    while (p) {
        PTRHD hd = p->pHD;

        if (hd->hd.Loai=='X' && hd->hd.NgayLap.nam == nam) {
            int thang = hd->hd.NgayLap.thang;

            dt[thang] += tinhTriGiaHoaDon(hd);
        }

        p = p->next;
    }

    cout << "\n===== DOANH THU NAM " << nam << " =====\n";
    for (int m=1; m<=12; m++) {
        cout << "Thang " << setw(2) << m << ": "
             << fixed << setprecision(2) << dt[m] << endl;
    }
}


// ===============================
// 🔹 MENU HOÁ ĐƠN
// ===============================
void menuHoaDon(DS_NHANVIEN &dsnv, treeVT &rootVT, HD_NODE* &firstHD) {

    int ch;

    do {
        cout << "\n============= MENU HOA DON =============\n";
        cout << "1. Lap hoa don\n";
        cout << "2. In hoa don theo so (f)\n";
        cout << "3. In hoa don theo khoang ngay (g)\n";
        cout << "4. Top 10 theo khoang (h)\n";
        cout << "5. Doanh thu theo thang (i)\n";
        cout << "0. Thoat\n";

        getValidatedIntInput("Chon: ", ch, "Sai!");

        switch (ch) {
            case 1: themHoaDon(dsnv, rootVT, firstHD); break;
            case 2: inHoaDonTheoSo(dsnv, rootVT); break;
            case 3: inHoaDonKhoangNgay(dsnv, rootVT, firstHD); break;
            case 4: thongKeTop10_TheoKhoang(rootVT, dsnv, firstHD); break;
            case 5: thongKeDoanhThuTheoThang(dsnv, rootVT, firstHD); break;
            case 0: cout << "Thoat menu hoa don.\n"; break;
            default: cout << "Sai lua chon!\n";
        }

    } while (ch != 0);
}


