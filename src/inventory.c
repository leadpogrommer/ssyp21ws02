#include "inventory.h"
#include <stdlib.h>

inventory_t* init_inventory(int size){
    inventory_t* inventory = calloc(sizeof(inventory_t), 1);
    inventory->size = size;
    inventory->items = calloc(sizeof(item_t*), inventory->size);
    inventory->item_count = 0;
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

void destroy_inventory(inventory_t* inventory){
    free(inventory->items);
    for (int i = 0; i < inventory->item_count; i++){
        free(inventory->items[i]);
    }
    free(inventory);
}

void use_item(inventory_t* inventory, item_t* item, struct world_t* world){
    if (item->use){
        item->use(world);
        delete_item_from_inventory(inventory, item);
    }
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