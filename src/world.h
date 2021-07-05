#pragma once
#include "player.h"

typedef struct world_t{
    unsigned long long time;
    player_t* player;
}world_t;

world_t* init_world();
void destroy_world(world_t* world);
