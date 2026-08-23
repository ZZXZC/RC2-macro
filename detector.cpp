#include "detector.h"
#include <windows.h>
#include <algorithm>
#include <map>

void Detector::getPixelFromBuffer(const std::vector<unsigned char>& pixels,
                                   int bufWidth, int x, int y,
                                   int& r, int& g, int& b) {
    int idx = (y * bufWidth + x) * 4;
    if (idx >= 0 && idx + 2 < static_cast<int>(pixels.size())) {
        b = pixels[idx];
        g = pixels[idx + 1];
        r = pixels[idx + 2];
    } else {
        r = g = b = 0;
    }
}

bool Detector::detectCastColor(int pixelX, int pixelY,
                                const std::string& targetHex, int tolerance) {
    int tr, tg, tb;
    hexToRgb(targetHex, tr, tg, tb);

    HDC hdc = GetDC(nullptr);
    COLORREF color = GetPixel(hdc, pixelX, pixelY);
    ReleaseDC(nullptr, hdc);

    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);

    return colorMatch(r, g, b, tr, tg, tb, tolerance);
}

bool Detector::isBarVisible(const std::vector<unsigned char>& pixels,
                             int regionWidth, int regionHeight,
                             const std::string& barHex,
                             int tolerance) {
    int br, bg, bb;
    hexToRgb(barHex, br, bg, bb);

    int barPixelCount = 0;
    int scanY = regionHeight / 2;

    for (int x = 0; x < regionWidth; x++) {
        int r, g, b;
        getPixelFromBuffer(pixels, regionWidth, x, scanY, r, g, b);
        if (colorMatch(r, g, b, br, bg, bb, tolerance)) {
            barPixelCount++;
        }
    }

    return barPixelCount > 10;
}

BarInfo Detector::detectBar(const std::vector<unsigned char>& pixels,
                             int regionWidth, int regionHeight,
                             const std::string& barHex,
                             const std::string& lineHex,
                             int tolerance,
                             double centerWindowPct,
                             int contactPadding) {
    BarInfo info = {};
    info.found = false;

    int br, bg, bb;
    hexToRgb(barHex, br, bg, bb);
    int lr, lg, lb;
    hexToRgb(lineHex, lr, lg, lb);

    int scanRows[3];
    scanRows[0] = regionHeight / 2;
    scanRows[1] = regionHeight / 2 - 1;
    scanRows[2] = regionHeight / 2 + 1;
    int numRows = 3;
    if (regionHeight <= 2) {
        scanRows[0] = 0;
        numRows = 1;
    }

    int barLeft = regionWidth;
    int barRight = 0;
    int barPixelCount = 0;

    for (int row = 0; row < numRows; row++) {
        int y = scanRows[row];
        if (y < 0 || y >= regionHeight) continue;

        for (int x = 0; x < regionWidth; x++) {
            int r, g, b;
            getPixelFromBuffer(pixels, regionWidth, x, y, r, g, b);
            if (colorMatch(r, g, b, br, bg, bb, tolerance)) {
                if (x < barLeft) barLeft = x;
                if (x > barRight) barRight = x;
                barPixelCount++;
            }
        }
    }

    if (barPixelCount < 5) {
        return info;
    }

    info.found = true;
    info.barLeftX = barLeft;
    info.barRightX = barRight;
    info.barWidth = barRight - barLeft;
    info.barCenterX = (barLeft + barRight) / 2;

    int lineX = -1;
    int lineBrightness = 0;

    for (int row = 0; row < numRows; row++) {
        int y = scanRows[row];
        if (y < 0 || y >= regionHeight) continue;

        for (int x = barLeft; x <= barRight; x++) {
            int r, g, b;
            getPixelFromBuffer(pixels, regionWidth, x, y, r, g, b);

            if (colorMatch(r, g, b, lr, lg, lb, tolerance)) {
                int brightness = r + g + b;
                if (brightness > lineBrightness) {
                    lineBrightness = brightness;
                    lineX = x;
                }
            }
        }
    }

    if (lineX < 0) {
        for (int row = 0; row < numRows; row++) {
            int y = scanRows[row];
            if (y < 0 || y >= regionHeight) continue;

            for (int x = 0; x < regionWidth; x++) {
                int r, g, b;
                getPixelFromBuffer(pixels, regionWidth, x, y, r, g, b);
                if (colorMatch(r, g, b, lr, lg, lb, tolerance)) {
                    int brightness = r + g + b;
                    if (brightness > lineBrightness) {
                        lineBrightness = brightness;
                        lineX = x;
                    }
                }
            }
        }
    }

    info.lineX = lineX;

    if (lineX >= 0) {
        info.distance = abs(lineX - info.barCenterX);
        int hitZoneHalfWidth = static_cast<int>((info.barWidth * centerWindowPct / 100.0) / 2.0);
        hitZoneHalfWidth += contactPadding;
        info.inHitZone = (info.distance <= hitZoneHalfWidth);
    } else {
        info.distance = -1;
        info.inHitZone = false;
    }

    return info;
}

void Detector::findDominantColor(const std::vector<unsigned char>& pixels,
                                  int regionWidth, int regionHeight,
                                  int& outR, int& outG, int& outB) {
    std::map<int, int> colorCounts;
    std::map<int, std::tuple<int,int,int>> colorMap;

    for (int y = 0; y < regionHeight; y++) {
        for (int x = 0; x < regionWidth; x++) {
            int r, g, b;
            getPixelFromBuffer(pixels, regionWidth, x, y, r, g, b);

            if (r + g + b < 60) continue;

            int qr = r / 8 * 8;
            int qg = g / 8 * 8;
            int qb = b / 8 * 8;
            int key = (qr << 16) | (qg << 8) | qb;

            colorCounts[key]++;
            colorMap[key] = std::make_tuple(r, g, b);
        }
    }

    int maxCount = 0;
    int maxKey = 0;
    for (std::map<int,int>::iterator it = colorCounts.begin(); it != colorCounts.end(); ++it) {
        if (it->second > maxCount) {
            maxCount = it->second;
            maxKey = it->first;
        }
    }

    if (colorMap.count(maxKey)) {
        std::tuple<int,int,int>& clr = colorMap[maxKey];
        outR = std::get<0>(clr);
        outG = std::get<1>(clr);
        outB = std::get<2>(clr);
    } else {
        outR = outG = outB = 128;
    }
}
