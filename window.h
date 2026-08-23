#pragma once

#include <windows.h>
#include "config.h"

class Window {
public:
    static HWND create(HINSTANCE hInstance, Config& config);
    static HWND getHwnd();

    // Update status display
    static void updateStatus(const std::string& status);

    // Update control values from config
    static void syncFromConfig();

    // Read control values into config
    static void syncToConfig();

private:
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static HWND hwndMain;
    static HWND hwndTab;

    // Tab pages
    static HWND hwndGeneral, hwndCast, hwndBar;

    // General controls
    static HWND lblHotkeyStart, edtHotkeyStart, btnRebindStart, btnResetStart;
    static HWND lblHotkeyRegion, edtHotkeyRegion, btnRebindRegion, btnResetRegion;
    static HWND lblHotkeyExit, edtHotkeyExit, btnRebindExit, btnResetExit;
    static HWND lblHotkeyPause, edtHotkeyPause, btnRebindPause, btnResetPause;
    static HWND cmbMonitor;
    static HWND chkAlwaysOnTop, chkAutoMinimize, chkAutoFocus, chkAutoMaximize;

    // Cast controls
    static HWND btnSetCastRegion;
    static HWND lblCastColor, edtCastColor;
    static HWND lblCastTolerance, edtCastTolerance;
    static HWND lblCastScan, edtCastScan;
    static HWND lblCastTimeout, edtCastTimeout;
    static HWND lblRecastDelay, edtRecastDelay;
    static HWND chkDismissPopup;
    static HWND lblDismissDelay, edtDismissDelay;

    // Bar controls
    static HWND btnSelectArea;
    static HWND lblBarColor, edtBarColor;
    static HWND lblTargetLine, edtTargetLine;
    static HWND lblBarTolerance, edtBarTolerance;
    static HWND lblBarScan, edtBarScan;
    static HWND lblClickCooldown, edtClickCooldown;
    static HWND lblContactPad, edtContactPad;
    static HWND lblCenterWindow, edtCenterWindow;
    static HWND lblBarTimeout, edtBarTimeout;
    static HWND lblEndWait, edtEndWait;
    static HWND btnCalibrate;

    // Action buttons
    static HWND btnStart, btnStop, btnExit;

    // Status
    static HWND lblStatus;

    static Config* cfgPtr;
    static HINSTANCE hInst;

    // Helper to create a labeled edit control
    static HWND createLabeledEdit(HWND parent, const char* label, const char* value,
                                   int x, int y, int labelW, int editW, int id);
    static HWND createButton(HWND parent, const char* text, int x, int y, int w, int h, int id);
    static HWND createCheckbox(HWND parent, const char* text, bool checked, int x, int y, int id);
    static HWND createLabel(HWND parent, const char* text, int x, int y, int w, int h, int id);

    static void createGeneralTab(HWND parent);
    static void createCastTab(HWND parent);
    static void createBarTab(HWND parent);
};
