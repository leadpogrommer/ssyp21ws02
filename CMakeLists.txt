cmake_minimum_required(VERSION 3.19)
project(NCurces_roguelike C)

set(CMAKE_C_STANDARD 11)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

add_executable(game src/main.c src/room.c src/room.h src/utility.c src/utility.h src/render.c src/render.h src/world.c src/world.h src/player.c src/player.h)

target_link_libraries(game ${CURSES_LIBRARIES})