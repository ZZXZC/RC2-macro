#include "input.h"
#include <cmath>

INPUT Input::makeMouseInput(DWORD flags, LONG dx, LONG dy, DWORD data) {
    INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dx = dx;
    input.mi.dy = dy;
    input.mi.mouseData = data;
    input.mi.dwFlags = flags;
    return input;
}

INPUT Input::makeKeyInput(WORD virtualKey, bool keyUp) {
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = virtualKey;
    input.ki.dwFlags = keyUp ? KEYEVENTF_KEYUP : 0;
    return input;
}

void Input::moveMouse(int x, int y) {
    // Convert screen coordinates to absolute (0-65535 range)
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    LONG absX = static_cast<LONG>((x * 65535.0) / screenW);
    LONG absY = static_cast<LONG>((y * 65535.0) / screenH);

    INPUT input = makeMouseInput(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, absX, absY, 0);
    SendInput(1, &input, sizeof(INPUT));
}

void Input::leftClick() {
    INPUT inputs[2];
    inputs[0] = makeMouseInput(MOUSEEVENTF_LEFTDOWN, 0, 0, 0);
    inputs[1] = makeMouseInput(MOUSEEVENTF_LEFTUP, 0, 0, 0);
    SendInput(2, inputs, sizeof(INPUT));
}

void Input::leftClickAt(int x, int y) {
    moveMouse(x, y);
    leftClick();
}

void Input::rightClick() {
    INPUT inputs[2];
    inputs[0] = makeMouseInput(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0);
    inputs[1] = makeMouseInput(MOUSEEVENTF_RIGHTUP, 0, 0, 0);
    SendInput(2, inputs, sizeof(INPUT));
}

void Input::pressKey(WORD virtualKey) {
    INPUT inputs[2];
    inputs[0] = makeKeyInput(virtualKey, false);
    inputs[1] = makeKeyInput(virtualKey, true);
    SendInput(2, inputs, sizeof(INPUT));
}

void Input::keyDown(WORD virtualKey) {
    INPUT input = makeKeyInput(virtualKey, false);
    SendInput(1, &input, sizeof(INPUT));
}

void Input::keyUp(WORD virtualKey) {
    INPUT input = makeKeyInput(virtualKey, true);
    SendInput(1, &input, sizeof(INPUT));
}

void Input::moveMouseSmooth(int x, int y, int steps) {
    POINT current;
    GetCursorPos(&current);

    double dx = (x - current.x) / static_cast<double>(steps);
    double dy = (y - current.y) / static_cast<double>(steps);

    for (int i = 1; i <= steps; i++) {
        int nx = static_cast<int>(current.x + dx * i);
        int ny = static_cast<int>(current.y + dy * i);
        moveMouse(nx, ny);
        Sleep(1);
    }
}
