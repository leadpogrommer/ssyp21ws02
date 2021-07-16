#pragma once
#include "achivements.h"

#define STATE_MAIN_MENU 2
#define STATE_PAUSE_MENU 3
#define STATE_GAME 0
#define STATE_INVENTORY 4
#define STATE_EXIT 1
#define STATE_GAMEOVER 5

typedef struct {
    palette_t* palette, *light_palette, *level_popup_palette;
    world_t* world;
    WINDOW* game_window;
    inventory_display_t* inventory_display;
    achievements_t* achievements;
    popup_t* achievement_popup;
    int_array_t* achievement_queue;
    int state;
} game_state_t;
