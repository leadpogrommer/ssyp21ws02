#pragma once
#include <ncurses.h>

typedef struct room_t{
    int width, height;
    char** data;
}room_t;

typedef struct room_pool_t{
    room_t** rooms;
    int count;
    int capacity;
}room_pool_t;

void destroy_room(room_t* room);
void destroy_room_pool(room_pool_t* room_pool);
void add_room_to_pool(room_pool_t* room_pool, room_t* room);
room_pool_t* load_room_directory(const char* directory);
void draw_room(WINDOW* window, room_t* room, int x_offset, int y_offset);


room_t* load_room(const char* filename);
room_pool_t* load_room_pool(int room_count, ...);

void print_room(room_t* room);
