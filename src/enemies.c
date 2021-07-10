#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "enemies.h"
#include "world.h"

enemies_t* enemies_init() {
    enemies_t* enemies = malloc(sizeof(enemies_t));
    enemies->array = malloc(sizeof(enemy_t));
    enemies->count = 0;
    enemies->capacity = 1;
    return enemies;
}

void enemies_add(enemies_t* enemies, enemy_t enemy) {
    if(enemies->count + 1 > enemies->capacity) {
        enemies->array = realloc(enemies->array, sizeof(enemy_t) * enemies->capacity * 2);
        enemies->capacity *= 2;
    }
    enemies->array[enemies->count] = enemy;
    enemies->count++;
}

void enemies_remove(enemies_t* enemies, int pos) {
    for(int i = pos; i < enemies->count - 1; i++) {
        enemies->array[i] = enemies->array[i + 1];
    }
    enemies->count--;
}

void enemies_clear(enemies_t* enemies) {
    enemies->count = 0;
}



void spawn_enemies(level_t* level, enemies_t* enemies) {
    enemies_clear(enemies);
    for(int i = 0; i < level->room_grid_size.x; i++) {
        for(int j = 0; j < level->room_grid_size.y; j++) {
            if(level->room_grid[j][i] == NULL || (level->start_room_grid_position.x == i && level->start_room_grid_position.y == j)) continue;
            int ex = ((double)rand() / RAND_MAX) * (level->room_grid[j][i]->size.x - 2) + 1;
            int ey = ((double)rand() / RAND_MAX) * (level->room_grid[j][i]->size.y - 2) + 1;
            int espeed = ((double)rand() / RAND_MAX) * (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN + 1) + ENEMY_SPEED_MIN;
            int edamage = ((double)rand() / RAND_MAX) * (ENEMY_DAMAGE_MAX - ENEMY_DAMAGE_MIN + 1) + ENEMY_DAMAGE_MIN;
            if(level->room_grid[j][i]->data[ey][ex] == '.') {
                vector2_t outer = { .x = i, .y = j };
                vector2_t inner = { .x = ex, .y = ey };
                enemy_t enemy = {
                    .pos = get_level_position(level, outer, inner),
                    .speed = espeed,
                    .damage = edamage
                };
                enemies_add(enemies, enemy);
            }
        }
    }
}

void process_enemies(pathfinder_t* pathfinder, enemies_t* enemies, player_t* player, unsigned long long time) {
    for(int i = 0; i < enemies->count; i++) {
        if(time % enemies->array[i].speed == 0) {
            vector2_array_t* path = find_path(pathfinder, enemies->array[i].pos, player->pos, 1);
            enemies->array[i].pos = path->data[path->size - 1];
        }
        if(equal(enemies->array[i].pos, player->pos)) {
            player->health -= enemies->array[i].damage;
            enemies_remove(enemies, i);
            if(i > 0) i--;
        }
    }
}

void draw_enemies(WINDOW* window, palette_t* palette, enemies_t* enemies, vector2_t offset){
    for(int i = 0; i < enemies->count; i++)
        mvwaddch(window, enemies->array[i].pos.y + offset.y, enemies->array[i].pos.x + offset.x, '&');
}