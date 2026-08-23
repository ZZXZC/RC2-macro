#include "fisher.h"
#include "screen.h"
#include "detector.h"
#include "input.h"
#include <windows.h>

Fisher::Fisher(Config& config)
    : cfg(config), state(FisherState::IDLE),
      stateStartTime(0), lastScanTime(0), lastClickTime(0),
      lastTickTime(0), barDisappearTime(0), barWasVisible(false),
      clickCooldownMs(50), fishCaught(0), failedCasts(0) {
}

void Fisher::start() {
    cfg.running = true;
    cfg.paused = false;
    fishCaught = 0;
    failedCasts = 0;
    setState(FisherState::CASTING, "Casting...");
}

void Fisher::stop() {
    cfg.running = false;
    setState(FisherState::IDLE, "Stopped");
}

void Fisher::togglePause() {
    cfg.paused = !cfg.paused;
    if (cfg.paused) {
        statusMessage = "Paused";
    } else {
        statusMessage = "Resumed";
    }
}

FisherState Fisher::getState() const { return state; }

std::string Fisher::getStateName() const {
    switch (state) {
        case FisherState::IDLE:               return "IDLE";
        case FisherState::CASTING:            return "CASTING";
        case FisherState::WAITING_FOR_CAST_COLOR: return "WAITING FOR CAST";
        case FisherState::WAITING_FOR_BAR:    return "WAITING FOR BAR";
        case FisherState::MINIGAME_ACTIVE:    return "MINIGAME";
        case FisherState::MINIGAME_DONE:      return "MINIGAME DONE";
        case FisherState::POST_CLICK_1:       return "DISMISSING POPUP";
        case FisherState::POST_CLICK_2:       return "RECASTING";
        case FisherState::FAILED:             return "FAILED";
    }
    return "UNKNOWN";
}

std::string Fisher::getStatusMessage() const { return statusMessage; }
int Fisher::getFishCaught() const { return fishCaught; }
int Fisher::getFailedCasts() const { return failedCasts; }

DWORD Fisher::elapsed() const {
    return GetTickCount() - stateStartTime;
}

void Fisher::setState(FisherState newState, const std::string& msg) {
    state = newState;
    stateStartTime = GetTickCount();
    statusMessage = msg;
    if (onStateChange) {
        onStateChange(newState, msg);
    }
}

void Fisher::update() {
    if (!cfg.running || cfg.paused) return;

    DWORD now = GetTickCount();

    switch (state) {
        case FisherState::IDLE:               handleIdle(); break;
        case FisherState::CASTING:            handleCasting(); break;
        case FisherState::WAITING_FOR_CAST_COLOR: handleWaitingForCastColor(); break;
        case FisherState::WAITING_FOR_BAR:    handleWaitingForBar(); break;
        case FisherState::MINIGAME_ACTIVE:    handleMinigameActive(); break;
        case FisherState::MINIGAME_DONE:      handleMinigameDone(); break;
        case FisherState::POST_CLICK_1:       handlePostClick1(); break;
        case FisherState::POST_CLICK_2:       handlePostClick2(); break;
        case FisherState::FAILED:             handleFailed(); break;
    }

    lastTickTime = now;
}

void Fisher::handleIdle() {
    // Nothing to do
}

void Fisher::handleCasting() {
    // Get center of screen for the click
    int screenW = Screen::getScreenWidth();
    int screenH = Screen::getScreenHeight();

    // Click in the center-bottom area (where water typically is)
    int clickX = screenW / 2;
    int clickY = screenH / 2 + 50;

    Input::leftClickAt(clickX, clickY);
    setState(FisherState::WAITING_FOR_CAST_COLOR, "Waiting for perfect cast...");
}

void Fisher::handleWaitingForCastColor() {
    // Check timeout
    if (elapsed() > static_cast<DWORD>(cfg.castTimeoutS * 1000)) {
        setState(FisherState::FAILED, "Cast timeout - no perfect cast color");
        return;
    }

    // Scan for the green perfect-cast pixel
    if (Detector::detectCastColor(cfg.castPixelX, cfg.castPixelY,
                                   cfg.castTargetColor, cfg.castTolerance)) {
        // Perfect cast detected - click to release
        Input::leftClickAt(cfg.castPixelX, cfg.castPixelY);
        setState(FisherState::WAITING_FOR_BAR, "Cast released! Waiting for bar...");
    }
}

