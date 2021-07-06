#include "player.h"
#include <stdlib.h>

player_t* init_player(){
    player_t* player = malloc(sizeof(player_t));
    player->pos.x = 0;
    player->pos.y = 0;
    player->screen_pos.y = 0;
    player->screen_pos.x = 0;
    return player;
}

void destroy_player(player_t* player){
    free(player);
}

void move_player(player_t* player, vector2_t move){
    player->pos = sum(move, player->pos);
}

void move_player_to(player_t* player, vector2_t pos){
    player->pos = pos;
}