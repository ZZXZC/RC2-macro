#pragma once

#include "config.h"
#include <vector>

struct BarInfo {
    bool found;
    int barCenterX;      // Center X of the blue bar
    int barLeftX;        // Left edge of blue bar
    int barRightX;       // Right edge of blue bar
    int barWidth;        // Width of blue bar
    int lineX;           // X position of the white moving line
    int distance;        // Distance from line to bar center
    bool inHitZone;      // Is line within the center window?
};

class Detector {
public:
    // Check if the perfect-cast green pixel is visible
    static bool detectCastColor(int pixelX, int pixelY,
                                const std::string& targetHex, int tolerance);

    // Detect the bar minigame elements in a thin horizontal strip
    // pixels: raw BGRA pixel data from captureRegion
    // regionWidth, regionHeight: dimensions of captured region
    static BarInfo detectBar(const std::vector<unsigned char>& pixels,
                             int regionWidth, int regionHeight,
                             const std::string& barHex,
                             const std::string& lineHex,
                             int tolerance,
                             double centerWindowPct,
                             int contactPadding);

    // Check if the blue bar is visible at all (minigame started)
    static bool isBarVisible(const std::vector<unsigned char>& pixels,
                             int regionWidth, int regionHeight,
                             const std::string& barHex,
                             int tolerance);

    // Find the most common color in a region (for calibration)
    static void findDominantColor(const std::vector<unsigned char>& pixels,
                                  int regionWidth, int regionHeight,
                                  int& outR, int& outG, int& outB);

private:
    // Get pixel color from BGRA buffer (x, y are region-local)
    static void getPixelFromBuffer(const std::vector<unsigned char>& pixels,
                                   int bufWidth, int x, int y,
                                   int& r, int& g, int& b);
};
