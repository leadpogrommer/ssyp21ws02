#pragma once
#include <ncurses.h>
#include "render.h"
#include "utility.h"

typedef struct room_t{
    int width, height;
    char** data;
}room_t;

typedef struct room_pool_t{
    room_t** rooms;
    int count;
    int capacity;
}room_pool_t;

room_t* load_room(const char* filename);
void destroy_room(room_t* room);

room_pool_t* load_room_pool(int room_count, ...);
void destroy_room_pool(room_pool_t* room_pool);
void add_room_to_pool(room_pool_t* room_pool, room_t* room);

room_pool_t* load_room_directory(const char* directory);

void print_room(room_t* room);
void draw_room(WINDOW* window, palette_t* palette, room_t* room, vector2_t offset);
