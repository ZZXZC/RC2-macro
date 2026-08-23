#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#ifndef WINVER
#define WINVER 0x0601
#endif

#include <windows.h>
#include <commctrl.h>
#include "window.h"
#include "resource.h"
#include "screen.h"
#include "selector.h"
#include "calibrator.h"
#include <string>
#include <cstdio>

HWND Window::hwndMain = nullptr;
HWND Window::hwndTab = nullptr;
HWND Window::hwndGeneral = nullptr;
HWND Window::hwndCast = nullptr;
HWND Window::hwndBar = nullptr;

HWND Window::lblHotkeyStart = nullptr, Window::edtHotkeyStart = nullptr;
HWND Window::btnRebindStart = nullptr, Window::btnResetStart = nullptr;
HWND Window::lblHotkeyRegion = nullptr, Window::edtHotkeyRegion = nullptr;
HWND Window::btnRebindRegion = nullptr, Window::btnResetRegion = nullptr;
HWND Window::lblHotkeyExit = nullptr, Window::edtHotkeyExit = nullptr;
HWND Window::btnRebindExit = nullptr, Window::btnResetExit = nullptr;
HWND Window::lblHotkeyPause = nullptr, Window::edtHotkeyPause = nullptr;
HWND Window::btnRebindPause = nullptr, Window::btnResetPause = nullptr;
HWND Window::cmbMonitor = nullptr;
HWND Window::chkAlwaysOnTop = nullptr, Window::chkAutoMinimize = nullptr;
HWND Window::chkAutoFocus = nullptr, Window::chkAutoMaximize = nullptr;

HWND Window::btnSetCastRegion = nullptr;
HWND Window::lblCastColor = nullptr, Window::edtCastColor = nullptr;
HWND Window::lblCastTolerance = nullptr, Window::edtCastTolerance = nullptr;
HWND Window::lblCastScan = nullptr, Window::edtCastScan = nullptr;
HWND Window::lblCastTimeout = nullptr, Window::edtCastTimeout = nullptr;
HWND Window::lblRecastDelay = nullptr, Window::edtRecastDelay = nullptr;
HWND Window::chkDismissPopup = nullptr;
HWND Window::lblDismissDelay = nullptr, Window::edtDismissDelay = nullptr;

HWND Window::btnSelectArea = nullptr;
HWND Window::lblBarColor = nullptr, Window::edtBarColor = nullptr;
HWND Window::lblTargetLine = nullptr, Window::edtTargetLine = nullptr;
HWND Window::lblBarTolerance = nullptr, Window::edtBarTolerance = nullptr;
HWND Window::lblBarScan = nullptr, Window::edtBarScan = nullptr;
HWND Window::lblClickCooldown = nullptr, Window::edtClickCooldown = nullptr;
HWND Window::lblContactPad = nullptr, Window::edtContactPad = nullptr;
HWND Window::lblCenterWindow = nullptr, Window::edtCenterWindow = nullptr;
HWND Window::lblBarTimeout = nullptr, Window::edtBarTimeout = nullptr;
HWND Window::lblEndWait = nullptr, Window::edtEndWait = nullptr;
HWND Window::btnCalibrate = nullptr;

HWND Window::btnStart = nullptr, Window::btnStop = nullptr, Window::btnExit = nullptr;
HWND Window::lblStatus = nullptr;

Config* Window::cfgPtr = nullptr;
HINSTANCE Window::hInst = nullptr;

HWND Window::createLabeledEdit(HWND parent, const char* label, const char* value,
                                int x, int y, int labelW, int editW, int id) {
    CreateWindowA("STATIC", label, WS_CHILD | WS_VISIBLE,
                  x, y, labelW, 20, parent, NULL, hInst, NULL);
    HWND edt = CreateWindowA("EDIT", value,
                              WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                              x + labelW + 5, y, editW, 20, parent,
                              reinterpret_cast<HMENU>(id), hInst, NULL);
    return edt;
}

HWND Window::createButton(HWND parent, const char* text, int x, int y, int w, int h, int id) {
    return CreateWindowA("BUTTON", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                          x, y, w, h, parent, reinterpret_cast<HMENU>(id), hInst, NULL);
}

