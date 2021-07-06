#include "level.h"
#include "utility.h"
#include <string.h>
#include <stdlib.h>

level_t* generate_level(int room_count, room_pool_t* room_pool){
    level_t* level = malloc(sizeof(level_t));
    room_t* rooms[room_count * 2 + 1][room_count * 2 + 1];
    memset(rooms, 0, sizeof(rooms));

    vector2_t path[room_count];

    vector2_t start_position_abs = {.x = room_count, .y = room_count};
    if (room_placer(room_count, room_pool, room_count, rooms, start_position_abs, path) == 0){
        fail_gracefully("Couldn't generate level");
    }

    vector2_t left_up = get_upper_left_corner_of_cropped_map(room_count * 2 + 1, rooms); // Cropping map to rooms
    vector2_t right_down = get_down_right_corner_of_cropped_map(room_count * 2 + 1, rooms);

    level->size = sub(right_down, left_up);
    level->size = sum(level->size, VEC2_ONE); // Setting as size of cropped map

    vector2_t cell_size = sum(room_pool->max_size, scale(VEC2_ONE, 2));
    level->size = scale_accordingly(level->size, cell_size);
    // Scaling according with maximum room so any room will fit + space for corridor

    level->data = calloc(sizeof(char*), level->size.y);
    for (int i = 0; i < level->size.y; i++){
        level->data[i] = calloc(sizeof(char), level->size.x);
    }

    vector2_t offset = VEC2_ONE;
    for (int i = left_up.y; i < right_down.y + 1; i++){
        offset.x = 1;
        for (int j = left_up.x; j < right_down.x + 1; j++){
            if (rooms[i][j] != NULL){
                for (int k = 0; k < rooms[i][j]->size.y; k++){
                    for (int l = 0; l < rooms[i][j]->size.x; l++){
                        level->data[offset.y + k][offset.x + l] = rooms[i][j]->data[k][l];
                    }
                }
            }
            offset.x += cell_size.x;
        }
        offset.y += cell_size.y;
    }

    room_t* start_room = rooms[start_position_abs.y][start_position_abs.x];
    vector2_t start_room_position_rel = sub(start_position_abs, left_up);

    for (int i = 0; i < room_count - 1; i++){
        vector2_t end_room_position_rel = sum(start_room_position_rel, path[i]);

        room_t* end_room = rooms[end_room_position_rel.y + left_up.y][end_room_position_rel.x + left_up.x];

        vector2_t start_room_position_on_level = scale_accordingly(start_room_position_rel, cell_size);
        vector2_t end_room_position_on_level = scale_accordingly(end_room_position_rel, cell_size);

        vector2_t entry = sum(start_room_position_on_level, start_room->doors[path[i].y + 1][path[i].x + 1]);
        entry = sum(entry, VEC2_ONE); // All rooms are drawn with padding of 1

        vector2_t negative_path = scale(path[i], -1);
        vector2_t exit = sum( end_room_position_on_level, end_room->doors[negative_path.y + 1][negative_path.x + 1] );
        exit = sum(exit, VEC2_ONE);


        level->data[entry.y][entry.x] = '#';
        vector2_t current = sum(entry, path[i]);

        if (equal(path[i], VEC2_UP) || equal(path[i], VEC2_DOWN)){
            while (current.x != exit.x){
                level->data[current.y][current.x] = '#';
                current.x += current.x > exit.x ? -1 : 1;
            }
            while (current.y != exit.y){
                level->data[current.y][current.x] = '#';
                current.y += current.y > exit.y ? -1 : 1;
            }
        }

        if (equal(path[i], VEC2_LEFT) || equal(path[i], VEC2_RIGHT)){
            while (current.y != exit.y){
                level->data[current.y][current.x] = '#';
                current.y += current.y > exit.y ? -1 : 1;
            }
            while (current.x != exit.x){
                level->data[current.y][current.x] = '#';
                current.x += current.x > exit.x ? -1 : 1;
            }
        }
        level->data[current.y][current.x] = '#';
        start_room_position_rel = end_room_position_rel;
        start_room = end_room;
    }

    return level;
}

void destroy_level(level_t* level){
    for (int i = 0; i < level->size.y; i++){
        free(level->data[i]);
    }
    free(level->data);
    free(level);
}

