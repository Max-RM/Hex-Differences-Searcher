// ArtemSU 2024

#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
namespace fs = std::filesystem;

#pragma comment(lib, "comctl32.lib")

const std::string programName = "HEX Differences Searcher 1.0.0";
const std::string info = "Developed by ArtemSU. 2024 (c) TNT ENTERTAINMENT inc.";

class GetFiles {
private:
    HWND hwnd;
    HWND fileE1, fileE2, startB, infoText;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void Browse(int entryNum);
    void Start();
    int Go(const std::string& origPath, const std::string& newPath);

public:
    GetFiles();
    void Run();
};

GetFiles::GetFiles() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"GetFilesClass";
    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0, L"GetFilesClass", L"HEX Differences Searcher 1.0.0",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, GetModuleHandle(NULL), this
    );

    CreateWindow(L"STATIC", L"Path 1:", WS_VISIBLE | WS_CHILD, 10, 10, 50, 20, hwnd, NULL, NULL, NULL);
    fileE1 = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 70, 10, 600, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"BUTTON", L"BROWSE", WS_VISIBLE | WS_CHILD, 680, 10, 100, 20, hwnd, (HMENU)1, NULL, NULL);

    CreateWindow(L"STATIC", L"Path 2:", WS_VISIBLE | WS_CHILD, 10, 40, 50, 20, hwnd, NULL, NULL, NULL);
    fileE2 = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 70, 40, 600, 20, hwnd, NULL, NULL, NULL);
    CreateWindow(L"BUTTON", L"BROWSE", WS_VISIBLE | WS_CHILD, 680, 40, 100, 20, hwnd, (HMENU)2, NULL, NULL);

    startB = CreateWindow(L"BUTTON", L"START", WS_VISIBLE | WS_CHILD, 10, 70, 770, 50, hwnd, (HMENU)3, NULL, NULL);
    infoText = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL, 10, 130, 770, 400, hwnd, NULL, NULL, NULL);

    CreateWindow(L"STATIC", std::wstring(info.begin(), info.end()).c_str(), WS_VISIBLE | WS_CHILD, 10, 540, 770, 20, hwnd, NULL, NULL, NULL);
}

void GetFiles::Run() {
    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK GetFiles::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    GetFiles* pThis = NULL;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (GetFiles*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
    }
    else {
        pThis = (GetFiles*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pThis) {
        switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case 1:
                pThis->Browse(1);
                break;
            case 2:
                pThis->Browse(2);
                break;
            case 3:
                pThis->Start();
                break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GetFiles::Browse(int entryNum) {
    OPENFILENAME ofn = {};
    wchar_t szFile[260] = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        SetWindowText(entryNum == 1 ? fileE1 : fileE2, szFile);
    }
}

void GetFiles::Start() {
    wchar_t oneFile[260], twoFile[260];
    GetWindowText(fileE1, oneFile, 260);
    GetWindowText(fileE2, twoFile, 260);

    if (wcslen(oneFile) == 0 || wcslen(twoFile) == 0) {
        SendMessage(infoText, EM_SETSEL, -1, -1);
        SendMessage(infoText, EM_REPLACESEL, 0, (LPARAM)L"ERROR: Select 2 files.\r\n");
        return;
    }

    std::wstring message = L"Files:\r\nPath 1: " + std::wstring(oneFile) + L"\r\nPath 2: " + std::wstring(twoFile) + L"\r\n";
    SendMessage(infoText, EM_SETSEL, -1, -1);
    SendMessage(infoText, EM_REPLACESEL, 0, (LPARAM)message.c_str());

    SendMessage(infoText, EM_SETSEL, -1, -1);
    SendMessage(infoText, EM_REPLACESEL, 0, (LPARAM)L"The program has started working.\r\n");

    int o = Go(std::string(oneFile, oneFile + wcslen(oneFile)), std::string(twoFile, twoFile + wcslen(twoFile)));

    std::vector<std::wstring> messages = {
        L"The work is completed.\r\n",
        L"The program log is written to file.\r\n",
        L"Total differences found: " + std::to_wstring(o) + L"\r\n"
    };

    for (const auto& msg : messages) {
        SendMessage(infoText, EM_SETSEL, -1, -1);
        SendMessage(infoText, EM_REPLACESEL, 0, (LPARAM)msg.c_str());
    }

    if (o == 0) {
        SendMessage(infoText, EM_SETSEL, -1, -1);
        SendMessage(infoText, EM_REPLACESEL, 0, (LPARAM)L"NO DIFFERENCES WERE FOUND. THE FILES ARE THE SAME.\r\n");
    }
}
int GetFiles::Go(const std::string& origPath, const std::string& newPath) {
    std::vector<std::string> strings;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    tm timeInfo;
    localtime_s(&timeInfo, &in_time_t);
    ss << std::put_time(&timeInfo, "%Y-%m-%d %X");

    strings.push_back(programName + "\n");
    strings.push_back("Time: " + ss.str() + "\n");
    strings.push_back("File 1: " + origPath + "\n");
    strings.push_back("Size of File 1: " + std::to_string(fs::file_size(origPath)) + "\n");
    strings.push_back("File 2: " + newPath + "\n");
    strings.push_back("Size of File 2: " + std::to_string(fs::file_size(newPath)) + "\n");
    strings.push_back("\nSEARCH FOR DIFFERENCES...\n");

    std::ifstream origFile(origPath, std::ios::binary);
    std::ifstream newFile(newPath, std::ios::binary);

    int o = 0;
    char origByte, newByte;
    for (int i = 0; origFile.get(origByte) && newFile.get(newByte); ++i) {
        if (origByte != newByte) {
            std::stringstream hexSS;
            hexSS << "0x" << std::uppercase << std::hex << i;
            std::string hexStr = hexSS.str();

            std::stringstream diffSS;
            diffSS << hexStr << " >> "
                << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)origByte
                << " >> "
                << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)newByte;

            strings.push_back(diffSS.str() + "\n");
            ++o;
        }
    }

    if (o == 0) {
        strings.push_back("NO DIFFERENCES WERE FOUND. THE FILES ARE THE SAME.");
    }
    else {
        strings.push_back(std::to_string(o) + " DIFFERENCES FOUND.");
    }

    std::string logFile = "HDS-Logs_" + ss.str() + ".txt";
    std::replace(logFile.begin(), logFile.end(), ':', '-');

    std::ofstream outFile(logFile);
    for (const auto& str : strings) {
        outFile << str;
    }
    outFile.close();

    ShellExecute(NULL, L"open", std::wstring(logFile.begin(), logFile.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);

    return o;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GetFiles app;
    app.Run();
    return 0;
}


//This C++ code uses the Windows API for the GUI part as requested.It implements the same functionality as the Python code, creating a window with file selection inputs, a start button, and an info text area.The file comparison logic is also implemented similarly.

//Note that this code requires Windows - specific headers and libraries, and should be compiled with a Windows C++ compiler(like MSVC).Also, make sure to link against the necessary Windows libraries(user32.lib, gdi32.lib, comctl32.lib, comdlg32.lib, shell32.lib).
