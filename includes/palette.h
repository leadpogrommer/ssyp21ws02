#pragma once
#include <ncurses.h>

typedef struct palette_t{
    chtype symbol[256];
    int player_pair;
    int wall_pair;
    int floor_pair;
    int symbol_pair;
    int enemy_pair;
    int text_pair;
} palette_t;

palette_t* init_palette(short background_color, short floor_color, short player_color, short symbols_color, short enemies_color, short text_color);
void destroy_palette(palette_t* palette);

void fill_window_with_background_color(WINDOW* window, palette_t* palette);