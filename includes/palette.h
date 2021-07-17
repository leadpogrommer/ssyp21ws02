#pragma once
#include <ncurses.h>

typedef struct palette_t{
    chtype symbol[256];
    int player_pair;
    int wall_pair;
    int floor_pair;
    int symbol_pair;
    int text_pair;
    int bullet_pair;
    short enemy_brightest_red;
    short floor_color;
    int brightest_enemy_pair;
} palette_t;

palette_t* init_palette(short background_color, short floor_color, short player_color, short symbols_color,
                        short enemies_brightest_red, short text_color, short bullet_color);
void destroy_palette(palette_t* palette);

void fill_window_with_background_color(WINDOW* window, palette_t* palette);
short get_pair_count();
