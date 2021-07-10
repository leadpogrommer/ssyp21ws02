#include "inventory.h"
#include <stdlib.h>
#include "player.h"
#include "world.h"

void ability0(world_t* world){
    heal_player(world->player, 10);
}

void ability1(world_t* world){
    int count = world->enemies->count;
    for (int i = 0; i < count; i++){
        enemies_remove(world->enemies, 0);
    }
}

static void (*callbacks[2])(world_t* world) = { ability0, ability1 };

inventory_t* init_inventory(int size){
    inventory_t* inventory = calloc(sizeof(inventory_t), 1);
    inventory->size = size;
    inventory->items = calloc(sizeof(item_t*), inventory->size);
    inventory->item_count = 0;

    return inventory;
}

int add_item_to_inventory(inventory_t* inventory, item_t* item){
    if (inventory->item_count == inventory->size){
        return 0;
    }

    inventory->items[inventory->item_count++] = item;

    return 1;
}

int delete_item_from_inventory(inventory_t* inventory, item_t* item){
    int item_index = -1;
    for (int i = 0; i < inventory->item_count; i++){
        if (inventory->items[i] == item){
            item_index = i;
        }
    }

    if (item_index == -1){
        return 0;
    }

    for (int i = item_index; i < inventory->item_count - 1; i++){
        inventory->items[i] = inventory->items[i+1];
    }
    inventory->item_count--;
    return 1;
}

void destroy_inventory(inventory_t* inventory, int destroy_items){
    if (destroy_items) {
        for (int i = 0; i < inventory->item_count; i++) {
            free(inventory->items[i]);
        }
    }
    free(inventory->items);
    free(inventory);
}

int use_item(player_t* player, item_t* item, struct world_t* world){
    if (item->callback_index != CALLBACK_NONE){
        callbacks[item->callback_index](world);
        deapply_item_to_player(player, item);
        delete_item_from_inventory(player->inventory, item);
        return 1;
    }
    return 0;
}

void change_inventory_size(inventory_t* inventory, int new_size){
    inventory->size = new_size;
    inventory->items = realloc(inventory->items, sizeof(item_t*) * new_size);
    if (inventory->size > inventory->item_count){
        inventory->item_count = inventory->size;
    }
}

item_t* get_random_item(inventory_t* inventory){
    return inventory->items[rand() % inventory->item_count];
}