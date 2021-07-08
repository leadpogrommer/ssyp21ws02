#pragma once
#include <ncurses.h>
#include "world.h"

typedef struct hud_t{
    WINDOW* window;
    world_t* world;
}hud_t;


hud_t* init_hud(WINDOW* game_window, int height, world_t* world);
void destroy_hud(hud_t* hud);

void draw_hud(hud_t* hud);