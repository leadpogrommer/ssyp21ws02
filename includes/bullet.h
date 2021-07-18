#pragma once

#include "utility.h"
#include "statistics.h"
#include "enemies.h"
#include "level.h"

#define BULLET_SPEED 10

typedef struct {
    vector2_t pos;
    vector2_t vel;
    int damage;
    char is_enemy;
} bullet_t;

typedef struct bullets_t {
    bullet_t* array;
    int count, capacity;
} bullets_t;

bullets_t* bullets_init();
void bullets_add(bullets_t* bullets, vector2_t pos, vector2_t vel, int damage, char is_enemy);
void bullets_remove(bullets_t* bullets, int pos);
void bullets_destroy(bullets_t* bullets);
void bullets_clear(bullets_t* bullets);

void fire(bullets_t* bullets, vector2_t position, vector2_t vel, int damage, char is_enemy);
void process_bullets(bullets_t* bullets, enemies_t* enemies, level_t* level, player_t* player, statistics_t* stats, unsigned long long time);