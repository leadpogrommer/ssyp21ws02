#pragma once

typedef struct {
    palette_t* palette;
    palette_t* light_palette;
    world_t* world;
    WINDOW* game_window;
    int state;
} game_state_t;