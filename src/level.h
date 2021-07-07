#pragma once
#include "utility.h"
#include "room.h"

typedef struct level_t{
    char** data;
    vector2_t size;
    int room_count;
    room_t*** room_grid;
    vector2_t room_grid_size;
    vector2_t room_grid_cell_size;
    vector2_t start_room_grid_position;
    vector2_t* path;
}level_t;

level_t* generate_level(int room_count, room_pool_t* room_pool);
level_t* init_level(int room_count, vector2_t room_grid_size, vector2_t room_grid_cell_size);
level_t* get_level_populated_with_rooms(int room_count, room_pool_t* room_pool);
void connect_doors_along_the_path(level_t* level);
void shut_all_open_doors(level_t* level);
int room_placer(int rooms_left, room_pool_t* room_pool, int room_count, room_t* rooms[room_count * 2 + 1][room_count * 2 + 1], vector2_t pos, vector2_t path[room_count]);

void draw_level(WINDOW* window, palette_t* palette, level_t* level, vector2_t offset);

void log_level_to_file(level_t* level, const char* filename);

vector2_t get_upper_left_corner_of_cropped_map(int map_size, room_t* rooms[map_size][map_size]);
vector2_t get_down_right_corner_of_cropped_map(int map_size, room_t* rooms[map_size][map_size]);

void destroy_level(level_t* level);