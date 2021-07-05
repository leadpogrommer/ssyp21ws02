#pragma once
#include <ncurses.h>

typedef struct palette_t{
    chtype symbol[256];
} palette_t;

palette_t* init_palette();
void destroy_palette(palette_t* palette);