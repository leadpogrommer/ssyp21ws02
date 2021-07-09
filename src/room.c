#include "room.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>


room_t* load_room(const char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL) fail_gracefully("Cannot open this file: %s", filename);

    room_t* room = malloc(sizeof(room_t));
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            room->doors[i][j] = VEC2_UP;
        }
    }

    fscanf(file, "%d %d %hd", &room->size.x, &room->size.y, &room->is_shrine);

    room->data = malloc(sizeof(char*) * room->size.y);

    while (getc(file) != '\n'); // To step into data itself

    for (int i = 0; i < room->size.y; i++){
        room->data[i] = malloc(sizeof(char) * room->size.x);
        for (int j = 0; j < room->size.x; j++){
            room->data[i][j] = getc(file);

            if (room->data[i][j] == 'd'){
                vector2_t direction = VEC2_ZERO;
                if (i == 0){
                    direction = VEC2_UP;
                }else if(i == room->size.y - 1){
                    direction = VEC2_DOWN;
                }else if (j == 0){
                    direction = VEC2_LEFT;
                }else if(j == room->size.x - 1){
                    direction = VEC2_RIGHT;
                }
                room->doors[direction.y + 1][direction.x + 1] = (vector2_t) {.x = j, .y = i};
            }

        }

        if (i != room->size.y - 1) {
            while (getc(file) != '\n'); // Last line might not contain \n bc it's last line
        }
    }

    return room;
}

room_pool_t* load_room_pool(int room_count, ...){
    room_pool_t* room_pool = init_room_pool();

    va_list paths;
    va_start(paths, room_count);
    for (int i = 0; i < room_count; i++){
        add_room_to_pool(room_pool, load_room(va_arg(paths, const char*)));
    }
    va_end(paths);
    return room_pool;
}

void add_room_to_pool(room_pool_t* room_pool, room_t* room){
    if (!room->is_shrine) {
        if (room_pool->count == room_pool->capacity) {
            room_pool->capacity *= 2;
            room_pool->rooms = realloc(room_pool->rooms, sizeof(room_t *) * room_pool->capacity);
        }

        room_pool->rooms[room_pool->count++] = room;
    }else{
        if (room_pool->shrine_count == room_pool->shrine_capacity) {
            room_pool->shrine_capacity *= 2;
            room_pool->shrine_rooms = realloc(room_pool->shrine_rooms, sizeof(room_t *) * room_pool->shrine_capacity);
        }

        room_pool->shrine_rooms[room_pool->shrine_count++] = room;
    }
}

room_pool_t* load_room_directory(const char* directory){
    DIR* dir = opendir(directory);
    if (dir == NULL) fail_gracefully("Cannot open directory: %s", directory);

    char root_directory[256];
    getcwd(root_directory, 256); // We need to save initial directory

    room_pool_t* room_pool = load_room_pool(0);

    chdir(directory); // Going in target directory

    struct dirent* current_el = readdir(dir); // Reading next position
    while (current_el != NULL){
        if (current_el->d_type == DT_REG){ // Element needs to be a file
            if (!strcmp(current_el->d_name, "start_room.txt")){
                room_pool->start_room = load_room(current_el->d_name);
            }else if (!strcmp(current_el->d_name, "end_room.txt")){
                room_pool->end_room = load_room(current_el->d_name);
            }else{
                add_room_to_pool(room_pool, load_room(current_el->d_name));
            }
        }
        current_el = readdir(dir);
    }

    if (!room_pool->start_room || !room_pool->end_room){
        fail_gracefully("No start or end room was found. Game resources corrupted");
    }

    chdir(root_directory); // Going back to initial directory
    return room_pool;
}

void print_room(room_t* room){
    for (int i = 0; i < room->size.y; i++){
        for (int j = 0; j < room->size.x; j++){
            putchar(room->data[i][j]);
        }
        putchar('\n');
    }
}

room_pool_t* init_room_pool(){
    room_pool_t* room_pool = malloc(sizeof(room_pool_t));
    room_pool->count = 0;
    room_pool->capacity = 1;
    room_pool->shrine_count = 0;
    room_pool->shrine_capacity = 1;
    room_pool->rooms = malloc(sizeof(room_t*) * room_pool->capacity);
    room_pool->shrine_rooms = malloc(sizeof(room_t*) * room_pool->shrine_capacity);

    return room_pool;
}

void destroy_room_pool(room_pool_t* room_pool){
    for (int i = 0; i < room_pool->count; i++){
        destroy_room(room_pool->rooms[i]);
    }

    for (int i = 0; i < room_pool->shrine_count; i++){
        destroy_room(room_pool->shrine_rooms[i]);
    }
    free(room_pool->rooms);
    free(room_pool->shrine_rooms);
    free(room_pool);
}

void destroy_room(room_t* room){
    for (int i = 0; i < room->size.y; i++) free(room->data[i]);
    free(room->data);
    free(room);
}