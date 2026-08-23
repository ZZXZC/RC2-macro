#pragma once

#include <windows.h>

class Input {
public:
    // Move the mouse cursor to screen coordinates
    static void moveMouse(int x, int y);

    // Left click at current cursor position
    static void leftClick();

    // Left click at specific screen coordinates
    static void leftClickAt(int x, int y);

    // Right click at current cursor position
    static void rightClick();

    // Press and release a key
    static void pressKey(WORD virtualKey);

    // Hold a key down
    static void keyDown(WORD virtualKey);

    // Release a key
    static void keyUp(WORD virtualKey);

    // Move mouse with smooth interpolation (optional)
    static void moveMouseSmooth(int x, int y, int steps = 5);

private:
    static INPUT makeMouseInput(DWORD flags, LONG dx, LONG dy, DWORD data);
    static INPUT makeKeyInput(WORD virtualKey, bool keyUp);
};
