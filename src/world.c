#include "world.h"
#include <stdlib.h>

world_t* init_world(){
    world_t* world = malloc(sizeof(world_t));
    world->player = init_player();
    world->room_pool = load_room_directory("resources/rooms");
    load_level(world, 2);

    return world;
}

void process_world(world_t* world){
    if (equal(world->player->pos, world->end_position)){
        load_level(world, world->current_level->room_count + 1);
    }
}

void load_level(world_t* world, int room_count){
    if (world->current_level){
        destroy_level(world->current_level);
    }

    world->current_level = generate_level(room_count, world->room_pool);

    // Move player to start
    move_player_to(world->player, get_level_position(world->current_level, world->current_level->start_room_grid_position,
                                                     VEC2_SQUARE(2)));

    world->end_position = get_level_position(world->current_level, world->current_level->end_room_grid_position,
                                             VEC2_SQUARE(2));
}

void destroy_world(world_t* world){
    destroy_player(world->player);
    destroy_level(world->current_level);
    destroy_room_pool(world->room_pool);
    free(world);
}

vector2_t get_origin_on_screen(world_t* world){
    return sub(world->player->screen_pos, world->player->pos);
}

void move_player_in_world(world_t* world, vector2_t move){
    vector2_t end_position = sum(world->player->pos, move);
    if (world->current_level->data[end_position.y][end_position.x] != '*' &&
        world->current_level->data[end_position.y][end_position.x] != 0){
        move_player(world->player, move);
    }
}