HWND Window::createCheckbox(HWND parent, const char* text, bool checked, int x, int y, int id) {
    HWND chk = CreateWindowA("BUTTON", text,
                              WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                              x, y, 200, 20, parent,
                              reinterpret_cast<HMENU>(id), hInst, NULL);
    if (checked) SendMessage(chk, BM_SETCHECK, BST_CHECKED, 0);
    return chk;
}

HWND Window::createLabel(HWND parent, const char* text, int x, int y, int w, int h, int id) {
    return CreateWindowA("STATIC", text, WS_CHILD | WS_VISIBLE,
                          x, y, w, h, parent,
                          reinterpret_cast<HMENU>(id), hInst, NULL);
}

void Window::createGeneralTab(HWND parent) {
    int y = 10;

    CreateWindowA("STATIC", "Hotkeys", WS_CHILD | WS_VISIBLE,
                  10, y, 200, 20, parent, NULL, hInst, NULL);
    y += 25;

    lblHotkeyStart = createLabel(parent, "Start / Stop:", 10, y, 100, 20, 0);
    edtHotkeyStart = CreateWindowA("STATIC", "F1", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    120, y, 60, 20, parent, NULL, hInst, NULL);
    btnRebindStart = createButton(parent, "Rebind", 190, y, 60, 22, IDC_BTN_REBIND_START);
    btnResetStart = createButton(parent, "Reset", 260, y, 50, 22, IDC_BTN_RESET_START);
    y += 28;

    lblHotkeyRegion = createLabel(parent, "Select Region:", 10, y, 100, 20, 0);
    edtHotkeyRegion = CreateWindowA("STATIC", "F2", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                     120, y, 60, 20, parent, NULL, hInst, NULL);
    btnRebindRegion = createButton(parent, "Rebind", 190, y, 60, 22, IDC_BTN_REBIND_REGION);
    btnResetRegion = createButton(parent, "Reset", 260, y, 50, 22, IDC_BTN_RESET_REGION);
    y += 28;

    lblHotkeyExit = createLabel(parent, "Exit:", 10, y, 100, 20, 0);
    edtHotkeyExit = CreateWindowA("STATIC", "F3", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                   120, y, 60, 20, parent, NULL, hInst, NULL);
    btnRebindExit = createButton(parent, "Rebind", 190, y, 60, 22, IDC_BTN_REBIND_EXIT);
    btnResetExit = createButton(parent, "Reset", 260, y, 50, 22, IDC_BTN_RESET_EXIT);
    y += 28;

    lblHotkeyPause = createLabel(parent, "Pause:", 10, y, 100, 20, 0);
    edtHotkeyPause = CreateWindowA("STATIC", "F4", WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    120, y, 60, 20, parent, NULL, hInst, NULL);
    btnRebindPause = createButton(parent, "Rebind", 190, y, 60, 22, IDC_BTN_REBIND_PAUSE);
    btnResetPause = createButton(parent, "Reset", 260, y, 50, 22, IDC_BTN_RESET_PAUSE);
    y += 35;

    CreateWindowA("STATIC", "Monitor", WS_CHILD | WS_VISIBLE,
                  10, y, 200, 20, parent, NULL, hInst, NULL);
    y += 25;

    CreateWindowA("STATIC", "Monitor:", WS_CHILD | WS_VISIBLE, 10, y, 60, 20, parent, NULL, hInst, NULL);
    cmbMonitor = CreateWindowA("COMBOBOX", NULL,
                                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
                                80, y, 200, 200, parent,
                                reinterpret_cast<HMENU>(IDC_MONITOR_COMBO), hInst, NULL);

    int monCount = Screen::getMonitorCount();
    for (int i = 0; i < monCount; i++) {
        std::string name = Screen::getMonitorName(i);
        char buf[256];
        RECT rc;
        Screen::getMonitorRect(i, rc);
        snprintf(buf, sizeof(buf), "%s - %ldx%ld", name.c_str(),
                 rc.right - rc.left, rc.bottom - rc.top);
        SendMessageA(cmbMonitor, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buf));
    }
    if (monCount > 0) SendMessageA(cmbMonitor, CB_SETCURSEL, 0, 0);
    y += 30;

    CreateWindowA("STATIC", "GUI", WS_CHILD | WS_VISIBLE,
                  10, y, 200, 20, parent, NULL, hInst, NULL);
    y += 25;

    chkAlwaysOnTop = createCheckbox(parent, "Always On Top", true, 10, y, IDC_CHK_ALWAYSONTOP);
    y += 22;
    chkAutoMinimize = createCheckbox(parent, "Auto Minimize", false, 10, y, IDC_CHK_AUTOMINIMIZE);
    chkAutoFocus = createCheckbox(parent, "Auto Focus", true, 160, y, IDC_CHK_AUTOFOCUS);
    y += 22;
    chkAutoMaximize = createCheckbox(parent, "Auto Maximize", false, 10, y, IDC_CHK_AUTOMAXIMIZE);
    y += 30;
}

