#pragma once
#include <ncurses.h>
#include "player.h"

typedef struct hud_t{
    WINDOW* window;
    player_t* player;
    int* current_level;
}hud_t;


hud_t* init_hud(WINDOW* game_window, int height, player_t* player, int* current_level);
void destroy_hud(hud_t* hud);

void draw_hud(hud_t* hud);

void print_message(hud_t* hud, const char* format_string, ...);