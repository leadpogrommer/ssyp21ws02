#include "hud.h"
#include <stdlib.h>

hud_t* init_hud(WINDOW* game_window, int height, player_t* player, int* current_level, palette_t* palette){
    hud_t* hud = malloc(sizeof(hud_t));

    vector2_t window_size = { .y = height, .x = getmaxx(game_window) };
    vector2_t window_pos = { .y = getmaxy(game_window) - height, .x = 0 };
    hud->game_window = game_window;
    hud->height = height;
    hud->window = newwin(window_size.y, window_size.x, window_pos.y, window_pos.x);
    hud->player = player;
    hud->current_level = current_level;
    hud->palette = palette;

    wbkgd(hud->window, COLOR_PAIR(hud->palette->text_pair));

    fill_window_with_background_color(hud->window, palette);

    return hud;
}

void destroy_hud(hud_t* hud){
    free(hud);
}

void print_message(hud_t* hud, const char* format_string, ...){

    va_list args;
    va_start(args, format_string);

    vsprintf(hud->message, format_string, args);

    va_end(args);
}