void Window::createCastTab(HWND parent) {
    int y = 10;

    CreateWindowA("STATIC", "Detection", WS_CHILD | WS_VISIBLE,
                  10, y, 200, 20, parent, NULL, hInst, NULL);
    y += 25;

    edtCastColor = createLabeledEdit(parent, "Target Color:", "#0af809",
                                      10, y, 100, 120, IDC_EDT_CASTCOLOR);
    y += 28;

    edtCastTolerance = createLabeledEdit(parent, "Tolerance:", "10",
                                          10, y, 100, 120, IDC_EDT_CASTTOLERANCE);
    y += 28;

    edtCastScan = createLabeledEdit(parent, "Scan Every (ms):", "10",
                                     10, y, 100, 120, IDC_EDT_CASTSCAN);
    y += 28;

    edtCastTimeout = createLabeledEdit(parent, "Timeout (s):", "10.0",
                                        10, y, 100, 120, IDC_EDT_CASTTIMEOUT);
    y += 28;

    edtRecastDelay = createLabeledEdit(parent, "Recast Delay (s):", "0.75",
                                        10, y, 100, 120, IDC_EDT_RECASTDELAY);
    y += 28;

    chkDismissPopup = createCheckbox(parent, "Dismiss popup before recast", true,
                                      10, y, IDC_CHK_DISMISSPOPUP);
    y += 22;

    edtDismissDelay = createLabeledEdit(parent, "Dismiss Delay (s):", "0.25",
                                         10, y, 100, 120, IDC_EDT_DISMISSDELAY);
    y += 35;

    btnSetCastRegion = createButton(parent, "Set Cast Pixel", 10, y, 120, 25, IDC_BTN_SETCASTREGION);
    y += 35;
}

void Window::createBarTab(HWND parent) {
    int y = 10;

    btnSelectArea = createButton(parent, "Select Area", 10, y, 120, 25, IDC_BTN_SELECTAREA);
    btnCalibrate = createButton(parent, "Calibrate", 140, y, 80, 25, IDC_BTN_CALIBRATE);
    y += 35;

    CreateWindowA("STATIC", "Detection", WS_CHILD | WS_VISIBLE,
                  10, y, 200, 20, parent, NULL, hInst, NULL);
    y += 25;

    edtBarColor = createLabeledEdit(parent, "Bar Color:", "#628aef",
                                     10, y, 100, 120, IDC_EDT_BARCOLOR);
    y += 28;

    edtTargetLine = createLabeledEdit(parent, "Target Line:", "#ffffff",
                                       10, y, 100, 120, IDC_EDT_TARGETLINE);
    y += 28;

    edtBarTolerance = createLabeledEdit(parent, "Tolerance:", "30",
                                         10, y, 100, 120, IDC_EDT_BARTOLERANCE);
    y += 28;

    edtBarScan = createLabeledEdit(parent, "Scan Every (ms):", "5",
                                    10, y, 100, 120, IDC_EDT_BARSCAN);
    y += 28;

    edtClickCooldown = createLabeledEdit(parent, "Click Cooldown:", "50",
                                          10, y, 100, 120, IDC_EDT_CLICKCOOLDOWN);
    y += 28;

    edtContactPad = createLabeledEdit(parent, "Contact Padding:", "3",
                                       10, y, 100, 120, IDC_EDT_CONTACTPAD);
    y += 28;

    edtCenterWindow = createLabeledEdit(parent, "Center Window:", "35.0",
                                         10, y, 100, 120, IDC_EDT_CENTERWINDOW);
    y += 28;

    edtBarTimeout = createLabeledEdit(parent, "Timeout (s):", "45.0",
                                       10, y, 100, 120, IDC_EDT_BARTIMEOUT);
    y += 28;

    edtEndWait = createLabeledEdit(parent, "End Wait (s):", "1.25",
                                    10, y, 100, 120, IDC_EDT_ENDWAIT);
    y += 30;
}

