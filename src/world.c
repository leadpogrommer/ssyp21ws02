#include "world.h"
#include <stdlib.h>
#include "rich_presence.h"
#include "saver.h"
#include "string.h"
#include <math.h>

world_t* start_new_world(){
    world_t* world = init_world();
    world->player = init_player(10);
    world->fade_speed = 10;
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
    if (world->fade_radius != world->max_fade_radius){
        if (world->fade_speed > 0){
            world->fade_radius += 2 * sqrt(world->fade_radius) * sqrt(world->fade_speed) + world->fade_speed;
        }else{
            world->fade_radius -= 2 * sqrt(world->fade_radius) * sqrt(-world->fade_speed) + world->fade_speed;
        }
    }else {
        world->time++;
        switch (world->level->data[world->player->pos.y][world->player->pos.x]) {
            case 'e':
                change_level(world);
                break;
            case 'h':
                print_message(world->hud, world->time, 0, 60, "Press e to buy max hp");
                break;
            case '?': {
                item_t *item = get_item_on_position(world->level, world->player->pos);
                print_message(world->hud, world->time, 0, 60, "Press e to buy %s", item->name);
                break;
            }
        }

        process_bullets(world->bullets, world->enemies, world->level, world->player, world->stats, world->time);
        process_enemies(world);

        if (world->level_popup){
            process_popup(&world->level_popup);
        }

        process_hud(world->hud, world->time);
    }

    if (world->fade_radius < abs(world->fade_speed) || world->fade_radius > world->max_fade_radius - abs(world->fade_speed)){
        world->fade_speed *= -1;
        world->fade_radius = world->fade_radius < abs(world->fade_speed) ? 0 : world->max_fade_radius;
        if (!world->fade_radius){
            generate_new_level(world, world->level->room_count + 1);
        }
    }

}

void use_shrine(world_t* world){
    int cost;
    switch (world->level->data[world->player->pos.y][world->player->pos.x]){
        case 'h':
            cost = 10;
            if (world->player->gold < cost){
                print_message(world->hud, world->time, 1, 120,"You have not enough gold, %d is needed", cost);
                break;
            }

            world->player->gold -= cost;
            world->player->max_health += 10;
            heal_player(world->player, 10);
            world->level->data[world->player->pos.y][world->player->pos.x] = '.';
            print_message(world->hud, world->time, 1, 120, "You used a health shrine");
            world->stats->shrines_used++;
            break;
        case '?':
            cost = get_item_on_position(world->level, world->player->pos)->cost;
            if (world->player->gold < cost){
                print_message(world->hud, world->time, 1, 120, "You have not enough gold, %d is needed", cost);
                break;
            }

            world->player->gold -= cost;
            pick_up_item(world->player, get_item_on_position(world->level, world->player->pos));
            world->level->data[world->player->pos.y][world->player->pos.x] = '.';
            print_message(world->hud, world->time, 1, 120, "Picked up an item: %s", world->player->inventory->items[world->player->inventory->item_count - 1]->name);
            world->stats->items_picked_up++;
            break;
        default:
            print_message(world->hud, world->time, 1, 30, "There is nothing to use");
            break;
    }
}

void change_level(world_t* world){
    world->fade_radius += world->fade_speed;
}

void generate_new_level(world_t* world, int room_count){
    if (world->level){
        destroy_pathfinder(world->pathfinder);
        destroy_level(world->level);
    }

    world->current_level++;
    world->level = generate_level(room_count, world->room_pool);
    world->pathfinder = init_pathfinder(world->level);
    spawn_enemies(world->level, world->enemies, 1);
    spawn_items_on_level(world->level, world->items);
    bullets_clear(world->bullets);

    world->stats->max_level = MAX(world->stats->max_level, world->current_level);

    // Move player to start
    move_player_to(world->player, get_level_position(world->level, world->level->start_room_grid_position,
                                                     VEC2_SQUARE(2)));

    update_rich_presence_level(world->current_level);
    world->level_popup = init_popup(NULL, 7, POPUP_ULCORNER, "Level %d", world->current_level);
}

void spawn_items_on_level(level_t* level, inventory_t* inventory){
    level->item_array = init_item_array();
    level->shrines_array = init_vector2_array();
    for (int i = 0; i < level->size.y; i++){
        for (int j = 0; j < level->size.x; j++){
            if (level->data[i][j] == '?'){
                push_back_item(level->item_array, *get_random_item(inventory));
                push_back_vector2(level->shrines_array, (vector2_t) {.x = j, .y = i});
            }
        }
    }
}

void load_items(world_t* world){
    world->items = init_inventory(4);
    item_t* power_up = calloc(sizeof(item_t), 1);
    power_up->name = "Apple of Edem";
    power_up->id = 0;
    power_up->callback_index = CALLBACK_HEAL_SMALL;
    power_up->cost = 10;
    item_t* item2 = calloc(sizeof(item_t), 1);
    item2->damage_buff = 10;
    item2->name = "Sword of the Storm";
    item2->id = 1;
    item2->callback_index = CALLBACK_NONE;
    item2->equippable = 1;
    item2->equipment_type = EQUIPMENT_WEAPON;
    item2->cost = 25;
    item_t* item3 = calloc(sizeof(item_t), 1);
    item3->damage_buff = -3;
    item3->name = "Shotgun";
    item3->id = 2;
    item3->callback_index = CALLBACK_SHOTGUN;
    item3->equippable = 1;
    item3->equipment_type = EQUIPMENT_WEAPON;
    item3->cost = 15;
    item_t* item4 = calloc(sizeof(item_t), 1);
    item4->id = 3;
    item4->callback_index = CALLBACK_GOD_VISION;
    item4->name = "Eye Of The God";
    item4->cost = 35;
    add_item_to_inventory(world->items, power_up);
    add_item_to_inventory(world->items, item2);
    add_item_to_inventory(world->items, item3);
    add_item_to_inventory(world->items, item4);
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
    minimap_destroy(world->minimap_data);
    free(world);
}

vector2_t get_origin_on_screen(world_t* world){
    return sub(world->player->screen_pos, world->player->pos);
}

void move_player_in_world(world_t* world, vector2_t move){
    if (world->fade_radius == world->max_fade_radius) {
        vector2_t end_position = sum(world->player->pos, move);
        if (world->level->data[end_position.y][end_position.x] != '*' &&
            world->level->data[end_position.y][end_position.x] != 0) {
            move_player(world->player, move);
        }
    }
}
