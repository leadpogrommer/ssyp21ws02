#include "room.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <unistd.h>
#include <ncurses.h>

room_t* load_room(const char* filename){
    FILE* file = fopen(filename, "r");
    char directory[256];
    getcwd(directory, 256);
    if (file == NULL) fail_gracefully("Cannot open this file: %s", filename);

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

room_pool_t* load_room_pool(int room_count, ...){
    room_pool_t* room_pool = malloc(sizeof(room_pool_t));
    room_pool->count = room_count;
    room_pool->capacity = room_count + 1;
    room_pool->rooms = malloc(sizeof(room_t*) * room_pool->capacity);

    va_list paths;
    va_start(paths, room_count);
    for (int i = 0; i < room_count; i++){
        room_pool->rooms[i] = load_room(va_arg(paths, const char*));
    }
    va_end(paths);
    return room_pool;
}

void add_room_to_pool(room_pool_t* room_pool, room_t* room){
    if (room_pool->count == room_pool->capacity){
        room_pool->capacity *= 2;
        room_pool->rooms = realloc(room_pool->rooms, sizeof(room_t*) * room_pool->capacity);
    }

    room_pool->rooms[room_pool->count++] = room;
}

room_pool_t* load_room_directory(const char* directory){
    DIR* dir = opendir(directory);
    if (dir == NULL) fail_gracefully("Cannot open directory: %s", directory);

    char root_directory[256];
    getcwd(root_directory, 256); // We need to save initial directory

    room_pool_t* room_pool = load_room_pool(0);

    chdir(directory); // Going in target directory

    struct dirent* current_el = readdir(dir); // Reading next element
    while (current_el != NULL){
        if (current_el->d_type == DT_REG){ // Element needs to be a file
            add_room_to_pool(room_pool, load_room(current_el->d_name));
        }
        current_el = readdir(dir);
    }

    chdir(root_directory); // Going back to initial directory
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

void draw_room(WINDOW* window, room_t* room, int x_offset, int y_offset){
    for (int i = 0; i < room->height; i++){
        for (int j = 0; j < room->width; j++){
            mvwaddch(window, y_offset + i, x_offset + j, room->data[i][j]);
        }
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