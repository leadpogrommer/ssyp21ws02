#pragma once
#include "utility.h"
#include "inventory.h"

typedef struct player_t{
    vector2_t pos, screen_pos;
    int max_health, health, gold, damage;
    inventory_t* inventory;
} player_t;

player_t* init_player(int max_health);
void destroy_player(player_t* player);

void move_player(player_t* player, vector2_t);
void move_player_to(player_t* player, vector2_t);

void apply_item_to_player(player_t* player, item_t* item);
void pick_up_item(player_t* player, item_t* item);