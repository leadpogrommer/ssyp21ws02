#pragma once

#include "utility.h"
#include "level.h"
#include "pathfinder.h"
#include "player.h"

#define ENEMY_DAMAGE_MIN 1
#define ENEMY_DAMAGE_MAX 2
#define ENEMY_SPEED_MIN 30
#define ENEMY_SPEED_MAX 60

typedef struct {
    int speed, damage;
    vector2_t pos;
} enemy_t;

typedef struct {
    enemy_t* array;
    int count, capacity;
} enemies_t;

enemies_t* enemies_init();
void enemies_add(enemies_t* enemies, enemy_t enemy);
void enemies_remove(enemies_t* enemies, int pos);
void enemies_destroy(enemies_t* enemies);

void spawn_enemies(level_t* level, enemies_t* enemies);
void process_enemies(pathfinder_t* pathfinder, enemies_t* enemies, player_t* player, unsigned long long time);