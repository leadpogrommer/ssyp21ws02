#include "player.h"
#include <stdlib.h>

player_t* init_player(int max_health){
    player_t* player = calloc(sizeof(player_t), 1);
    player->gold = 1;
    player->max_health = max_health;
    player->health = max_health;
    player->inventory = init_inventory(10);
    player->vision_radius = 30;
    return player;
}

void destroy_player(player_t* player){
    destroy_inventory(player->inventory, 0);
    free(player);
}

void move_player(player_t* player, vector2_t move){
    player->pos = sum(move, player->pos);
}

void move_player_to(player_t* player, vector2_t pos){
    player->pos = pos;
}

void apply_item_to_player(player_t* player, item_t* item){
    player->max_health += item->hp_buff;
    player->damage += item->damage_buff;
}

void deapply_item_to_player(player_t* player, item_t* item){
    player->max_health -= item->hp_buff;
    player->damage -= item->damage_buff;
}

void heal_player(player_t* player, int amount){
    player->health = player->health + amount > player->max_health ? player->max_health : player->health + amount;
}

void pick_up_item(player_t* player, item_t* item){
    if (add_item_to_inventory(player->inventory, item)){
        apply_item_to_player(player, item);
    } // TODO if not - window or something with info that there is not enough space in inventory
}