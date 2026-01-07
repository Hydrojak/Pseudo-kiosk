cmake_minimum_required(VERSION 3.15)
project(Pseudo-kiosk)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -g")




include_directories(header)

set(MAIN_SOURCES
        src/launcher.c
)

add_executable(main ${MAIN_SOURCES})