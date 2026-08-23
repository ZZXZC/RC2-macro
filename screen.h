#pragma once

#include <windows.h>
#include <string>
#include <vector>

class Screen {
public:
    static HWND findRobloxWindow();
    static HWND findRobloxWindowByTitle();

    static bool getWindowRect(HWND hwnd, RECT& rect);
    static bool getClientRect(HWND hwnd, RECT& rect);

    // Capture a region of the screen into a DIB section
    // Returns a buffer of BGRA pixels (4 bytes per pixel)
    static std::vector<unsigned char> captureRegion(int x, int y, int width, int height);

    // Read a single pixel color from screen
    static COLORREF getPixel(int x, int y);

    // Read pixel as RGB values
    static void getPixelRgb(int x, int y, int& r, int& g, int& b);

    // Get the screen dimensions
    static int getScreenWidth();
    static int getScreenHeight();

    // Get monitor info
    static int getMonitorCount();
    static std::string getMonitorName(int index);
    static void getMonitorRect(int index, RECT& rect);
};
