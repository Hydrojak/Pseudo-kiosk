//
// Created by Hydrojak on 07/01/2026.
//

// pong_kiosk.h
#ifndef PONG_KIOSK_H
#define PONG_KIOSK_H

#include <windows.h>

#define PONG_CLASS_NAME L"PongKioskWindowClass"

LRESULT CALLBACK PongWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND create_pong_window(HINSTANCE hInstance);
int  pong_message_loop(void);

#endif // PONG_KIOSK_H
