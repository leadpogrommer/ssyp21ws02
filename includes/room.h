#pragma once
#include <ncurses.h>
#include "palette.h"
#include "utility.h"

typedef struct room_t{
    char filename[256];
    vector2_t size;
    char** data;
    short is_shrine;
    vector2_t doors[3][3]; // Created so indices are vector2_t direction coords like so: [y+1][x+1]
}room_t;                   // Contains position of a door and VEC2_UP if it

typedef struct room_pool_t{
    room_t** rooms;
    room_t** shrine_rooms;
    int count, capacity;
    int shrine_count, shrine_capacity;
    room_t* start_room;
    room_t* end_room;
}room_pool_t;

room_t* load_room(const char* filename);
void destroy_room(room_t* room);

room_pool_t* load_room_pool(int room_count, ...);
room_pool_t* init_room_pool();
void destroy_room_pool(room_pool_t* room_pool);
void add_room_to_pool(room_pool_t* room_pool, room_t* room);
room_t* get_room_by_name(room_pool_t* room_pool, const char* filename);

room_pool_t* load_room_directory();

void print_room(room_t* room);
