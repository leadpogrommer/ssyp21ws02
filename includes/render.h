#pragma once

#include "world.h"
#include "palette.h"
#include "main.h"

void render(game_state_t* game_state);

void draw_level(WINDOW* window, palette_t* palette, level_t* level, vector2_t offset);
void draw_level_with_lighting(WINDOW* window, palette_t* palette, palette_t* light_palette, world_t* world, vector2_t offset);

void draw_hud(hud_t* hud);

void draw_room(WINDOW* window, palette_t* palette, room_t* room, vector2_t offset);

void draw_player(WINDOW* window, player_t* player, palette_t* palette);