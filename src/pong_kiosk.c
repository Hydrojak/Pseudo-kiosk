//
// Created by darkn on 07/01/2026.
//
#define UNICODE
#define _UNICODE

#include "pong_kiosk.h"
#include <windows.h>
#include <wchar.h>
#include <stdbool.h>

// --- Constantes jeu ---
#define TIMER_ID      1
#define TIMER_MS      16      // ~60 FPS
#define PADDLE_WIDTH  15
#define PADDLE_HEIGHT 80
#define BALL_SIZE     16
#define PADDLE_SPEED  15
#define BALL_SPEED_X  9
#define BALL_SPEED_Y  7
#define TARGET_SCORE  5

// --- État global du jeu ---
static int  g_screenW = 0;
static int  g_screenH = 0;

static RECT g_paddle;    // raquette à gauche
static RECT g_ball;      // balle
static int  g_ballVelX = BALL_SPEED_X;
static int  g_ballVelY = BALL_SPEED_Y;

static bool g_upPressed   = false;
static bool g_downPressed = false;

static int  g_score = 0;

// Protos internes
static void reset_ball(HWND hwnd);
static void reset_game(HWND hwnd);
static void update_game(HWND hwnd);
static void draw_game(HWND hwnd);

// --- Procédure de fenêtre ---
LRESULT CALLBACK PongWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_CREATE: {
        // Récupère la taille de la fenêtre client
        RECT rc;
        GetClientRect(hwnd, &rc);
        g_screenW = rc.right  - rc.left;
        g_screenH = rc.bottom - rc.top;

        // Positionne la raquette à gauche, centrée verticalement
        int paddleX = 40;
        int paddleY = (g_screenH - PADDLE_HEIGHT) / 2;
        g_paddle.left   = paddleX;
        g_paddle.top    = paddleY;
        g_paddle.right  = paddleX + PADDLE_WIDTH;
        g_paddle.bottom = paddleY + PADDLE_HEIGHT;

        reset_game(hwnd);

        // Timer pour le “game loop”
        SetTimer(hwnd, TIMER_ID, TIMER_MS, NULL);
        return 0;
    }

    // Bloquer Alt+F4
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_CLOSE) {
            return 0;
        }
        break;

    // Bloquer fermeture standard
    case WM_CLOSE:
        return 0;

    case WM_KEYDOWN:
{
    // Quitte en urgence (panic button)
    if (wParam == VK_ESCAPE) {
        PostQuitMessage(0);
        return 0;
    }

    // Sécurité : P quitte aussi
    if (wParam == 'P' || wParam == 'p') {
        PostQuitMessage(0);
        return 0;
    }

    // Contrôles du jeu
    if (wParam == VK_UP) {
        g_upPressed = true;
    }
    if (wParam == VK_DOWN) {
        g_downPressed = true;
    }
    return 0;
}


    case WM_KEYUP:
        if (wParam == VK_UP) {
            g_upPressed = false;
        }
        if (wParam == VK_DOWN) {
            g_downPressed = false;
        }
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_ID) {
            update_game(hwnd);
        }
        return 0;

    case WM_PAINT:
        draw_game(hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// --- Création de la fenêtre plein écran ---
HWND create_pong_window(HINSTANCE hInstance) {
    WNDCLASSEXW wc = {0};
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = PongWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = PONG_CLASS_NAME;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        MessageBoxW(NULL, L"Échec RegisterClassExW", L"PongKiosk", MB_ICONERROR | MB_OK);
        return NULL;
    }

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST,
        PONG_CLASS_NAME,
        L"Pong Kiosk",
        WS_POPUP | WS_VISIBLE,
        0, 0,
        screenW, screenH,
        NULL, NULL,
        hInstance,
        NULL
    );

    if (!hwnd) {
        MessageBoxW(NULL, L"Échec CreateWindowExW", L"PongKiosk", MB_ICONERROR | MB_OK);
        return NULL;
    }

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hwnd);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    return hwnd;
}

// --- Boucle de messages ---
int pong_message_loop(void) {
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}

// --- Logique jeu ---

