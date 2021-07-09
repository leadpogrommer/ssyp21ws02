#include "render.h"
#include <stdlib.h>

palette_t* init_palette(short background_color, short floor_color, short player_color, short symbols_color, short enemies_color, short text_color){
    palette_t* palette = malloc(sizeof(palette_t));

    init_pair(1, floor_color, floor_color);
    init_pair(2, symbols_color, floor_color);
    init_pair(3, player_color, floor_color);
    init_pair(4, background_color, background_color);
    init_pair(5, enemies_color, floor_color);
    init_pair(6, text_color, background_color);

    palette->floor_pair = 1;
    palette->symbol_pair = 2;
    palette->player_pair = 3;
    palette->wall_pair = 4;
    palette->enemy_pair = 5;
    palette->text_pair = 6;


    for (int i = 0; i < 256; i++){
        palette->symbol[i] = i | COLOR_PAIR(palette->symbol_pair);
    }

    for (int i = 'A'; i < 'Z' + 1; i++){
        palette->symbol[i] = i | COLOR_PAIR(palette->enemy_pair);
    }

    palette->symbol['P'] = 'P' | COLOR_PAIR(palette->player_pair);
    palette->symbol['.'] = ' ' | COLOR_PAIR(palette->floor_pair);
    palette->symbol['#'] = ' ' | COLOR_PAIR(palette->floor_pair);
    palette->symbol['*'] = ' ' | COLOR_PAIR(palette->wall_pair);

    return palette;
}

void destroy_palette(palette_t* palette){
    free(palette);
}

void fill_window_with_background_color(WINDOW* window, palette_t* palette){
    for (int i = 0; i < getmaxy(window); i++){
        for (int j = 0; j < getmaxx(window); j++){
            mvwaddch(window, i, j, ' ' | COLOR_PAIR(palette->wall_pair));
        }
    }
}