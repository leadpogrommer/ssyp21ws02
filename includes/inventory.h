#pragma once

#define CALLBACK_NONE (-1)
#define CALLBACK_HEAL_SMALL (0)
#define CALLBACK_ANNIHILATION (1)

typedef struct world_t world_t;
typedef struct player_t player_t;

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

int use_item(player_t* player, item_t* item, struct world_t* world);

inventory_t* init_inventory(int size);
void destroy_inventory(inventory_t* inventory, int destroy_items);

item_t* get_random_item(inventory_t* inventory);

int add_item_to_inventory(inventory_t* inventory, item_t* item);
int delete_item_from_inventory(inventory_t* inventory, item_t* item);