#pragma once

#include <string>

struct Config {
    // Hotkeys
    int hotkeyStart    = 0x7A; // F1
    int hotkeyRegion   = 0x7B; // F2
    int hotkeyExit     = 0x7C; // F3
    int hotkeyPause    = 0x7D; // F4
    int hotkeyCalibrate= 0x7E; // F5

    // GUI
    bool alwaysOnTop   = true;
    bool autoMinimize  = false;
    bool autoFocus     = true;
    bool autoMaximize  = false;

    // Monitor
    int monitorIndex   = 0;

    // Cast detection
    std::string castTargetColor = "#0af809";
    int castTolerance   = 10;
    int castScanEveryMs = 10;
    double castTimeoutS = 10.0;
    double recastDelayS = 0.75;
    bool dismissPopup   = true;
    double dismissDelayS= 0.25;

    // Bar minigame detection
    std::string barColor       = "#628aef";
    std::string targetLineColor= "#ffffff";
    int barTolerance    = 30;
    int barScanEveryMs  = 5;
    int clickCooldownMs = 50;
    int contactPaddingPx= 3;
    double centerWindowPct = 35.0;
    double barTimeoutS  = 45.0;
    double endWaitS     = 1.25;

    // Region selection (set by user)
    bool regionSelected = false;
    int regionX1 = 0, regionY1 = 0;
    int regionX2 = 0, regionY2 = 0;

    // Cast region (set by user)
    bool castRegionSelected = false;
    int castPixelX = 0, castPixelY = 0;

    // Runtime state
    bool running  = false;
    bool paused   = false;
};

// Hex color to RGB
inline void hexToRgb(const std::string& hex, int& r, int& g, int& b) {
    std::string h = hex;
    if (h[0] == '#') h = h.substr(1);
    r = std::stoi(h.substr(0, 2), nullptr, 16);
    g = std::stoi(h.substr(2, 2), nullptr, 16);
    b = std::stoi(h.substr(4, 2), nullptr, 16);
}

// Check if two RGB colors are within tolerance
inline bool colorMatch(int r1, int g1, int b1, int r2, int g2, int b2, int tolerance) {
    return abs(r1 - r2) <= tolerance &&
           abs(g1 - g2) <= tolerance &&
           abs(b1 - b2) <= tolerance;
}
