//
// Created by Hydrojak on 07/01/2026.
//
#define UNICODE
#define _UNICODE

#include "kiosk.h"
#include <windows.h>
#include <wchar.h>

HWND g_hEdit   = NULL;   // champ de texte
HWND g_hButton = NULL;   // bouton "Valider"

// mot secret à saisir
static const wchar_t *SECRET = L"secret";

// Procédure de fenêtre principale
LRESULT CALLBACK KioskWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    // Bloquer Alt+F4
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_CLOSE) {
            return 0;   // on ignore la fermeture
        }
        break;

    // Bloquer les fermetures classiques
    case WM_CLOSE:
        return 0;

    case WM_COMMAND: {
        // LOWORD(wParam) = ID du contrôle
        // HIWORD(wParam) = notification code
        if (LOWORD(wParam) == 2 && HIWORD(wParam) == BN_CLICKED) {
            // Clic sur le bouton "Valider"

            wchar_t buffer[256];
            GetWindowTextW(g_hEdit, buffer, 256);

            if (lstrcmpiW(buffer, SECRET) == 0) {
                // Mot correct -> on quitte
                PostQuitMessage(0);
            } else {
                // Mauvais mot -> on efface le champ (optionnel)
                SetWindowTextW(g_hEdit, L"");
            }
        }
        break;
    }

    case WM_KEYDOWN:
        // Échap = quit "urgence"
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Création de la fenêtre plein écran + champ texte + bouton
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

    int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST,
        KIOSK_CLASS_NAME,
        L"Kiosk",
        WS_POPUP | WS_VISIBLE,
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

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // --- Champ texte centré ---
    int editWidth  = 300;
    int editHeight = 30;
    int editX = (screenWidth  - editWidth)  / 2;
    int editY = (screenHeight - editHeight) / 2;

    g_hEdit = CreateWindowExW(
        0,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
        editX,
        editY,
        editWidth,
        editHeight,
        hwnd,
        (HMENU)1,
        hInstance,
        NULL
    );

    if (!g_hEdit) {
        MessageBoxW(NULL, L"Échec de création de l'EDIT", L"Kiosk", MB_ICONERROR | MB_OK);
        return hwnd;
    }

    // --- Bouton "Valider" sous le champ ---
    int buttonWidth  = 120;
    int buttonHeight = 32;
    int buttonX = (screenWidth  - buttonWidth)  / 2;
    int buttonY = editY + editHeight + 20;

    g_hButton = CreateWindowExW(
        0,
        L"BUTTON",
        L"Valider",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        buttonX,
        buttonY,
        buttonWidth,
        buttonHeight,
        hwnd,
        (HMENU)2,          // ID du bouton
        hInstance,
        NULL
    );

    if (!g_hButton) {
        MessageBoxW(NULL, L"Échec de création du bouton", L"Kiosk", MB_ICONERROR | MB_OK);
        return hwnd;
    }

    // Focus direct dans le champ texte
    SetFocus(g_hEdit);

    return hwnd;
}

// Boucle de messages
int kiosk_message_loop(void) {
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

// Point d'entrée
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

