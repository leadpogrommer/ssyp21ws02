#pragma once

#include "world.h"
#include "palette.h"
#include "main.h"
#include "popup.h"

void render(game_state_t* game_state);

void draw_level(WINDOW* window, palette_t* palette, level_t* level, vector2_t offset);
void draw_game_window(game_state_t* game_state);

void bake_lighting(world_t* world, short** is_visible);
void draw_level_with_lighting(WINDOW* window, palette_t* palette, palette_t* light_palette, level_t* level, vector2_t offset,
                              short** is_visible);
void draw_enemies_with_lighting(WINDOW* window, palette_t* palette, palette_t* light_palette, enemies_t* enemies, vector2_t offset,
                                short** is_visible);
void draw_bullets_with_lighting(WINDOW* window, palette_t* palette, palette_t* light_palette, bullets_t* bullets, vector2_t offset,
                                short** is_visible);

void draw_hud(hud_t* hud);

void draw_room(WINDOW* window, palette_t* palette, room_t* room, vector2_t offset);

void draw_player(WINDOW* window, player_t* player, palette_t* palette);
void draw_inventory(inventory_display_t* inventory_display);

void draw_popup(popup_t* popup, palette_t* popup_palette, palette_t* main_palette);
void draw_level_changing_animation(game_state_t* game_state);
