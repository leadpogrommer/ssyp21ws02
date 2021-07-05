#pragma once
#include "utility.h"

typedef struct player_t{
    vector2_t pos, screen_pos;
} player_t;

player_t* init_player();
void destroy_player(player_t* player);

void move_player(player_t* player, vector2_t);
void move_player_to(player_t* player, vector2_t);