void Fisher::handleWaitingForBar() {
    // Check timeout
    if (elapsed() > static_cast<DWORD>(cfg.barTimeoutS * 1000)) {
        setState(FisherState::FAILED, "Bar timeout - minigame never appeared");
        return;
    }

    if (!cfg.regionSelected) {
        setState(FisherState::FAILED, "No region selected");
        return;
    }

    // Rate limit scanning
    DWORD now = GetTickCount();
    if (now - lastScanTime < static_cast<DWORD>(cfg.barScanEveryMs)) return;
    lastScanTime = now;

    int w = cfg.regionX2 - cfg.regionX1;
    int h = cfg.regionY2 - cfg.regionY1;
    auto pixels = Screen::captureRegion(cfg.regionX1, cfg.regionY1, w, h);

    if (Detector::isBarVisible(pixels, w, h, cfg.barColor, cfg.barTolerance)) {
        setState(FisherState::MINIGAME_ACTIVE, "Bar detected! Playing minigame...");
        barWasVisible = true;
        barDisappearTime = 0;
    }
}

void Fisher::handleMinigameActive() {
    if (!cfg.regionSelected) {
        setState(FisherState::FAILED, "No region selected");
        return;
    }

    // Rate limit scanning
    DWORD now = GetTickCount();
    if (now - lastScanTime < static_cast<DWORD>(cfg.barScanEveryMs)) return;
    lastScanTime = now;

    int w = cfg.regionX2 - cfg.regionX1;
    int h = cfg.regionY2 - cfg.regionY1;
    auto pixels = Screen::captureRegion(cfg.regionX1, cfg.regionY1, w, h);

    BarInfo bar = Detector::detectBar(pixels, w, h,
                                       cfg.barColor, cfg.targetLineColor,
                                       cfg.barTolerance, cfg.centerWindowPct,
                                       cfg.contactPaddingPx);

    if (bar.found) {
        barWasVisible = true;
        barDisappearTime = 0;

        // Check if line is in hit zone and click cooldown has passed
        if (bar.inHitZone && (now - lastClickTime >= static_cast<DWORD>(cfg.clickCooldownMs))) {
            // Calculate screen coordinates for the click
            int clickX = cfg.regionX1 + bar.lineX;
            int clickY = (cfg.regionY1 + cfg.regionY2) / 2;

            Input::leftClickAt(clickX, clickY);
            lastClickTime = now;
            setState(FisherState::MINIGAME_ACTIVE, "Clicked! Continuing...");
        }
    } else {
        // Bar not visible - might be transitioning
        if (barWasVisible) {
            if (barDisappearTime == 0) {
                barDisappearTime = now;
            } else if (now - barDisappearTime >= static_cast<DWORD>(cfg.endWaitS * 1000)) {
                // Bar has been gone long enough - minigame complete
                setState(FisherState::MINIGAME_DONE, "Minigame complete!");
            }
        }
    }

    // Check overall timeout
    if (elapsed() > static_cast<DWORD>(cfg.barTimeoutS * 1000)) {
        setState(FisherState::FAILED, "Minigame timeout");
    }
}

void Fisher::handleMinigameDone() {
    fishCaught++;
    setState(FisherState::POST_CLICK_1, "Dismissing popup...");
}

void Fisher::handlePostClick1() {
    if (elapsed() >= static_cast<DWORD>(cfg.dismissDelayS * 1000)) {
        // Click to dismiss popup
        int screenW = Screen::getScreenWidth();
        int screenH = Screen::getScreenHeight();
        Input::leftClickAt(screenW / 2, screenH / 2);

        setState(FisherState::POST_CLICK_2, "Recasting...");
    }
}

void Fisher::handlePostClick2() {
    if (elapsed() >= static_cast<DWORD>(cfg.recastDelayS * 1000)) {
        // Click to recast
        int screenW = Screen::getScreenWidth();
        int screenH = Screen::getScreenHeight();
        Input::leftClickAt(screenW / 2, screenH / 2 + 50);

        setState(FisherState::WAITING_FOR_CAST_COLOR, "Recast! Waiting for perfect cast...");
    }
}

void Fisher::handleFailed() {
    failedCasts++;

    // Wait a moment then try again
    if (elapsed() > 2000) {
        setState(FisherState::CASTING, "Retrying cast...");
    }
}
