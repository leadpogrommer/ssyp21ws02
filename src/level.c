#include "level.h"
#include "utility.h"
#include <string.h>
#include <stdlib.h>

level_t* generate_level(int room_count, room_pool_t* room_pool){
    level_t* level = get_level_populated_with_rooms(room_count, room_pool);

    connect_doors_along_the_path(level);

    shut_all_open_doors(level);

    return level;
}

void shut_all_open_doors(level_t* level){
    for (int i = 0; i < level->size.y; i++){
        for (int j = 0; j < level->size.x; j++){
            level->data[i][j] = level->data[i][j] == 'd' ? '*' : level->data[i][j];
        }
    }
}

void connect_doors_along_the_path(level_t* level){
    room_t* start_room = level->room_grid[level->start_room_grid_position.y][level->start_room_grid_position.x];
    vector2_t start_room_position = level->start_room_grid_position;

    for (int i = 0; i < level->room_count - 1; i++){
        vector2_t end_room_position = sum(start_room_position, level->path[i]);
        room_t* end_room = level->room_grid[end_room_position.y][end_room_position.x];

        // Entry - position of a door that needs to be connected from
        vector2_t entry = get_level_position(level, start_room_position, start_room->doors[level->path[i].y + 1][level->path[i].x + 1]);

        // Exit - position of a door that needs to be connected to
        vector2_t negative_path = scale(level->path[i], -1);
        vector2_t exit = get_level_position(level, end_room_position, end_room->doors[negative_path.y + 1][negative_path.x + 1] );

        vector2_t current = entry;
        vector2_t forward_direction = level->path[i];

        int first_steps = 1;
        if (equal(level->path[i], VEC2_UP) || equal(level->path[i], VEC2_DOWN)){
            first_steps += rand() % (sub(exit, entry).y * level->path[i].y - 1);
        }else{
            first_steps += rand() % (sub(exit, entry).x * level->path[i].x - 1);
        }

        for (int j = 0; j < first_steps; j++){
            level->data[current.y][current.x] = '#';
            current = sum( current, forward_direction);
        }

        vector2_t sideways_direction = { .x = level->path[i].y, .y = level->path[i].x};
        sideways_direction.y *= ((exit.y - current.y) * sideways_direction.y) > 0 ? 1 : -1;
        sideways_direction.x *= ((exit.x - current.x) * sideways_direction.x) > 0 ? 1 : -1;

        vector2_t move_direction = sideways_direction;
        while (!equal(current, exit)){
            level->data[current.y][current.x] = '#';
            if (!cross(sub(exit, current), forward_direction)){
                move_direction = forward_direction;
            }
            current = sum(current, move_direction);
        }

        level->data[current.y][current.x] = '#';
        start_room_position = end_room_position;
        start_room = end_room;
    }
}

level_t* get_level_populated_with_rooms(int room_count, room_pool_t* room_pool){
    level_t* level = get_level_with_room_grid(room_count, room_pool);

    init_level_char_grid(level, get_room_grid_cell_size(level));

    //FILE* file = fopen("output.txt", "w");

    for (int i = 0; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            if (level->room_grid[i][j]){
                vector2_t room_pos = get_level_position(level, (vector2_t) {.x = j, .y = i}, VEC2_ZERO);
                for (int k = 0; k < level->room_grid[i][j]->size.y; k++){
                    for (int l = 0; l < level->room_grid[i][j]->size.x; l++){
                        level->data[room_pos.y + k][room_pos.x + l] = level->room_grid[i][j]->data[k][l];
                    }
                }
            }
        }
    }

    return level;
}

level_t* get_level_with_room_grid(int room_count, room_pool_t* room_pool){
    int map_size = room_count * 2 + 1;
    room_t* room_grid[map_size][map_size];
    memset(room_grid, 0, sizeof(room_grid));

    vector2_t path[room_count];

    vector2_t start_position_abs = VEC2_SQUARE(room_count);
    if (room_placer(room_count, room_pool, room_count, room_grid, start_position_abs, path) == 0){
        fail_gracefully("Couldn't generate level");
    }

    vector2_t left_up = get_upper_left_corner_of_cropped_map(map_size, room_grid); // Cropping room_grid to contents
    vector2_t right_down = get_down_right_corner_of_cropped_map(map_size, room_grid);
    vector2_t cropped_map_size = sum( sub(right_down, left_up), VEC2_ONE );


    level_t* level = init_level_room_grid(room_count, cropped_map_size);
    level->path = path;
    level->start_room_grid_position = sub( start_position_abs, left_up );
    level->end_room_grid_position = level->start_room_grid_position;
    for (int i = 0; i < room_count - 1; i++){
        level->end_room_grid_position = sum( level->end_room_grid_position, path[i] );
    }

    for (int i = left_up.y; i < right_down.y + 1; i++){
        for (int j = left_up.x; j < right_down.x + 1; j++){
            level->room_grid[i - left_up.y][j - left_up.x] = room_grid[i][j];
        }
    }
    return level;
}

