#include "world.h"
#include <stdlib.h>
#include "rich_presence.h"
#include "saver.h"

world_t* start_new_world(){
    world_t* world = init_world();
    world->player = init_player(10);
    generate_new_level(world, 2);

    return world;
}

world_t* init_world(){
    world_t* world = calloc(sizeof(world_t), 1);
    world->room_pool = load_room_directory();
    world->enemies = enemies_init();
    world->bullets = bullets_init();
    world->stats = load_statistics();
    load_items(world);

    return world;
}

void process_world(world_t* world){
    world->time++;
    switch (world->level->data[world->player->pos.y][world->player->pos.x]){
        case 'e':
            world->current_level++;
            generate_new_level(world, world->level->room_count + 1);
            break;
        case 'h':
            world->player->max_health += 10;
            world->level->data[world->player->pos.y][world->player->pos.x] = '.';
            print_message(world->hud, "You used a shrine of prosperity");
            world->stats->shrines_used++;
            break;
        case '?':
            pick_up_item(world->player, get_random_item(world->items));
            world->level->data[world->player->pos.y][world->player->pos.x] = '.';
            print_message(world->hud, "Picked up an item: %s", world->player->inventory->items[world->player->inventory->item_count - 1]->name);
            world->stats->items_picked_up++;
            break;
    }
    process_bullets(world->bullets, world->enemies, world->level, world->player, world->stats, world->time);
    process_enemies(world->pathfinder, world->enemies, world->player, world->time);
    if (world->level_popup){
        process_popup(&world->level_popup);
    }
    world->stats->max_gold = MAX(world->stats->max_gold, world->player->gold);
}

void generate_new_level(world_t* world, int room_count){
    if (world->level){
        destroy_pathfinder(world->pathfinder);
        destroy_level(world->level);
    }

    world->level = generate_level(room_count, world->room_pool);
    world->pathfinder = init_pathfinder(world->level);
    spawn_enemies(world->level, world->enemies);
    bullets_clear(world->bullets);

    world->stats->max_level = MAX(world->stats->max_level, world->current_level);

    // Move player to start
    move_player_to(world->player, get_level_position(world->level, world->level->start_room_grid_position,
                                                     VEC2_SQUARE(2)));

    update_rich_presence_level(world->current_level);
    world->level_popup = init_popup(NULL, 7, POPUP_ULCORNER, "Level %d", world->current_level);
}

void load_items(world_t* world){
    world->items = init_inventory(3);
    item_t* power_up = calloc(sizeof(item_t), 1);
    power_up->hp_buff = 10;
    power_up->name = "Apple of Edem";
    power_up->id = 0;
    power_up->callback_index = CALLBACK_HEAL_SMALL;
    item_t* item2 = calloc(sizeof(item_t), 1);
    item2->damage_buff = 15;
    item2->name = "Sword of the Storm";
    item2->id = 1;
    item2->callback_index = CALLBACK_NONE;
    item2->equippable = 1;
    item2->equipment_type = EQUIPMENT_WEAPON;
    item_t* item3 = calloc(sizeof(item_t), 1);
    item3->damage_buff = -3;
    item3->name = "Shotgun";
    item3->id = 2;
    item3->callback_index = CALLBACK_SHOTGUN;
    item3->equippable = 1;
    item3->equipment_type = EQUIPMENT_WEAPON;
    add_item_to_inventory(world->items, power_up);
    add_item_to_inventory(world->items, item2);
    add_item_to_inventory(world->items, item3);
}

void destroy_world(world_t* world){
    destroy_player(world->player);
    destroy_pathfinder(world->pathfinder);
    destroy_level(world->level);
    destroy_room_pool(world->room_pool);
    destroy_inventory(world->items, 1);
    destroy_hud(world->hud);
    enemies_destroy(world->enemies);
    bullets_destroy(world->bullets);
    destroy_statistics(world->stats);
    free(world);
}

vector2_t get_origin_on_screen(world_t* world){
    return sub(world->player->screen_pos, world->player->pos);
}

void move_player_in_world(world_t* world, vector2_t move){
    vector2_t end_position = sum(world->player->pos, move);
    if (world->level->data[end_position.y][end_position.x] != '*' &&
        world->level->data[end_position.y][end_position.x] != 0){
        move_player(world->player, move);
    }
}