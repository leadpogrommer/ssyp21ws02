#pragma once
#include "world.h"

typedef struct save{
    unsigned long long time;


}save;

void save_world(world_t* world);
void save_player(player_t* player, FILE* file);
void save_inventory(inventory_t* inventory, FILE* file);
void save_level(level_t* level, FILE* file);
void save_room(room_t* room, FILE* file);

world_t* load_world();
player_t* load_player(FILE* file, inventory_t* parent_inventory);
inventory_t* load_inventory(FILE* file, inventory_t* parent_inventory);
level_t* load_level(FILE* file, room_pool_t* room_pool);
room_t* load_saved_room(FILE* file, room_pool_t* room_pool);