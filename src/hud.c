#include "hud.h"
#include <stdlib.h>

hud_t* init_hud(WINDOW* game_window, int height, world_t* world){
    hud_t* hud = malloc(sizeof(hud_t));

    vector2_t window_size = { .y = height, .x = getmaxx(game_window) };
    vector2_t window_pos = { .y = getmaxy(game_window) - height, .x = 0 };
    hud->window = newwin(window_size.y, window_size.x, window_pos.y, window_pos.x);
    hud->world = world;

    return hud;
}

void destroy_hud(hud_t* hud){
    free(hud);
}

void draw_hud(hud_t* hud){
    box(hud->window, 0, 0);
    mvwprintw(hud->window, 1, 1, "gold: %d", hud->world->player->gold);
    mvwprintw(hud->window, 1, 15, "hp: %d/%d", hud->world->player->health, hud->world->player->max_health);
    mvwprintw(hud->window, 1, getmaxx(hud->window) - 9, "lvl: %d", hud->world->level);


    wnoutrefresh(hud->window);
}