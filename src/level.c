#include "level.h"
#include "utility.h"
#include <string.h>
#include <stdlib.h>

level_t* generate_level(int room_count, room_pool_t* room_pool){
    level_t* level = get_level_populated_with_rooms(room_count, room_pool);

    connect_doors(level);

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

void connect_doors(level_t* level){
    for (int i = 0; i < level->connected_rooms->size; i++){
        room_t* start_room = level->room_grid[level->connected_rooms->data[i].first.y][level->connected_rooms->data[i].first.x];

        room_t* end_room = level->room_grid[level->connected_rooms->data[i].second.y][level->connected_rooms->data[i].second.x];

        vector2_t forward_direction = sub( level->connected_rooms->data[i].second, level->connected_rooms->data[i].first );
        // Entry - position of a door that needs to be connected from
        vector2_t entry = get_level_position(level, level->connected_rooms->data[i].first, start_room->doors[forward_direction.y + 1][forward_direction.x + 1]);

        // Exit - position of a door that needs to be connected to
        vector2_t negative_path = scale(forward_direction, -1);
        vector2_t exit = get_level_position(level, level->connected_rooms->data[i].second, end_room->doors[negative_path.y + 1][negative_path.x + 1] );

        vector2_t current = entry;
        //   entry
        //    |
        // ___|
        //|
        //|
        //exit
        int first_steps = 1;
        if (equal(forward_direction, VEC2_UP) || equal(forward_direction, VEC2_DOWN)){
            first_steps += rand() % (sub(exit, entry).y * forward_direction.y - 1);
        }else{
            first_steps += rand() % (sub(exit, entry).x * forward_direction.x - 1);
        }

        for (int j = 0; j < first_steps; j++){
            level->data[current.y][current.x] = '#';
            current = sum( current, forward_direction);
        }

        vector2_t sideways_direction = { .x = forward_direction.y, .y = forward_direction.x };
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
    }
}

level_t* get_level_populated_with_rooms(int room_count, room_pool_t* room_pool){
    level_t* level = get_level_with_room_grid(room_count, room_pool);

    set_room_grid_cell_sizes(level);
    init_level_char_grid(level);

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

    vector2_pair_array_t* connected_rooms = init_vector2_pair_array();

    int branch_count = (room_count - 2)/2;
    if (room_placer(room_count, &branch_count, room_pool, room_count, room_grid, VEC2_ZERO, VEC2_SQUARE(room_count), connected_rooms) == 0){
        fail_gracefully("Couldn't generate level");
    }

    return cropped_level(map_size, room_count, room_grid, connected_rooms);
}

level_t* cropped_level(int map_size, int room_count, room_t *room_grid[map_size][map_size], vector2_pair_array_t* connected_rooms){

    vector2_t left_up = get_upper_left_corner_of_cropped_map(map_size, room_grid); // Cropping room_grid to contents
    vector2_t right_down = get_down_right_corner_of_cropped_map(map_size, room_grid);
    vector2_t cropped_map_size = sum( sub(right_down, left_up), VEC2_ONE );


    level_t* level = init_level_room_grid(room_count, cropped_map_size);
    level->connected_rooms = connected_rooms;
    level->start_room_grid_position = sub(VEC2_SQUARE(room_count), left_up );

    correct_connected_rooms_grid_pos(level, left_up);

    for (int i = left_up.y; i < right_down.y + 1; i++){
        for (int j = left_up.x; j < right_down.x + 1; j++){
            level->room_grid[i - left_up.y][j - left_up.x] = room_grid[i][j];
        }
    }

    return level;
}

void correct_connected_rooms_grid_pos(level_t* level, vector2_t left_up_corner){
    for (int i = 0; i < level->connected_rooms->size; i++){
        level->connected_rooms->data[i].first = sub( level->connected_rooms->data[i].first, left_up_corner );
        level->connected_rooms->data[i].second = sub( level->connected_rooms->data[i].second, left_up_corner );
    }
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

// set_level_grid_cell_sizes must be called before this one
void init_level_char_grid(level_t* level){
    int* positions[2];
    positions[0] = calloc(sizeof(int), level->room_grid_size.y + 1);
    positions[1] = calloc(sizeof(int), level->room_grid_size.x + 1);

    // From cell item_count now just calculating prefix sum
    for (int i = 0; i < level->room_grid_size.y; i++){
        positions[0][i + 1] = positions[0][i] + level->room_grid_positions[0][i];
    }

    for (int i = 0; i < level->room_grid_size.x; i++){
        positions[1][i + 1] = positions[1][i] + level->room_grid_positions[1][i];
    }

    free(level->room_grid_positions[0]);
    free(level->room_grid_positions[1]);

    level->room_grid_positions[0] = positions[0];
    level->room_grid_positions[1] = positions[1];

    level->size = (vector2_t) { .y = level->room_grid_positions[0][level->room_grid_size.y], .x = level->room_grid_positions[1][level->room_grid_size.x] };
    level->data = calloc(sizeof(char*), level->size.y);
    for (int i = 0; i < level->size.y; i++){
        level->data[i] = calloc(sizeof(char), level->size.x);
    }
}

void set_room_grid_cell_sizes(level_t* level){
    // Using this field just because it's easy; It is calculated from data this function will get anyway
    // So no useful data is corrupted
    level->room_grid_positions[0] = calloc(sizeof(int), level->room_grid_size.y);
    level->room_grid_positions[1] = calloc(sizeof(int), level->room_grid_size.x);

    // Finding cell sizes first
    for (int i = 0; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            if (level->room_grid[i][j]) {
                level->room_grid_positions[0][i] = level->room_grid_positions[0][i] > level->room_grid[i][j]->size.y ?
                                                   level->room_grid_positions[0][i] :
                                                   level->room_grid[i][j]->size.y;

                level->room_grid_positions[1][j] = level->room_grid_positions[1][j] > level->room_grid[i][j]->size.x ?
                                                   level->room_grid_positions[1][j] :
                                                   level->room_grid[i][j]->size.x;
            }
        }
    }

    for (int i = 0; i < level->room_grid_size.y; i++){
        level->room_grid_positions[0][i] += 1; // Padding for the paths
    }

    for (int i = 0; i < level->room_grid_size.x; i++){
        level->room_grid_positions[1][i] += 1;
    }
}

void destroy_level(level_t* level){
    for (int i = 0; i < level->room_grid_size.y; i++){
        free(level->room_grid[i]);
    }
    free(level->room_grid);

    free(level->room_grid_positions[0]);
    free(level->room_grid_positions[1]);

    for (int i = 0; i < level->size.y; i++){
        free(level->data[i]);
    }
    free(level->data);
    free(level);
}

// TODO we need to give map item_count instead of hard-coded value for rooms item_count
int room_placer(int rooms_left, int* branches_left, room_pool_t* room_pool, int room_count, room_t* rooms[room_count * 2 + 1][room_count * 2 + 1], vector2_t previous_direction, vector2_t pos, vector2_pair_array_t* connected_rooms){
    if (rooms[pos.y][pos.x] != NULL) {
        return 0;
    }

    int current_room_index = room_count - rooms_left;

    // First we need to build a room in cell
    room_t *next_room;

    if (current_room_index == 0){
        next_room = room_pool->start_room;
    }else if (current_room_index == room_count - 1){
        if (branches_left){
            next_room = room_pool->end_room;
        }else{
            next_room = room_pool->shrine_rooms[rand() % room_pool->shrine_count];
        }
    }else{
        vector2_t needed_door = scale(previous_direction, -1);
        do {
            next_room = room_pool->rooms[rand() % room_pool->count];
        } while (equal(next_room->doors[needed_door.y + 1][needed_door.x + 1], VEC2_UP)); // While there is no needed door
        // This while is not gonna loop because we assume that there are enough rooms with all kind of doors
    }
    rooms[pos.y][pos.x] = next_room;

    // If all that was needed is to build one room, then we succeeded
    if (rooms_left == 1){
        return 1;
    }

    // Then we need to go to another room, but let's try all of them
    vector2_t directions[4];
    get_shuffled_directions(directions);

    int successful = 0;
    for (int i = 0; i < 4; i++) {
        // First we check if we can place a room in this direction
        vector2_t next_cell = sum(pos, directions[i]);
        if (!is_valid_rect_index(next_cell, VEC2_SQUARE( room_count * 2 + 1))
            || equal(rooms[pos.y][pos.x]->doors[directions[i].y + 1][directions[i].x + 1], VEC2_UP)){
            continue;
        }

        // Then we start placing
        if (successful && branches_left && *branches_left > 0 && rand() % 2 > 0){
            // If we already placed main path we try our chance at branches
            push_back_vector2_pair(connected_rooms, (vector2_pair_t){ pos, next_cell });
            if (room_placer(3, 0, room_pool, room_count, rooms, directions[i], next_cell, connected_rooms)){
                (*branches_left)--;
            }else{
                delete_last_vector2_pair(connected_rooms);
            }
        }else if (!successful){
            // Let's go find out if this variant viable This is the main path
            push_back_vector2_pair(connected_rooms, (vector2_pair_t) { pos, next_cell });
            successful = room_placer(rooms_left - 1, branches_left, room_pool, room_count, rooms, directions[i], next_cell, connected_rooms);
            if (!successful){
                delete_last_vector2_pair(connected_rooms);
            }
        }
    }

    if (!successful){
        rooms[pos.y][pos.x] = NULL;
    }

    return successful;
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

    vector2_t room_position_on_level = (vector2_t) { .y = level->room_grid_positions[0][room_grid_pos.y], .x = level->room_grid_positions[1][room_grid_pos.x] };
    vector2_t result = sum( room_position_on_level, sum( VEC2_ONE, in_room_position ));

    if (!is_valid_rect_index(result, level->size)){
        fail_gracefully("There is no point in room: %d %d with pos %d %d", room_grid_pos.x, room_grid_pos.y, in_room_position.x, in_room_position.y);
    }

    return result;
}