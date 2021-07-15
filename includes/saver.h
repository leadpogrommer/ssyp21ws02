#pragma once
#include "world.h"

void save_world(world_t* world);
void save_player(player_t* player, FILE* file);
void save_inventory(inventory_t* inventory, FILE* file);
void save_level(level_t* level, FILE* file);
void save_room(room_t* room, FILE* file);
void save_statistics(statistics_t* statistics);
void save_enemies(enemies_t* enemies, FILE* file);

world_t* load_world();
player_t* load_player(FILE* file, inventory_t* parent_inventory);
inventory_t* load_inventory(FILE* file, inventory_t* parent_inventory);
level_t* load_level(FILE* file, room_pool_t* room_pool, inventory_t* parent_inventory);
room_t* load_saved_room(FILE* file, room_pool_t* room_pool);
statistics_t* load_statistics();
enemies_t* load_enemies(FILE* file);

