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

typedef struct inventory_display_t{
    WINDOW *window, *game_window;
    inventory_t* inventory;
    palette_t* palette;
    vector2_t left_up_corner, right_down_corner, size;
    int current_item;
}inventory_display_t;

hud_t* init_hud(WINDOW* game_window, int height, player_t* player, int* current_level, palette_t* palette);
void destroy_hud(hud_t* hud);

void print_message(hud_t* hud, const char* format_string, ...);

inventory_display_t* init_inventory_display(inventory_t* inventory, WINDOW* game_window, palette_t* palette, vector2_t left_up_padding, vector2_t right_down_padding);
void destroy_inventory_display(inventory_display_t* display);