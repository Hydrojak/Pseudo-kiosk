cmake_minimum_required(VERSION 3.20)
project(kiosk_demo C)

set(CMAKE_C_STANDARD 11)
include_directories(header)
# launcher
add_executable(launcher
        src/launcher.c
        header/launcher.h)

# kiosk
add_executable(kiosk
        src/kiosk.c
        header/kiosk.h)

# WinAPI libs (souvent implicites, mais on force proprement)
target_link_libraries(kiosk user32 gdi32)