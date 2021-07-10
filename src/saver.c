#include "saver.h"
#include "string.h"

void save_world(world_t* world){
    FILE* file = fopen("save", "w");

    setvbuf(file, NULL, _IOFBF, 0);

    save_player(world->player, file);
    save_level(world->level, file);

    fwrite(&(world->current_level), sizeof(int), 1, file);
    fwrite(&(world->time), sizeof(unsigned long long), 1, file);

    fclose(file);
}

void save_player(player_t* player, FILE* file){
    fwrite(player, sizeof(player_t), 1, file);
    save_inventory(player->inventory, file);
}

void save_inventory(inventory_t* inventory, FILE* file){
    fwrite(&(inventory->size), sizeof(int), 1, file);
    fwrite(&(inventory->item_count), sizeof(int), 1, file);
    for (int i = 0; i < inventory->item_count; i++){
        fwrite(&(inventory->items[i]->id), sizeof(int), 1, file);
    }
}

void save_level(level_t* level, FILE* file){
    fwrite(&(level->room_count), sizeof(int), 1, file);
    fwrite(&(level->room_grid_size), sizeof(vector2_t), 1, file);
    for (int i = 0; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            save_room(level->room_grid[i][j], file);
        }
    }

    for (int i = 0; i < level->size.y; i++){
        fwrite(level->data[i], sizeof(char), level->size.x, file);
    }

    fwrite(&(level->start_room_grid_position), sizeof(vector2_t), 1, file);

}

void save_room(room_t* room, FILE* file){
    if (room){
        char null = 0;
        fwrite(&null, sizeof(char), 1, file);
        fwrite(room->filename, sizeof(char), 256, file);
    }else{
        char null = 1;
        fwrite(&null, sizeof(char), 1, file);
    }
}

world_t* load_world(){
    FILE* file = fopen("save", "r");

    world_t* world = init_world();

    world->player = load_player(file, world->items);
    world->level = load_level(file, world->room_pool);

    fread(&(world->current_level), sizeof(int), 1, file);
    fread(&(world->time), sizeof(unsigned long long), 1, file);

    world->pathfinder = init_pathfinder(world->level);

    fclose(file);

    return world;
}

player_t* load_player(FILE* file, inventory_t* parent_inventory){
    player_t* player = init_player(0);
    destroy_inventory(player->inventory, 0);

    fread(player, sizeof(player_t), 1, file);

    player->inventory = load_inventory(file, parent_inventory);

    return player;
}

inventory_t* load_inventory(FILE* file, inventory_t* parent_inventory){
    int size;
    fread(&size, sizeof(int), 1, file);
    inventory_t* inventory = init_inventory(size);
    int item_count;
    fread(&(item_count), sizeof(int), 1, file);
    for (int i = 0; i < item_count; i++){
        int id;
        fread(&id, sizeof(int), 1, file);
        if (is_valid_index(id, parent_inventory->item_count)){
            add_item_to_inventory(inventory, parent_inventory->items[id]);
        }
    }

    return inventory;
}

level_t* load_level(FILE* file, room_pool_t* room_pool){
    vector2_t room_grid_size;
    int room_count;
    fread(&room_count, sizeof(int), 1, file);
    fread(&room_grid_size, sizeof(vector2_t), 1, file);

    level_t* level = init_level_room_grid(room_count, room_grid_size);

    for (int i = 0; i < level->room_grid_size.y; i++){
        for (int j = 0; j < level->room_grid_size.x; j++){
            level->room_grid[i][j] = load_saved_room(file, room_pool);
        }
    }

    set_room_grid_cell_sizes(level);
    init_level_char_grid(level);

    for (int i = 0; i < level->size.y; i++){
        fread(level->data[i], sizeof(char), level->size.x, file);
    }

    fread(&(level->start_room_grid_position), sizeof(vector2_t), 1, file);

    return level;
}

room_t* load_saved_room(FILE* file, room_pool_t* room_pool){
    char null;
    fread(&null, sizeof(char), 1, file);
    if (!null){
        char filename[256];
        fread(filename, sizeof(char), 256, file);
        room_t* room = get_room_by_name(room_pool, filename);
        if (!room){
            fail_gracefully("Room with name: %s was not found", filename);
        }
        return room;
    }else{
        return NULL;
    }
}