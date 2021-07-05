#pragma once

typedef struct player_t{
    int x, y, x_screen, y_screen;
} player_t;

player_t* init_player();
void destroy_player(player_t* player);

void move_player(player_t* player, int x, int y);
void move_player_to(player_t* player, int x, int y);