#include <windows.h>
#include <commctrl.h>
#include "config.h"
#include "window.h"
#include "fisher.h"
#include "screen.h"
#include "selector.h"
#include "calibrator.h"
#include "resource.h"

#define WM_FISHER_UPDATE (WM_USER + 1)

Config g_config;
Fisher g_fisher(g_config);
HWND g_hwndMain = NULL;

#define HK_START    1
#define HK_REGION   2
#define HK_EXIT     3
#define HK_PAUSE    4
#define HK_CALIBRATE 5

void registerHotkeys(HWND hwnd) {
    RegisterHotKey(hwnd, HK_START,    0, g_config.hotkeyStart);
    RegisterHotKey(hwnd, HK_REGION,   0, g_config.hotkeyRegion);
    RegisterHotKey(hwnd, HK_EXIT,     0, g_config.hotkeyExit);
    RegisterHotKey(hwnd, HK_PAUSE,    0, g_config.hotkeyPause);
    RegisterHotKey(hwnd, HK_CALIBRATE,0, g_config.hotkeyCalibrate);
}

void unregisterHotkeys(HWND hwnd) {
    UnregisterHotKey(hwnd, HK_START);
    UnregisterHotKey(hwnd, HK_REGION);
    UnregisterHotKey(hwnd, HK_EXIT);
    UnregisterHotKey(hwnd, HK_PAUSE);
    UnregisterHotKey(hwnd, HK_CALIBRATE);
}

LRESULT CALLBACK mainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_HOTKEY: {
        switch (wParam) {
        case HK_START:
            if (g_config.running) {
                g_fisher.stop();
                Window::updateStatus("Stopped");
            } else {
                HWND robloxHwnd = Screen::findRobloxWindow();
                if (robloxHwnd) {
                    g_fisher.start();
                    Window::updateStatus("Starting...");
                } else {
                    Window::updateStatus("Roblox not found!");
                }
            }
            break;

        case HK_REGION: {
            Selector::startSelection(GetModuleHandle(NULL), [](int x1, int y1, int x2, int y2) {
                g_config.regionX1 = x1;
                g_config.regionY1 = y1;
                g_config.regionX2 = x2;
                g_config.regionY2 = y2;
                g_config.regionSelected = true;
                Window::updateStatus("Region selected!");
            });
            break;
        }

        case HK_EXIT:
            g_fisher.stop();
            PostQuitMessage(0);
            break;

        case HK_PAUSE:
            g_fisher.togglePause();
            Window::updateStatus(g_config.paused ? "Paused" : "Resumed");
            break;

        case HK_CALIBRATE: {
            if (Calibrator::calibrate(g_config)) {
                Window::syncFromConfig();
                Window::updateStatus("Calibration OK");
            } else {
                Window::updateStatus("Calibration failed");
            }
            break;
        }
        }
        return 0;
    }

    case WM_DESTROY:
        unregisterHotkeys(hwnd);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = mainProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "RC2FishMain";

    RegisterClassExA(&wc);

    g_hwndMain = CreateWindowExA(0, "RC2FishMain", "RC2Fish",
                                  0, 0, 0, 0, 0,
                                  HWND_MESSAGE, NULL, hInstance, NULL);

    Window::create(hInstance, g_config);

    registerHotkeys(g_hwndMain);

    g_fisher.onStateChange = [](FisherState state, const std::string& msg) {
        Window::updateStatus(msg);
    };

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        if (g_config.running && !g_config.paused) {
            g_fisher.update();
            std::string status = g_fisher.getStateName() + " | "
                               + "Caught: " + std::to_string(g_fisher.getFishCaught())
                               + " | Failed: " + std::to_string(g_fisher.getFailedCasts());
            Window::updateStatus(status);
        }
    }

    return static_cast<int>(msg.wParam);
}
