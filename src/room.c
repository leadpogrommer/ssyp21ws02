#include "room.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

room_t* load_room(const char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) fail_gracefully("File with this name does not exist: %s", filename);

    room_t* room = malloc(sizeof(room_t));
    fscanf(file, "%d %d", &room->width, &room->height);

    room->data = malloc(sizeof(char*) * room->height);

    while (getc(file) != '\n'); // To step into data itself

    for (int i = 0; i < room->height; i++){
        room->data[i] = malloc(sizeof(char) * room->width);
        for (int j = 0; j < room->width; j++){
            room->data[i][j] = getc(file);
        }

        if (i != room->height - 1) {
            while (getc(file) != '\n'); // Last line might not contain \n bc it's last line
        }
    }

    return room;
}

room_pool_t* create_room_pool(int room_count, ...){
    room_pool_t* room_pool = malloc(sizeof(room_pool_t));
    room_pool->count = room_count;
    room_pool->rooms = malloc(sizeof(room_t*) * room_count);

    va_list paths;
    va_start(paths, room_count);
    for (int i = 0; i < room_count; i++){
        room_pool->rooms[i] = load_room(va_arg(paths, const char*));
    }
    va_end(paths);
    return room_pool;
}

void print_room(room_t* room){
    for (int i = 0; i < room->height; i++){
        for (int j = 0; j < room->width; j++){
            putchar(room->data[i][j]);
        }
        putchar('\n');
    }
}

void destroy_room_pool(room_pool_t* room_pool){
    for (int i = 0; i < room_pool->count; i++) destroy_room(room_pool->rooms[i]);
    free(room_pool->rooms);
    free(room_pool);
}

void destroy_room(room_t* room){
    for (int i = 0; i < room->width; i++) free(room->data[i]);
    free(room->data);
    free(room);
}