level_t* init_level_room_grid(int room_count, vector2_t room_grid_size){
    level_t* level = malloc(sizeof(level_t));
    level->room_count = room_count;
    level->room_grid_size = room_grid_size;
    level->room_grid = calloc(sizeof(room_t**), room_grid_size.y);
    for (int i = 0; i < level->room_grid_size.y; i++){
        level->room_grid[i] = calloc(sizeof(room_t*), level->room_grid_size.x);
    }

    return level;
}

void init_level_char_grid(level_t* level, vector2_t** room_grid_cell_size){
    level->rooms_positions = calloc(sizeof(vector2_t*), level->room_grid_size.y);
    for (int i = 0; i < level->room_grid_size.y; i++){
        level->rooms_positions[i] = calloc(sizeof(vector2_t), level->room_grid_size.x);
    }

    for (int i = 1; i < level->room_grid_size.x; i++){
        level->rooms_positions[0][i] = level->rooms_positions[0][i-1];
        level->rooms_positions[0][i].x += room_grid_cell_size[0][i-1].x;
    }

    for (int i = 1; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            level->rooms_positions[i][j] = level->rooms_positions[i-1][j];
            level->rooms_positions[i][j].y += room_grid_cell_size[i-1][j].y;
        }
    }


    level->size = sum(level->rooms_positions[level->room_grid_size.y-1][level->room_grid_size.x-1], room_grid_cell_size[level->room_grid_size.y-1][level->room_grid_size.x-1]);
    level->data = calloc(sizeof(char*), level->size.y);
    for (int i = 0; i < level->size.y; i++){
        level->data[i] = calloc(sizeof(char), level->size.x);
    }

    free(room_grid_cell_size);
}

vector2_t** get_room_grid_cell_size(level_t* level){
    vector2_t **room_grid_cell_size = calloc(sizeof(vector2_t*), level->room_grid_size.y);
    for (int i = 0; i < level->room_grid_size.y; i++){
        room_grid_cell_size[i] = calloc(sizeof(vector2_t), level->room_grid_size.x);
        for (int j = 0; j < level->room_grid_size.x; j++){
            if (level->room_grid[i][j]) {
                room_grid_cell_size[i][0].y = room_grid_cell_size[i][0].y > level->room_grid[i][j]->size.y ?
                                              room_grid_cell_size[i][0].y : level->room_grid[i][j]->size.y;
                room_grid_cell_size[0][j].x = room_grid_cell_size[0][j].x > level->room_grid[i][j]->size.x ?
                                              room_grid_cell_size[0][j].x : level->room_grid[i][j]->size.x;
            }
        }
    }

    for (int i = 0; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            room_grid_cell_size[i][j].y = room_grid_cell_size[i][0].y;
            room_grid_cell_size[i][j].x = room_grid_cell_size[0][j].x;
        }
    }

    for (int i = 0; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            room_grid_cell_size[i][j] = sum(room_grid_cell_size[i][j], VEC2_ONE);
        }
    }

    return room_grid_cell_size;
}

void destroy_level(level_t* level){
    for (int i = 0; i < level->room_grid_size.y; i++){
        free(level->rooms_positions[i]);
        free(level->room_grid[i]);
    }
    free(level->rooms_positions);
    free(level->room_grid);

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
    room_t *next_room;

    if (current_room_index == 0){
        if (room_pool->start_room){
            next_room = room_pool->start_room;
        }else{
            next_room = room_pool->rooms[rand() % room_pool->count];
        }
    }else if (current_room_index == room_count - 1){
        if (room_pool->end_room){
            next_room = room_pool->end_room;
        }else{
            next_room = room_pool->rooms[rand() % room_pool->count];
        }
    }else{
        vector2_t needed_door = scale(path[current_room_index - 1], -1); // Door should be open on the opposite side
        do {
            next_room = room_pool->rooms[rand() % room_pool->count];
        } while (equal(next_room->doors[needed_door.y + 1][needed_door.x + 1], VEC2_UP) == 1); // While there is no needed door
        // This while is not gonna loop because we assume that there are enough rooms with all kind of doors
    }
    rooms[pos.y][pos.x] = next_room;

    // If all that was needed is to build one room, then we succeeded
    if (rooms_left == 1){
        return 1;
    }

    // Then we need to go to another room, but let's try all of them
    vector2_t* directions = get_shuffled_directions();

    for (int i = 0; i < 4; i++) {
        vector2_t next_cell = sum(pos, directions[i]);
        if (!is_valid_rect_index(next_cell, VEC2_SQUARE( room_count * 2 + 1))
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

vector2_t get_level_position(level_t* level, vector2_t room_grid_pos, vector2_t in_room_position){
    if (!is_valid_rect_index(room_grid_pos, level->room_grid_size)){
        fail_gracefully("There is no room with position: x = %d y = %d ", room_grid_pos.x, room_grid_pos.y);
    }

    vector2_t room_position_on_level = level->rooms_positions[room_grid_pos.y][room_grid_pos.x];
    vector2_t result = sum( room_position_on_level, sum( VEC2_ONE, in_room_position ));

    if (!is_valid_rect_index(result, level->size)){
        fail_gracefully("There is no point in room: %d %d with pos %d %d", room_grid_pos.x, room_grid_pos.y, in_room_position.x, in_room_position.y);
    }

    return result;
}