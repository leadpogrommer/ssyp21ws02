#include "saver.h"
#include "string.h"
#include <bson/bson.h>

void save_world(world_t* world){
    FILE* file = fopen("save", "w");

    //setvbuf(file, NULL, _IOFBF, 0);

    save_player(world->player, file);
    save_level(world->level, file);
    save_enemies(world->enemies, file);

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

    fwrite(&(level->shrines_array->size), sizeof(int), 1, file);
    fwrite(level->shrines_array->data, sizeof(vector2_t), level->shrines_array->size, file);
    for (int i = 0; i < level->item_array->size; i++){
        fwrite(&(level->item_array->data[i].id), sizeof(int), 1, file);
    }
}

void save_statistics(statistics_t* statistics){
    FILE* file = fopen("statistics.bson", "w");

    bson_t *b;
    uint8_t *buf = NULL;
    size_t buflen = 0;
    bson_writer_t *writer = bson_writer_new(&buf, &buflen, 0, bson_realloc_ctx, NULL);

    bson_writer_begin(writer, &b);
    BSON_APPEND_INT32(b, "enemies_killed", statistics->enemies_killed);
    BSON_APPEND_INT32(b, "max_gold", statistics->max_gold);
    BSON_APPEND_INT32(b, "deaths", statistics->deaths);
    BSON_APPEND_INT32(b, "max_level", statistics->max_level);
    BSON_APPEND_INT32(b, "items_picked_up", statistics->items_picked_up);
    BSON_APPEND_INT32(b, "shrines_used", statistics->shrines_used);
    bson_writer_end(writer);


    fwrite(buf, buflen * sizeof(uint8_t), 1, file);
    bson_free (buf);
    bson_writer_destroy(writer);

    fclose(file);
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

void save_enemies(enemies_t* enemies, FILE* file){
    fwrite(&(enemies->count), sizeof(int), 1, file);
    fwrite(enemies->array, sizeof(enemy_t), enemies->count, file);
}

world_t* load_world(){
    FILE* file = fopen("save", "r");
    if (!file){
        fail_gracefully("Looks like there is no save file");
    }

    world_t* world = init_world();

    world->player = load_player(file, world->items);
    world->level = load_level(file, world->room_pool, world->items);
    world->enemies = load_enemies(file);

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

level_t* load_level(FILE* file, room_pool_t* room_pool, inventory_t* parent_inventory){
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

    int count;
    fread(&count, sizeof(int), 1, file);
    level->item_array = init_item_array();
    level->shrines_array = init_vector2_array();

    for (int i = 0; i < count; i++){
        vector2_t tmp;
        fread(&tmp, sizeof(vector2_t), 1, file);
        push_back_vector2(level->shrines_array, tmp);
    }

    for (int i = 0; i < count; i++){
        int id;
        fread(&id, sizeof(int), 1, file);
        if (is_valid_index(id, parent_inventory->item_count)){
            push_back_item(level->item_array, *parent_inventory->items[id]);
        }
    }

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


statistics_t* load_statistics() {
    statistics_t * stats = init_statistics();

    bson_reader_t *reader;
    bson_error_t error;
    const bson_t *doc;
    bool eof;

    reader = bson_reader_new_from_file("statistics.bson", &error);

    if (reader) {
        doc = bson_reader_read(reader, &eof);

        bson_iter_t iter;
        bson_iter_init(&iter, doc);
        while (bson_iter_next(&iter)){ // TODO - We need to do a hash table search for these keys
            if (!strcmp(bson_iter_key(&iter), "enemies_killed")){
                stats->enemies_killed = bson_iter_value(&iter)->value.v_int32;
            }else if(!strcmp(bson_iter_key(&iter), "deaths")){
                stats->deaths = bson_iter_value(&iter)->value.v_int32;
            }else if(!strcmp(bson_iter_key(&iter), "items_picked_up")){
                stats->items_picked_up = bson_iter_value(&iter)->value.v_int32;
            }else if(!strcmp(bson_iter_key(&iter), "max_gold")){
                stats->max_gold = bson_iter_value(&iter)->value.v_int32;
            }else if(!strcmp(bson_iter_key(&iter), "shrines_used")){
                stats->shrines_used = bson_iter_value(&iter)->value.v_int32;
            }else if(!strcmp(bson_iter_key(&iter), "max_level")) {
                stats->max_level = bson_iter_value(&iter)->value.v_int32;
            }
        }
        bson_reader_destroy(reader);
    }

    return stats;
}

enemies_t* load_enemies(FILE* file){
    enemies_t* enemies = enemies_init();
    int count;
    fread(&count, sizeof(int), 1, file);
    enemies_resize(enemies, count);
    fread(enemies->array, sizeof(enemy_t), count, file);

    return enemies;
}
