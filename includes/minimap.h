#pragma once

#include <utility.h>
#include <level.h>

typedef struct {
    WINDOW* window;
    vector2_t player_room;
    int radius;

    int** visited;
    int visited_w, visited_h;

    short color_id_base;
    short pair_id_base;
} minimap_data_t;

minimap_data_t* minimap_init(WINDOW* game_window, int radius, short color_id_base, short pair_id_base);
void minimap_destroy(minimap_data_t* minimap_data);
void minimap_reset_visited(minimap_data_t* minimap_data, int w, int h);
void minimap_draw(minimap_data_t* minimap_data, WINDOW* game_window, level_t* level, player_t* player);