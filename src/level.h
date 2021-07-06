#pragma once
#include "utility.h"
#include "room.h"

typedef struct level_t{
    char** data;
    vector2_t size;
}level_t;

level_t* generate_level(int room_count, room_pool_t* room_pool);
int room_placer(int rooms_left, room_pool_t* room_pool, int room_count, room_t* rooms[room_count * 2 + 1][room_count * 2 + 1], vector2_t pos, vector2_t path[room_count]);

void draw_level(WINDOW* window, palette_t* palette, level_t* level, vector2_t offset);

vector2_t* get_shuffled_directions();
vector2_t get_upper_left_corner_of_cropped_map(int map_size, room_t* rooms[map_size][map_size]);
vector2_t get_down_right_corner_of_cropped_map(int map_size, room_t* rooms[map_size][map_size]);