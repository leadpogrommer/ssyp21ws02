#include "palette.h"
#include <stdlib.h>

static short pair_count = 1;

palette_t* init_palette(short background_color, short floor_color, short player_color, short symbols_color, short brightest_enemy_color,
                        short text_color, short bullet_color){
    palette_t* palette = malloc(sizeof(palette_t));

    palette->floor_pair = pair_count;
    init_pair(pair_count++, floor_color, floor_color);
    palette->symbol_pair = pair_count;
    init_pair(pair_count++, symbols_color, floor_color);
    palette->player_pair = pair_count;
    init_pair(pair_count++, player_color, floor_color);
    palette->wall_pair = pair_count;
    init_pair(pair_count++, background_color, background_color);
    palette->text_pair = pair_count;
    init_pair(pair_count++, text_color, background_color);
    palette->bullet_pair = pair_count;
    init_pair(pair_count++, bullet_color, floor_color);
    palette->brightest_enemy_pair = pair_count;
    init_pair(pair_count++, brightest_enemy_color, floor_color);

    short tmp;
    color_content(brightest_enemy_color, &(palette->enemy_brightest_red), &tmp, &tmp);
    palette->floor_color = floor_color;

    for (int i = 0; i < 256; i++){
        palette->symbol[i] = i | COLOR_PAIR(palette->symbol_pair);
    }

    palette->symbol['P'] = 'P' | COLOR_PAIR(palette->player_pair);
    palette->symbol['.'] = ' ' | COLOR_PAIR(palette->floor_pair);
    palette->symbol['#'] = ' ' | COLOR_PAIR(palette->floor_pair);
    palette->symbol['*'] = ' ' | COLOR_PAIR(palette->wall_pair);
    palette->symbol['-'] = '*' | COLOR_PAIR(palette->bullet_pair);
    palette->symbol['_'] = '*' | COLOR_PAIR(palette->brightest_enemy_pair);

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

short get_pair_count(){
    return pair_count;
}
