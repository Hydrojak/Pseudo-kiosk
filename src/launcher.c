//
// Created by Hydrojak on 07/01/2026.
//
#define UNICODE
#define _UNICODE-

#include "launcher.h"
#include <stdio.h>

bool get_launcher_directory(wchar_t *buffer, DWORD size) {
    wchar_t path[MAX_PATH];

    if (!GetModuleFileNameW(NULL, path, MAX_PATH))
        return false;

    wchar_t *last = wcsrchr(path, L'\\');
    if (!last)
        return false;

    size_t len = last - path;
    if (len >= size)
        return false;

    wcsncpy(buffer, path, len);
    buffer[len] = L'\0';
    return true;
}

bool build_source_kiosk_path(const wchar_t *launcherDir, wchar_t *outPath, DWORD size) {
    return swprintf(outPath, size, L"%s\\kiosk.exe", launcherDir) >= 0;
}

bool build_temp_kiosk_path(wchar_t *outPath, DWORD size) {
    wchar_t tempDir[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, tempDir);
    if (n == 0 || n > MAX_PATH)
        return false;

    return swprintf(outPath, size, L"%skiosk.exe", tempDir) >= 0;
}

bool copy_kiosk_to_temp(const wchar_t *src, const wchar_t *dst) {
    return CopyFileW(src, dst, FALSE);
}

bool run_kiosk(const wchar_t *path) {
    STARTUPINFOW si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    if (!CreateProcessW(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
        return false;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

int wmain(void) {
    wchar_t launcherDir[MAX_PATH];
    wchar_t src[MAX_PATH];
    wchar_t dst[MAX_PATH];

    if (!get_launcher_directory(launcherDir, MAX_PATH)) {
        wprintf(L"Erreur: impossible d'obtenir le répertoire du launcher.\n");
        return 1;
    }

    if (!build_source_kiosk_path(launcherDir, src, MAX_PATH)) {
        wprintf(L"Erreur: chemin source kiosk invalide.\n");
        return 1;
    }

    if (!build_temp_kiosk_path(dst, MAX_PATH)) {
        wprintf(L"Erreur: chemin TEMP invalide.\n");
        return 1;
    }

    wprintf(L"Source : %s\n", src);
    wprintf(L"Destination : %s\n", dst);

    if (!copy_kiosk_to_temp(src, dst)) {
        wprintf(L"Erreur copie (%lu)\n", GetLastError());
        return 1;
    }

    if (!run_kiosk(dst)) {
        wprintf(L"Erreur lancement kiosk (%lu)\n", GetLastError());
        return 1;
    }

    wprintf(L"OK — kiosk lancé depuis TEMP.\n");
    return 0;
}
