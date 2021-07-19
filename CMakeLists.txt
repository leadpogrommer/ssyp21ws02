cmake_minimum_required(VERSION 3.16)
project(ncurses_roguelike C)

set(CMAKE_C_STANDARD 11)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)


find_package(bson-1.0 REQUIRED)

add_link_options(-L${CMAKE_SOURCE_DIR} -Wl,-rpath,.)

#add_library(discord SHARED IMPORTED)
#set_target_properties(discord PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libs/discord_game_sdk.so)
#set(CMAKE_INSTALL_RPATH ".")

include_directories(SYSTEM includes/thirdparty)

add_compile_options(-Wall -Werror -pedantic-errors -Wextra -Wpedantic -Wno-error=unused-result)

add_executable(game
                src/main.c
                src/room.c
                includes/room.h
                src/utility.c
                includes/utility.h
                src/palette.c
                includes/palette.h
                src/world.c
                includes/world.h
                src/player.c
                includes/player.h
                src/title_screen.c
                includes/title_screen.h
                src/rich_presence.c
                src/level.c
                includes/level.h
                src/heap.c
                includes/heap.h
                src/pathfinder.c
                includes/pathfinder.h
                src/hud.c
                includes/hud.h
                src/inventory.c
                includes/inventory.h
                src/render.c
                includes/render.h
                includes/main.h
                src/saver.c
                includes/saver.h
                src/enemies.c
                includes/enemies.h
                src/bullet.c
                includes/bullet.h
                src/achivements.c
                includes/achivements.h
                src/statistics.c
                includes/statistics.h
                src/popup.c
                includes/popup.h
                includes/generic_array.h
                src/minimap.c
                includes/minimap.h)
target_include_directories(game PRIVATE includes)
target_link_libraries(game mongo::bson_shared)


target_link_libraries(game ${CURSES_LIBRARIES}  m discord_game_sdk)
