//
// Created by Hydrojak on 07/01/2026.
//


#define UNICODE
#define _UNICODE

#include "kiosk.h"
#include <windows.h>

// Procédure de fenêtre : gestion des messages
LRESULT CALLBACK KioskWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        // ⛔ Bloquer Alt+F4 (SC_CLOSE)
        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == SC_CLOSE) {
                // On ignore simplement
                return 0;
            }
            break;

            // ⛔ Bloquer toute tentative de fermeture standard
        case WM_CLOSE:
            // Ne PAS appeler DestroyWindow
            return 0;

        case WM_KEYDOWN:
            if (wParam == 'P' || wParam == 'p' || wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Création d'une fenêtre plein écran, topmost, sans bordure
HWND create_kiosk_window(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = KioskWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = KIOSK_CLASS_NAME;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Échec de RegisterClassExW", L"Kiosk", MB_ICONERROR | MB_OK);
        return NULL;
    }

    // Récupérer la taille de l'écran
    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Fenêtre plein écran, sans bordure, toujours au premier plan
    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST,                 // fenêtre au-dessus des autres
        KIOSK_CLASS_NAME,
        L"Kiosk",
        WS_POPUP | WS_VISIBLE,         // popup sans bordures, visible
        0,
        0,
        screenWidth,
        screenHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"Échec de CreateWindowExW", L"Kiosk", MB_ICONERROR | MB_OK);
        return NULL;
    }

    // Mettre la fenêtre en avant-plan et plein écran
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // Optionnel : cacher le curseur (décommente si tu veux)
    // ShowCursor(FALSE);

    return hwnd;
}

// Boucle de messages classique
int kiosk_message_loop(void) {
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

// Point d'entrée Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    HWND hwnd = create_kiosk_window(hInstance);
    if (!hwnd) {
        return 1;
    }

    int code = kiosk_message_loop();

    return code;
}

