//
// Created by Hydrojak on 07/01/2026.
//

// kiosk.h
// Déclaration des fonctions pour l'appli "kiosk"

#ifndef KIOSK_H
#define KIOSK_H

#include <windows.h>

#define KIOSK_CLASS_NAME L"KioskWindowClass"

// Procédure de fenêtre
LRESULT CALLBACK KioskWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Création de la fenêtre plein écran "kiosk"
HWND create_kiosk_window(HINSTANCE hInstance);

// Boucle de messages
int kiosk_message_loop(void);

#endif // KIOSK_H
