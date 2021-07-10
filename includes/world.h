#pragma once
#include "player.h"
#include "level.h"
#include "pathfinder.h"
#include "inventory.h"
#include "hud.h"

typedef struct world_t{
    unsigned long long time;
    player_t* player;
    level_t* level;
    int current_level;
    pathfinder_t* pathfinder;
    room_pool_t* room_pool;
    inventory_t* items;
    hud_t* hud;
}world_t;

world_t* start_new_world();

world_t* init_world();
void destroy_world(world_t* world);

void generate_new_level(world_t* world, int room_count);
void load_items(world_t* world);

void process_world(world_t* world);

vector2_t get_origin_on_screen(world_t* world);

void move_player_in_world(world_t* world, vector2_t move);