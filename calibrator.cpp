#include "calibrator.h"
#include "screen.h"
#include "detector.h"
#include <sstream>

std::string Calibrator::lastResult = "";

std::string Calibrator::getLastCalibrationResult() {
    return lastResult;
}

std::string rgbToHex(int r, int g, int b) {
    char buf[8];
    snprintf(buf, sizeof(buf), "#%02x%02x%02x", r, g, b);
    return std::string(buf);
}

bool Calibrator::autoDetectBarColor(const Config& cfg, std::string& outBarHex) {
    if (!cfg.regionSelected) {
        lastResult = "No region selected. Press F2 first.";
        return false;
    }

    int w = cfg.regionX2 - cfg.regionX1;
    int h = cfg.regionY2 - cfg.regionY1;
    if (w <= 0 || h <= 0) {
        lastResult = "Invalid region dimensions.";
        return false;
    }

    auto pixels = Screen::captureRegion(cfg.regionX1, cfg.regionY1, w, h);

    int r, g, b;
    Detector::findDominantColor(pixels, w, h, r, g, b);

    outBarHex = rgbToHex(r, g, b);

    std::ostringstream ss;
    ss << "Detected bar color: " << outBarHex
       << " (R=" << r << " G=" << g << " B=" << b << ")";
    lastResult = ss.str();

    return true;
}

bool Calibrator::autoDetectLineColor(const Config& cfg, std::string& outLineHex) {
    if (!cfg.regionSelected) {
        lastResult = "No region selected. Press F2 first.";
        return false;
    }

    int w = cfg.regionX2 - cfg.regionX1;
    int h = cfg.regionY2 - cfg.regionY1;
    if (w <= 0 || h <= 0) {
        lastResult = "Invalid region dimensions.";
        return false;
    }

    auto pixels = Screen::captureRegion(cfg.regionX1, cfg.regionY1, w, h);

    // Find the brightest pixel (likely the white line)
    int brightestR = 0, brightestG = 0, brightestB = 0;
    int maxBrightness = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            if (idx + 2 >= static_cast<int>(pixels.size())) continue;

            int b = pixels[idx];
            int g = pixels[idx + 1];
            int r = pixels[idx + 2];

            int brightness = r + g + b;
            // Only consider near-white pixels
            if (brightness > maxBrightness && r > 200 && g > 200 && b > 200) {
                maxBrightness = brightness;
                brightestR = r;
                brightestG = g;
                brightestB = b;
            }
        }
    }

    if (maxBrightness < 600) {
        lastResult = "No bright line detected. Make sure the minigame is active.";
        return false;
    }

    outLineHex = rgbToHex(brightestR, brightestG, brightestB);

    std::ostringstream ss;
    ss << "Detected line color: " << outLineHex
       << " (R=" << brightestR << " G=" << brightestG
       << " B=" << brightestB << ")";
    lastResult = ss.str();

    return true;
}

bool Calibrator::calibrate(Config& cfg) {
    std::string barHex, lineHex;
    bool barOk = autoDetectBarColor(cfg, barHex);
    bool lineOk = autoDetectLineColor(cfg, lineHex);

    if (barOk) {
        cfg.barColor = barHex;
    }
    if (lineOk) {
        cfg.targetLineColor = lineHex;
    }

    std::ostringstream ss;
    if (barOk && lineOk) {
        ss << "Calibration complete!\n"
           << "Bar: " << barHex << "\n"
           << "Line: " << lineHex;
    } else {
        ss << "Partial calibration:\n";
        if (barOk) ss << "Bar: " << barHex << "\n";
        else ss << "Bar: FAILED - " << lastResult << "\n";
        if (lineOk) ss << "Line: " << lineHex << "\n";
        else ss << "Line: FAILED - Make sure minigame is active\n";
    }

    lastResult = ss.str();
    return barOk && lineOk;
}
