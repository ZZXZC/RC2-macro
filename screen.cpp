#include "screen.h"
#include <string>
#include <vector>

// Static callback for EnumWindows
static BOOL CALLBACK enumWindowsCallback(HWND hwnd, LPARAM lParam) {
    HWND* result = reinterpret_cast<HWND*>(lParam);
    wchar_t buf[256];
    int len = GetWindowTextW(hwnd, buf, 256);
    if (len > 0) {
        std::wstring title(buf, len);
        for (size_t i = 0; i < title.size(); i++) {
            title[i] = towupper(title[i]);
        }
        if (title.find(L"ROBLOX") != std::wstring::npos) {
            if (IsWindowVisible(hwnd)) {
                *result = hwnd;
                return FALSE;
            }
        }
    }
    return TRUE;
}

HWND Screen::findRobloxWindow() {
    HWND hwnd = nullptr;

    const wchar_t* titles[] = {
        L"Roblox",
        L"Roblox Player",
        L"Roblox Game",
        L"REFINERY CAVES 2",
    };

    for (int i = 0; i < 4; i++) {
        hwnd = FindWindowW(nullptr, titles[i]);
        if (hwnd) return hwnd;
    }

    EnumWindows(enumWindowsCallback, reinterpret_cast<LPARAM>(&hwnd));
    return hwnd;
}

HWND Screen::findRobloxWindowByTitle() {
    return findRobloxWindow();
}

bool Screen::getWindowRect(HWND hwnd, RECT& rect) {
    return ::GetWindowRect(hwnd, &rect) != FALSE;
}

bool Screen::getClientRect(HWND hwnd, RECT& rect) {
    return ::GetClientRect(hwnd, &rect) != FALSE;
}

std::vector<unsigned char> Screen::captureRegion(int x, int y, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 4, 0);

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, x, y, SRCCOPY);

    BITMAPINFOHEADER bi = {};
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    GetDIBits(hdcMem, hBitmap, 0, height, pixels.data(),
              reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS);

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    return pixels;
}

COLORREF Screen::getPixel(int x, int y) {
    HDC hdc = GetDC(nullptr);
    COLORREF color = GetPixel(hdc, x, y);
    ReleaseDC(nullptr, hdc);
    return color;
}

void Screen::getPixelRgb(int x, int y, int& r, int& g, int& b) {
    COLORREF color = getPixel(x, y);
    r = GetRValue(color);
    g = GetGValue(color);
    b = GetBValue(color);
}

int Screen::getScreenWidth() {
    return GetSystemMetrics(SM_CXSCREEN);
}

int Screen::getScreenHeight() {
    return GetSystemMetrics(SM_CYSCREEN);
}

struct MonitorEnumData {
    std::vector<HMONITOR> monitors;
    std::vector<RECT> rects;
    std::vector<std::string> names;
};

static BOOL CALLBACK monitorEnumProc(HMONITOR hMon, HDC hdcMon, LPRECT lprcMon, LPARAM dwData) {
    MonitorEnumData* data = reinterpret_cast<MonitorEnumData*>(dwData);
    data->monitors.push_back(hMon);
    data->rects.push_back(*lprcMon);

    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(hMon, &mi);
    wchar_t wname[256];
    wcscpy(wname, mi.szDevice);
    std::string name(wname, wname + wcslen(wname));
    data->names.push_back(name);

    return TRUE;
}

int Screen::getMonitorCount() {
    MonitorEnumData data;
    EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&data));
    return static_cast<int>(data.monitors.size());
}

std::string Screen::getMonitorName(int index) {
    MonitorEnumData data;
    EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&data));
    if (index >= 0 && index < static_cast<int>(data.names.size())) {
        return data.names[index];
    }
    return "";
}

void Screen::getMonitorRect(int index, RECT& rect) {
    MonitorEnumData data;
    EnumDisplayMonitors(nullptr, nullptr, monitorEnumProc, reinterpret_cast<LPARAM>(&data));
    if (index >= 0 && index < static_cast<int>(data.rects.size())) {
        rect = data.rects[index];
    } else {
        rect.left = 0;
        rect.top = 0;
        rect.right = getScreenWidth();
        rect.bottom = getScreenHeight();
    }
}