LRESULT CALLBACK Window::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND: {
        int id = LOWORD(wParam);

        switch (id) {
        case IDC_BTN_START:
            if (cfgPtr) {
                syncToConfig();
                cfgPtr->running = true;
                cfgPtr->paused = false;
            }
            break;

        case IDC_BTN_STOP:
            if (cfgPtr) {
                cfgPtr->running = false;
            }
            break;

        case IDC_BTN_EXIT:
            PostQuitMessage(0);
            break;

        case IDC_BTN_SELECTAREA: {
            syncToConfig();
            Selector::startSelection(hInst, [](int x1, int y1, int x2, int y2) {
                if (cfgPtr) {
                    cfgPtr->regionX1 = x1;
                    cfgPtr->regionY1 = y1;
                    cfgPtr->regionX2 = x2;
                    cfgPtr->regionY2 = y2;
                    cfgPtr->regionSelected = true;
                    updateStatus("Region selected!");
                }
            });
            break;
        }

        case IDC_BTN_SETCASTREGION: {
            if (cfgPtr) {
                cfgPtr->castPixelX = Screen::getScreenWidth() / 2;
                cfgPtr->castPixelY = Screen::getScreenHeight() / 2;
                cfgPtr->castRegionSelected = true;
                updateStatus("Cast pixel set to center");
            }
            break;
        }

        case IDC_BTN_CALIBRATE: {
            syncToConfig();
            if (Calibrator::calibrate(*cfgPtr)) {
                syncFromConfig();
                updateStatus("Calibration OK: " + Calibrator::getLastCalibrationResult());
            } else {
                updateStatus("Calibration failed: " + Calibrator::getLastCalibrationResult());
            }
            break;
        }

        case IDC_CHK_ALWAYSONTOP:
            if (cfgPtr) {
                cfgPtr->alwaysOnTop = (SendMessage(chkAlwaysOnTop, BM_GETCHECK, 0, 0) == BST_CHECKED);
                SetWindowPos(hwndMain, cfgPtr->alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                             0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }
            break;

        case IDC_BTN_REBIND_START:
        case IDC_BTN_REBIND_REGION:
        case IDC_BTN_REBIND_EXIT:
        case IDC_BTN_REBIND_PAUSE:
            updateStatus("Press a key to bind...");
            break;
        }

        if (id >= IDC_CHK_ALWAYSONTOP && id <= IDC_CHK_AUTOMAXIMIZE) {
            syncToConfig();
        }
        break;
    }

    case WM_NOTIFY: {
        NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);
        if (nmhdr->hwndFrom == hwndTab && nmhdr->code == TCN_SELCHANGE) {
            int sel = TabCtrl_GetCurSel(hwndTab);
            ShowWindow(hwndGeneral, sel == 0 ? SW_SHOW : SW_HIDE);
            ShowWindow(hwndCast, sel == 1 ? SW_SHOW : SW_HIDE);
            ShowWindow(hwndBar, sel == 2 ? SW_SHOW : SW_HIDE);
        }
        break;
    }

    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_DESTROY:
        return 0;
    }

    return DefWindowProcA(hwnd, msg, wParam, lParam);
}