int room_placer(int rooms_left, room_pool_t* room_pool, int room_count, room_t* rooms[room_count * 2 + 1][room_count * 2 + 1], vector2_t pos, vector2_t path[room_count]){
    if (rooms[pos.y][pos.x] != NULL) {
        return 0;
    }

    int current_room_index = room_count - rooms_left;

    // First we need to build a room in cell
    room_t *random_room;
    if (current_room_index != 0) { // If it's first room there is no need to check doors
        vector2_t needed_door = scale(path[current_room_index - 1], -1); // Door should be open on the opposite side
        do {
            random_room = room_pool->rooms[rand() % room_pool->count];
        } while (equal(random_room->doors[needed_door.y + 1][needed_door.x + 1], VEC2_UP) == 1); // While there is no needed door
        // This while is not gonna loop because we assume that there are enough rooms with all kind of doors
    }else{
        random_room = room_pool->rooms[rand() % room_pool->count];
    }
    rooms[pos.y][pos.x] = random_room;

    // If all that was needed is to build one room, then we succeeded
    if (rooms_left == 1){
        return 1;
    }

    // Then we need to go to another room, but let's try all of them
    vector2_t* directions = get_shuffled_directions();

    for (int i = 0; i < 4; i++) {
        vector2_t next_cell = sum(pos, directions[i]);
        if (!is_valid_rect_index(next_cell, scale(VEC2_ONE, room_count * 2 + 1))
            || equal(rooms[pos.y][pos.x]->doors[directions[i].y + 1][directions[i].x + 1], VEC2_UP)){
            continue;
        }

        // Let's go find out if this variant viable
        path[current_room_index] = sub(next_cell, pos); // Remembering the path
        if (room_placer(rooms_left - 1, room_pool, room_count, rooms, next_cell, path) != 0) {
            free(directions);
            return 1;
        }
        path[current_room_index] = VEC2_ZERO;
    }

    // Well, we couldn't do it from here, let's rewind
    free(directions);
    rooms[pos.y][pos.x] = NULL;
    return 0;
}

void log_level_to_file(level_t* level, const char* filename){
    FILE* file = fopen(filename, "w");
    if (!file) fail_gracefully("Couldn't open file to log a level: %s", filename);

    for (int i = 0; i < level->size.y; i++){
        for (int j = 0; j < level->size.x; j++){
            if (!level->data[i][j]){
                putc(' ', file);
            }else{
                putc(level->data[i][j], file);
            }
        }
        putc('\n', file);
    }
    fclose(file);
}

void draw_level(WINDOW* window, palette_t* palette, level_t* level, vector2_t offset){
    for (int i = 0; i < level->size.y; i++){
        for (int j = 0; j < level->size.x; j++){
            if (!level->data[i][j]){
                mvwaddch(window, i + offset.y, j + offset.x, palette->symbol['.']);
            }else{
                mvwaddch(window, i + offset.y, j + offset.x, palette->symbol[level->data[i][j]]);
            }
        }
    }
}

vector2_t get_upper_left_corner_of_cropped_map(int map_size, room_t* rooms[map_size][map_size]){
    vector2_t result = { .x = -1, .y = -1 }; // it's because of it |
    int ready = 0;                           //                    |
    while (!ready){                      //                   \|/
        result.y++;                          //incrementing and then checking so first check will be with .y = 0
        for (int i = 0; i < map_size; i++){
            if (rooms[result.y][i] != NULL){
                ready = 1;
                break;
            }
        }
    }

    ready = 0;                              //It's the same but with x
    while (!ready){
        result.x++;
        for (int i = 0; i < map_size; i++){
            if (rooms[i][result.x] != NULL){
                ready = 1;
                break;
            }
        }
    }

    return result;
}

vector2_t get_down_right_corner_of_cropped_map(int map_size, room_t* rooms[map_size][map_size]){
    vector2_t result = { .x = map_size, .y = map_size }; // it's because of it |
    int ready = 0;                                       //                    |
    while (!ready){                                  //                   \|/
        result.y--;                                      //decrementing and then checking so first check will be valid
        for (int i = 0; i < map_size; i++){
            if (rooms[result.y][i] != NULL){
                ready = 1;
                break;
            }
        }
    }

    ready = 0;                              //It's the same but with x
    while (!ready){
        result.x--;
        for (int i = 0; i < map_size; i++){
            if (rooms[i][result.x] != NULL){
                ready = 1;
                break;
            }
        }
    }

    return result;
}