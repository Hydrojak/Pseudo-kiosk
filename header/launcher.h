//
// Created by Hydrojak on 07/01/2026.
//
// launcher.h
// Helpers for kiosk launcher

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <windows.h>
#include <wchar.h>
#include <stdbool.h>

bool get_launcher_directory(wchar_t *buffer, DWORD size);
bool build_source_kiosk_path(const wchar_t *launcherDir, wchar_t *outPath, DWORD size);
bool build_temp_kiosk_path(wchar_t *outPath, DWORD size);
bool copy_kiosk_to_temp(const wchar_t *src, const wchar_t *dst);
bool run_kiosk(const wchar_t *path);

#endif // LAUNCHER_H