static void reset_ball(HWND hwnd) {
    // Balle au centre
    int size = BALL_SIZE;
    int x = g_screenW / 2 - size / 2;
    int y = g_screenH / 2 - size / 2;

    g_ball.left   = x;
    g_ball.top    = y;
    g_ball.right  = x + size;
    g_ball.bottom = y + size;

    // Direction de base (vers la gauche)
    g_ballVelX = -BALL_SPEED_X;
    g_ballVelY = BALL_SPEED_Y;
    InvalidateRect(hwnd, NULL, TRUE);
}

static void reset_game(HWND hwnd) {
    g_score = 0;
    reset_ball(hwnd);
}

static void update_game(HWND hwnd) {
    // Déplacement de la raquette
    if (g_upPressed) {
        int dy = -PADDLE_SPEED;
        OffsetRect(&g_paddle, 0, dy);
        if (g_paddle.top < 0) {
            OffsetRect(&g_paddle, 0, -g_paddle.top);
        }
        if (g_paddle.bottom > g_screenH) {
            OffsetRect(&g_paddle, 0, g_screenH - g_paddle.bottom);
        }
    }
    if (g_downPressed) {
        int dy = PADDLE_SPEED;
        OffsetRect(&g_paddle, 0, dy);
        if (g_paddle.bottom > g_screenH) {
            OffsetRect(&g_paddle, 0, g_screenH - g_paddle.bottom);
        }
        if (g_paddle.top < 0) {
            OffsetRect(&g_paddle, 0, -g_paddle.top);
        }
    }

    // Déplacement de la balle
    OffsetRect(&g_ball, g_ballVelX, g_ballVelY);

    // Collisions haut/bas
    if (g_ball.top < 0) {
        int diff = -g_ball.top;
        OffsetRect(&g_ball, 0, diff);
        g_ballVelY = -g_ballVelY;
    }
    if (g_ball.bottom > g_screenH) {
        int diff = g_screenH - g_ball.bottom;
        OffsetRect(&g_ball, 0, diff);
        g_ballVelY = -g_ballVelY;
    }

    // Collision avec la raquette
    RECT inter;
    if (IntersectRect(&inter, &g_ball, &g_paddle)) {
        // On repousse la balle à droite de la raquette
        int dx = g_paddle.right - g_ball.left;
        OffsetRect(&g_ball, dx, 0);
        g_ballVelX = -g_ballVelX;

        // Ajoute un point
        g_score++;

        // Si score atteint -> quitter
        if (g_score >= TARGET_SCORE) {
            PostQuitMessage(0);
            return;
        }
    }

    // Si la balle sort à gauche -> reset balle & score
    if (g_ball.right < 0) {
        reset_game(hwnd);
    }

    // Si la balle sort à droite -> simplement rebondir
    if (g_ball.left > g_screenW) {
        int diff = g_ball.left - g_screenW;
        OffsetRect(&g_ball, -diff, 0);
        g_ballVelX = -g_ballVelX;
    }

    InvalidateRect(hwnd, NULL, TRUE);
}

static void draw_game(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // Fond noir
    HBRUSH brushBg = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, &ps.rcPaint, brushBg);
    DeleteObject(brushBg);

    // Raquette blanche
    HBRUSH brushWhite = CreateSolidBrush(RGB(255, 255, 255));
    FillRect(hdc, &g_paddle, brushWhite);

    // Balle blanche
    FillRect(hdc, &g_ball, brushWhite);

    // Score en haut au centre
    wchar_t scoreText[64];
    wsprintfW(scoreText, L"Score: %d / %d", g_score, TARGET_SCORE);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));

    RECT rc;
    GetClientRect(hwnd, &rc);
    rc.top += 20;
    DrawTextW(hdc, scoreText, -1, &rc, DT_CENTER | DT_TOP);

    DeleteObject(brushWhite);

    EndPaint(hwnd, &ps);
}

// --- Point d'entrée ---
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nCmdShow;

    HWND hwnd = create_pong_window(hInstance);
    if (!hwnd) {
        return 1;
    }

    return pong_message_loop();
}
