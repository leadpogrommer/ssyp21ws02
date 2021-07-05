cmake_minimum_required(VERSION 3.19)
project(NCources_roguelike C)

set(CMAKE_C_STANDARD 11)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

add_executable(game main.c)

target_link_libraries(game ${CURSES_LIBRARIES})