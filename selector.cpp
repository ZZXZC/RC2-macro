#include "selector.h"
#include <windowsx.h>

HWND Selector::overlayHwnd = nullptr;
bool Selector::selecting = false;
bool Selector::dragging = false;
int Selector::startX = 0, Selector::startY = 0;
int Selector::endX = 0, Selector::endY = 0;
RegionCallback Selector::onComplete = nullptr;
HINSTANCE Selector::hInst = nullptr;

LRESULT CALLBACK Selector::overlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN: {
        dragging = true;
        startX = GET_X_LPARAM(lParam);
        startY = GET_Y_LPARAM(lParam);
        endX = startX;
        endY = startY;
        SetCapture(hwnd);
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (dragging) {
            endX = GET_X_LPARAM(lParam);
            endY = GET_Y_LPARAM(lParam);
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (dragging) {
            dragging = false;
            ReleaseCapture();

            // Normalize coordinates
            int x1 = std::min(startX, endX);
            int y1 = std::min(startY, endY);
            int x2 = std::max(startX, endX);
            int y2 = std::max(startY, endY);

            // Convert from overlay coords to screen coords
            POINT topLeft = {x1, y1};
            POINT botRight = {x2, y2};
            ClientToScreen(hwnd, &topLeft);
            ClientToScreen(hwnd, &botRight);

            selecting = false;
            ShowWindow(hwnd, SW_HIDE);

            if (onComplete) {
                onComplete(topLeft.x, topLeft.y, botRight.x, botRight.y);
            }
        }
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Semi-transparent dark overlay
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        RECT rc;
        GetClientRect(hwnd, &rc);
        // Draw with alpha using a memory DC
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        SelectObject(memDC, memBmp);

        // Fill with semi-transparent black
        HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(memDC, &rc, bgBrush);
        DeleteObject(bgBrush);

        if (dragging) {
            // Draw selection rectangle (clear area)
            int x1 = std::min(startX, endX);
            int y1 = std::min(startY, endY);
            int x2 = std::max(startX, endX);
            int y2 = std::max(startY, endY);

            // Draw border around selection
            HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
            HPEN oldPen = (HPEN)SelectObject(memDC, pen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, GetStockObject(NULL_BRUSH));
            Rectangle(memDC, x1, y1, x2, y2);
            SelectObject(memDC, oldPen);
            SelectObject(memDC, oldBrush);
            DeleteObject(pen);

            // Draw instructions
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(255, 255, 255));
            TextOutA(memDC, x1, y1 - 20, "Release to confirm selection", 28);
        } else {
            // Show instructions
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(255, 255, 255));
            TextOutA(memDC, rc.right / 2 - 150, rc.bottom / 2 - 10,
                     "Click and drag to select the fishing bar region", 48);
        }

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
        DeleteObject(memBmp);
        DeleteDC(memDC);
        DeleteObject(brush);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_KEYDOWN: {
        if (wParam == VK_ESCAPE) {
            selecting = false;
            dragging = false;
            ReleaseCapture();
            ShowWindow(hwnd, SW_HIDE);
        }
        return 0;
    }

    case WM_DESTROY:
        selecting = false;
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

HWND Selector::startSelection(HINSTANCE hInstance, RegionCallback callback) {
    hInst = hInstance;
    onComplete = callback;

    // Register overlay window class
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = overlayProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wc.lpszClassName = "RC2FishSelector";

    static bool registered = false;
    if (!registered) {
        RegisterClassExA(&wc);
        registered = true;
    }

    // Create fullscreen transparent overlay
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    if (!overlayHwnd) {
        overlayHwnd = CreateWindowExA(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
            "RC2FishSelector", "Select Region",
            WS_POPUP,
            0, 0, screenW, screenH,
            nullptr, nullptr, hInstance, nullptr
        );

        // Make overlay semi-transparent
        SetLayeredWindowAttributes(overlayHwnd, 0, 80, LWA_ALPHA);
    }

    selecting = true;
    ShowWindow(overlayHwnd, SW_SHOW);
    SetForegroundWindow(overlayHwnd);

    return overlayHwnd;
}

void Selector::stopSelection() {
    selecting = false;
    dragging = false;
    if (overlayHwnd) {
        ShowWindow(overlayHwnd, SW_HIDE);
    }
}

bool Selector::isSelecting() {
    return selecting;
}
