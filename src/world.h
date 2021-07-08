#pragma once
#include "player.h"
#include "level.h"
#include "pathfinder.h"

typedef struct world_t{
    unsigned long long time;
    player_t* player;
    level_t* current_level;
    pathfinder_t* pathfinder;
    room_pool_t* room_pool;
}world_t;

world_t* init_world();
void destroy_world(world_t* world);
void load_level(world_t* world, int room_count);
void process_world(world_t* world);

vector2_t get_origin_on_screen(world_t* world);

void move_player_in_world(world_t* world, vector2_t move);