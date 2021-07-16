#pragma once

#include "utility.h"
#include "inventory.h"

typedef struct bullets_t bullets_t;

typedef struct player_t{
    vector2_t pos, screen_pos;
    int max_health, health, gold, damage, vision_radius, weapon_type;
    char god_vision;
    unsigned long long last_shot;
    int cooldowns[2];
    inventory_t* inventory;
} player_t;

player_t* init_player(int max_health);
void destroy_player(player_t* player);

void move_player(player_t* player, vector2_t);
void move_player_to(player_t* player, vector2_t);

void apply_item_to_player(player_t* player, item_t* item);
void pick_up_item(player_t* player, item_t* item);
void deapply_item_to_player(player_t* player, item_t* item);

void heal_player(player_t* player, int amount);

void shoot(player_t* player, bullets_t* bullets, vector2_t direction, unsigned long long time);