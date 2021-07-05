#pragma once

typedef struct room_t{
    int width, height;
    char** data;
}room_t;

typedef struct room_pool_t{
    room_t** rooms;
    int count;
}room_pool_t;

void destroy_room_pool(room_pool_t* room_pool);
void destroy_room(room_t* room);

room_t* load_room(const char* filename);
room_pool_t* create_room_pool(int room_count, ...);

void print_room(room_t* room);