HWND Window::create(HINSTANCE hInstance, Config& config) {
    hInst = hInstance;
    cfgPtr = &config;

    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(GetSysColor(COLOR_BTNFACE));
    wc.lpszClassName = "RC2FishWindow";

    RegisterClassExA(&wc);

    hwndMain = CreateWindowExA(
        config.alwaysOnTop ? WS_EX_TOPMOST : 0,
        "RC2FishWindow", "RC2 Fishing Macro",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 380, 520,
        NULL, NULL, hInstance, NULL
    );

    hwndTab = CreateWindowA(WC_TABCONTROLA, "",
                             WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
                             5, 5, 365, 440,
                             hwndMain, reinterpret_cast<HMENU>(IDC_TAB), hInstance, NULL);

    TCITEMA tie = {};
    tie.mask = TCIF_TEXT;
    tie.pszText = (LPSTR)"General";
    TabCtrl_InsertItem(hwndTab, 0, &tie);
    tie.pszText = (LPSTR)"Cast";
    TabCtrl_InsertItem(hwndTab, 1, &tie);
    tie.pszText = (LPSTR)"Bar";
    TabCtrl_InsertItem(hwndTab, 2, &tie);

    hwndGeneral = CreateWindowA("STATIC", "", WS_CHILD | WS_VISIBLE,
                                 10, 30, 350, 410, hwndTab, NULL, hInstance, NULL);
    hwndCast = CreateWindowA("STATIC", "", WS_CHILD,
                              10, 30, 350, 410, hwndTab, NULL, hInstance, NULL);
    hwndBar = CreateWindowA("STATIC", "", WS_CHILD,
                             10, 30, 350, 410, hwndTab, NULL, hInstance, NULL);

    createGeneralTab(hwndGeneral);
    createCastTab(hwndCast);
    createBarTab(hwndBar);

    btnStart = createButton(hwndMain, "Start (F1)", 10, 455, 80, 28, IDC_BTN_START);
    btnStop = createButton(hwndMain, "Stop", 100, 455, 60, 28, IDC_BTN_STOP);
    btnExit = createButton(hwndMain, "Exit (F3)", 170, 455, 70, 28, IDC_BTN_EXIT);

    lblStatus = createLabel(hwndMain, "Status: IDLE", 250, 460, 120, 20, IDC_LBL_STATUS);

    syncFromConfig();

    ShowWindow(hwndMain, SW_SHOW);
    UpdateWindow(hwndMain);

    return hwndMain;
}

HWND Window::getHwnd() { return hwndMain; }

void Window::updateStatus(const std::string& status) {
    if (lblStatus) {
        SetWindowTextA(lblStatus, ("Status: " + status).c_str());
    }
}

