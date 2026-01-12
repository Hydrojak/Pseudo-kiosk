//
// Created by Hydrojak on 07/01/2026.
//
#define UNICODE
#define _UNICODE

#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>

// ---------------- CONFIG ----------------
#define TARGET_SCORE 5
#define TIMER_ID     1
#define TIMER_MS     16   // ~60 FPS

#define PADDLE_W 15
#define PADDLE_H 80
#define BALL_S   14

#define PADDLE_SPEED 8
#define BALL_START_X 6
#define BALL_START_Y 4
#define MIN_BALL_SPEED 2
#define SLOW_FACTOR 0.90f
// ----------------------------------------

static int screenW, screenH;
static RECT paddle, ball;
static int vx, vy;
static int score = 0;
static bool up = false, down = false;

// --------- UTILS ---------
int sign(int v) { return (v >= 0) ? 1 : -1; }

// Ralentit la balle à chaque point
void slow_ball(void) {
    vx = sign(vx) * max(MIN_BALL_SPEED, (int)(abs(vx) * SLOW_FACTOR));
    vy = sign(vy) * max(MIN_BALL_SPEED, (int)(abs(vy) * SLOW_FACTOR));
}

void reset_ball(void) {
    ball.left   = screenW / 2;
    ball.top    = screenH / 2;
    ball.right  = ball.left + BALL_S;
    ball.bottom = ball.top  + BALL_S;

    vx = -BALL_START_X;
    vy =  BALL_START_Y;
}

void reset_game(void) {
    score = 0;
    reset_ball();
}
// -------------------------

void update(HWND hwnd) {
    // Paddle
    if (up)   OffsetRect(&paddle, 0, -PADDLE_SPEED);
    if (down) OffsetRect(&paddle, 0,  PADDLE_SPEED);

    if (paddle.top < 0) paddle.top = 0;
    if (paddle.bottom > screenH) {
        paddle.bottom = screenH;
        paddle.top = screenH - PADDLE_H;
    }

    // Ball
    OffsetRect(&ball, vx, vy);

    if (ball.top <= 0 || ball.bottom >= screenH)
        vy = -vy;

    RECT hit;
    if (IntersectRect(&hit, &ball, &paddle)) {
        vx = -vx;
        score++;
        slow_ball();

        if (score >= TARGET_SCORE)
            PostQuitMessage(0);
    }

    if (ball.right < 0)
        reset_game();

    InvalidateRect(hwnd, NULL, TRUE);
}

void draw(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    HBRUSH bg = CreateSolidBrush(RGB(0,0,0));
    FillRect(hdc, &ps.rcPaint, bg);
    DeleteObject(bg);

    HBRUSH white = CreateSolidBrush(RGB(255,255,255));
    FillRect(hdc, &paddle, white);
    FillRect(hdc, &ball, white);

    wchar_t txt[64];
    wsprintfW(txt, L"Score: %d / %d", score, TARGET_SCORE);
    SetTextColor(hdc, RGB(255,255,255));
    SetBkMode(hdc, TRANSPARENT);
    TextOutW(hdc, screenW/2 - 60, 20, txt, lstrlenW(txt));

    DeleteObject(white);
    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {

    case WM_CREATE: {
        RECT r; GetClientRect(hwnd, &r);
        screenW = r.right;
        screenH = r.bottom;

        paddle.left = 40;
        paddle.top = (screenH - PADDLE_H)/2;
        paddle.right = paddle.left + PADDLE_W;
        paddle.bottom = paddle.top + PADDLE_H;

        reset_game();
        SetTimer(hwnd, TIMER_ID, TIMER_MS, NULL);
        break;
    }

    case WM_TIMER:
        update(hwnd);
        break;

    case WM_PAINT:
        draw(hwnd);
        break;

    case WM_KEYDOWN:
        if (w == VK_UP)   up = true;
        if (w == VK_DOWN) down = true;
        if (w == 'P' || w == 'p' || w == VK_ESCAPE)
            PostQuitMessage(0);
        break;

    case WM_KEYUP:
        if (w == VK_UP)   up = false;
        if (w == VK_DOWN) down = false;
        break;

    case WM_SYSCOMMAND:
        if ((w & 0xFFF0) == SC_CLOSE) return 0;
        break;

    case WM_CLOSE:
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcW(hwnd, msg, w, l);
}

int WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = h;
    wc.lpszClassName = L"PongKiosk";
    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        WS_EX_TOPMOST,
        wc.lpszClassName,
        L"Pong Kiosk",
        WS_POPUP | WS_VISIBLE,
        0,0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,NULL,h,NULL
    );

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
        DispatchMessageW(&msg);

    return 0;
}
