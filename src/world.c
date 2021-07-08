#include "world.h"
#include <stdlib.h>

world_t* init_world(){
    world_t* world = malloc(sizeof(world_t));
    world->player = init_player(10);
    world->room_pool = load_room_directory("resources/rooms");
    load_level(world, 2);
    load_items(world);

    return world;
}

void process_world(world_t* world){
    switch (world->current_level->data[world->player->pos.y][world->player->pos.x]){
        case 'e':
            world->level++;
            load_level(world, world->current_level->room_count + 1);
            break;
        case 'h':
            world->player->max_health += 10;
            world->current_level->data[world->player->pos.y][world->player->pos.x] = '.';
            break;
        case '?':
            pick_up_item(world->player, get_random_item(world->items));
            world->current_level->data[world->player->pos.y][world->player->pos.x] = '.';
            print_message(world->hud, "Picked up an iteghhsssssssssshhhhhhhhm: %s", world->player->inventory->items[world->player->inventory->item_count - 1]->name);
            break;
    }
}

void load_level(world_t* world, int room_count){
    if (world->current_level){
        destroy_pathfinder(world->pathfinder);
        destroy_level(world->current_level);
    }

    world->current_level = generate_level(room_count, world->room_pool);
    world->pathfinder = init_pathfinder(world->current_level);

    // Move player to start
    move_player_to(world->player, get_level_position(world->current_level, world->current_level->start_room_grid_position,
                                                     VEC2_SQUARE(2)));
}

void load_items(world_t* world){
    world->items = init_inventory(1);
    item_t* power_up = calloc(sizeof(item_t), 1);
    power_up->hp_buff = 10;
    power_up->name = "Apple of Edem";
    add_item_to_inventory(world->items, power_up);
}

void destroy_world(world_t* world){
    destroy_player(world->player);
    destroy_level(world->current_level);
    destroy_room_pool(world->room_pool);
    destroy_inventory(world->items);
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