void Window::syncFromConfig() {
    if (!cfgPtr) return;

    SendMessage(chkAlwaysOnTop, BM_SETCHECK, cfgPtr->alwaysOnTop ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(chkAutoMinimize, BM_SETCHECK, cfgPtr->autoMinimize ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(chkAutoFocus, BM_SETCHECK, cfgPtr->autoFocus ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(chkAutoMaximize, BM_SETCHECK, cfgPtr->autoMaximize ? BST_CHECKED : BST_UNCHECKED, 0);

    SetWindowTextA(edtCastColor, cfgPtr->castTargetColor.c_str());
    SetWindowTextA(edtCastTolerance, std::to_string(cfgPtr->castTolerance).c_str());
    SetWindowTextA(edtCastScan, std::to_string(cfgPtr->castScanEveryMs).c_str());

    char buf[64];
    snprintf(buf, sizeof(buf), "%.1f", cfgPtr->castTimeoutS);
    SetWindowTextA(edtCastTimeout, buf);
    snprintf(buf, sizeof(buf), "%.2f", cfgPtr->recastDelayS);
    SetWindowTextA(edtRecastDelay, buf);
    SendMessage(chkDismissPopup, BM_SETCHECK, cfgPtr->dismissPopup ? BST_CHECKED : BST_UNCHECKED, 0);
    snprintf(buf, sizeof(buf), "%.2f", cfgPtr->dismissDelayS);
    SetWindowTextA(edtDismissDelay, buf);

    SetWindowTextA(edtBarColor, cfgPtr->barColor.c_str());
    SetWindowTextA(edtTargetLine, cfgPtr->targetLineColor.c_str());
    SetWindowTextA(edtBarTolerance, std::to_string(cfgPtr->barTolerance).c_str());
    SetWindowTextA(edtBarScan, std::to_string(cfgPtr->barScanEveryMs).c_str());
    SetWindowTextA(edtClickCooldown, std::to_string(cfgPtr->clickCooldownMs).c_str());
    SetWindowTextA(edtContactPad, std::to_string(cfgPtr->contactPaddingPx).c_str());
    snprintf(buf, sizeof(buf), "%.1f", cfgPtr->centerWindowPct);
    SetWindowTextA(edtCenterWindow, buf);
    snprintf(buf, sizeof(buf), "%.1f", cfgPtr->barTimeoutS);
    SetWindowTextA(edtBarTimeout, buf);
    snprintf(buf, sizeof(buf), "%.2f", cfgPtr->endWaitS);
    SetWindowTextA(edtEndWait, buf);
}

void Window::syncToConfig() {
    if (!cfgPtr) return;

    char buf[256];

    cfgPtr->alwaysOnTop = (SendMessage(chkAlwaysOnTop, BM_GETCHECK, 0, 0) == BST_CHECKED);
    cfgPtr->autoMinimize = (SendMessage(chkAutoMinimize, BM_GETCHECK, 0, 0) == BST_CHECKED);
    cfgPtr->autoFocus = (SendMessage(chkAutoFocus, BM_GETCHECK, 0, 0) == BST_CHECKED);
    cfgPtr->autoMaximize = (SendMessage(chkAutoMaximize, BM_GETCHECK, 0, 0) == BST_CHECKED);

    cfgPtr->monitorIndex = static_cast<int>(SendMessage(cmbMonitor, CB_GETCURSEL, 0, 0));

    GetWindowTextA(edtCastColor, buf, sizeof(buf));
    cfgPtr->castTargetColor = buf;
    GetWindowTextA(edtCastTolerance, buf, sizeof(buf));
    cfgPtr->castTolerance = atoi(buf);
    GetWindowTextA(edtCastScan, buf, sizeof(buf));
    cfgPtr->castScanEveryMs = atoi(buf);
    GetWindowTextA(edtCastTimeout, buf, sizeof(buf));
    cfgPtr->castTimeoutS = atof(buf);
    GetWindowTextA(edtRecastDelay, buf, sizeof(buf));
    cfgPtr->recastDelayS = atof(buf);
    cfgPtr->dismissPopup = (SendMessage(chkDismissPopup, BM_GETCHECK, 0, 0) == BST_CHECKED);
    GetWindowTextA(edtDismissDelay, buf, sizeof(buf));
    cfgPtr->dismissDelayS = atof(buf);

    GetWindowTextA(edtBarColor, buf, sizeof(buf));
    cfgPtr->barColor = buf;
    GetWindowTextA(edtTargetLine, buf, sizeof(buf));
    cfgPtr->targetLineColor = buf;
    GetWindowTextA(edtBarTolerance, buf, sizeof(buf));
    cfgPtr->barTolerance = atoi(buf);
    GetWindowTextA(edtBarScan, buf, sizeof(buf));
    cfgPtr->barScanEveryMs = atoi(buf);
    GetWindowTextA(edtClickCooldown, buf, sizeof(buf));
    cfgPtr->clickCooldownMs = atoi(buf);
    GetWindowTextA(edtContactPad, buf, sizeof(buf));
    cfgPtr->contactPaddingPx = atoi(buf);
    GetWindowTextA(edtCenterWindow, buf, sizeof(buf));
    cfgPtr->centerWindowPct = atof(buf);
    GetWindowTextA(edtBarTimeout, buf, sizeof(buf));
    cfgPtr->barTimeoutS = atof(buf);
    GetWindowTextA(edtEndWait, buf, sizeof(buf));
    cfgPtr->endWaitS = atof(buf);
}
