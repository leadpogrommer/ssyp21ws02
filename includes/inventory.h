#pragma once

struct world_t;

typedef struct item_t{
    const char* name;
    int id;
    int damage_buff, hp_buff;
    int callback_index;
}item_t;

typedef struct inventory_t{
    item_t** items;
    int item_count, size;
}inventory_t;

void use_item(inventory_t* inventory, item_t* item, struct world_t* world);

inventory_t* init_inventory(int size);
void destroy_inventory(inventory_t* inventory);

item_t* get_random_item(inventory_t* inventory);

int add_item_to_inventory(inventory_t* inventory, item_t* item);
int delete_item_from_inventory(inventory_t* inventory, item_t* item);