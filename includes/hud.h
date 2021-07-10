#pragma once
#include <ncurses.h>
#include "player.h"
#include "palette.h"


typedef struct hud_t{
    WINDOW* window;
    WINDOW* game_window;
    player_t* player;
    int* current_level;
    palette_t* palette;
    int height;
    char message[256];
}hud_t;


hud_t* init_hud(WINDOW* game_window, int height, player_t* player, int* current_level, palette_t* palette);
void destroy_hud(hud_t* hud);

void print_message(hud_t* hud, const char* format_string, ...);