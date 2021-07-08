cmake_minimum_required(VERSION 3.19)
project(ncurses_roguelike C)

set(CMAKE_C_STANDARD 11)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)

add_link_options(-L${CMAKE_SOURCE_DIR} -Wl,-rpath,.)

#add_library(discord SHARED IMPORTED)
#set_target_properties(discord PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/discord_game_sdk.so)
#set(CMAKE_INSTALL_RPATH ".")

add_executable(game
                src/main.c
                src/room.c
                includes/room.h
                src/utility.c
                includes/utility.h
                src/render.c
                includes/render.h
                src/world.c
                includes/world.h
                src/player.c
                includes/player.h
                src/title_screen.c
                includes/title_screen.h
                src/rich_presence.c
                src/level.c
                includes/level.h src/heap.c includes/heap.h src/pathfinder.c includes/pathfinder.h src/hud.c includes/hud.h)
target_include_directories(game PRIVATE includes)


target_link_libraries(game ${CURSES_LIBRARIES}  m discord_game_sdk)
