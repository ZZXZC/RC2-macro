#pragma once

#include "config.h"
#include <windows.h>
#include <string>
#include <functional>

enum class FisherState {
    IDLE,
    CASTING,
    WAITING_FOR_CAST_COLOR,
    WAITING_FOR_BAR,
    MINIGAME_ACTIVE,
    MINIGAME_DONE,
    POST_CLICK_1,
    POST_CLICK_2,
    FAILED
};

class Fisher {
public:
    Fisher(Config& config);

    void update();
    void start();
    void stop();
    void togglePause();

    FisherState getState() const;
    std::string getStateName() const;
    std::string getStatusMessage() const;
    int getFishCaught() const;
    int getFailedCasts() const;

    std::function<void(FisherState, const std::string&)> onStateChange;

private:
    Config& cfg;
    FisherState state;
    std::string statusMessage;

    DWORD stateStartTime;
    DWORD lastScanTime;
    DWORD lastClickTime;
    DWORD lastTickTime;

    DWORD barDisappearTime;
    bool barWasVisible;
    int clickCooldownMs;

    int fishCaught;
    int failedCasts;

    void handleIdle();
    void handleCasting();
    void handleWaitingForCastColor();
    void handleWaitingForBar();
    void handleMinigameActive();
    void handleMinigameDone();
    void handlePostClick1();
    void handlePostClick2();
    void handleFailed();

    void setState(FisherState newState, const std::string& msg);
    DWORD elapsed() const;
    void clickAtCenterOfBar();
};
