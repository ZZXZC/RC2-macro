#pragma once

#include "config.h"
#include <string>

class Calibrator {
public:
    // Auto-detect the bar color from the selected region
    // Captures the region and finds the dominant non-background color
    static bool autoDetectBarColor(const Config& cfg, std::string& outBarHex);

    // Auto-detect the target line color (white/bright pixels in the bar region)
    static bool autoDetectLineColor(const Config& cfg, std::string& outLineHex);

    // Run full calibration
    static bool calibrate(Config& cfg);

    // Get a description of what was detected
    static std::string getLastCalibrationResult();

private:
    static std::string lastResult;
};
