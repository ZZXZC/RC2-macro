#pragma once

#include <windows.h>
#include <functional>

// Callback when region selection is complete
using RegionCallback = std::function<void(int x1, int y1, int x2, int y2)>;

class Selector {
public:
    // Start the transparent overlay for region selection
    // Returns the HWND of the overlay window
    static HWND startSelection(HINSTANCE hInstance, RegionCallback callback);

    // Stop and destroy the overlay
    static void stopSelection();

    // Is selection currently active?
    static bool isSelecting();

private:
    static LRESULT CALLBACK overlayProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static HWND overlayHwnd;
    static bool selecting;
    static bool dragging;
    static int startX, startY;
    static int endX, endY;
    static RegionCallback onComplete;
    static HINSTANCE hInst;
};
