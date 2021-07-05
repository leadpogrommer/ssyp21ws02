#include "player.h"
#include <stdlib.h>

player_t* init_player(){
    player_t* player = malloc(sizeof(player_t));
    player->x = 0;
    player->y = 0;
    return player;
}

void destroy_player(player_t* player){
    free(player);
}

void move_player(player_t* player, int x, int y){
    player->x += x;
    player->y += y;
}

void move_player_to(player_t* player, int x, int y){
    player->x = x;
    player->y = y;
}
