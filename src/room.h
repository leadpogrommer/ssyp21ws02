#pragma once
#include <ncurses.h>
#include "render.h"
#include "utility.h"

typedef struct room_t{
    vector2_t size;
    char** data;
    vector2_t doors[3][3]; // Created so indices are vector2_t direction coords like so: [y+1][x+1]
}room_t;                   // Contains position of a door and VEC2_UP if it

typedef struct room_pool_t{
    room_t** rooms;
    int count;
    int capacity;
    vector2_t max_size;
    room_t* start_room;
    room_t* end_room;
}room_pool_t;

room_t* load_room(const char* filename);
void destroy_room(room_t* room);

room_pool_t* load_room_pool(int room_count, ...);
void destroy_room_pool(room_pool_t* room_pool);
void add_room_to_pool(room_pool_t* room_pool, room_t* room);

room_pool_t* load_room_directory(const char* directory);

void print_room(room_t* room);
void draw_room(WINDOW* window, palette_t* palette, room_t* room, vector2_t offset);
