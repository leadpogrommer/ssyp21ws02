#include <unistd.h>
#include <stdlib.h>
#include "enemies.h"
#include "bullet.h"
#include "world.h"

enemies_t* enemies_init() {
    enemies_t* enemies = malloc(sizeof(enemies_t));
    enemies->array = malloc(sizeof(enemy_t));
    enemies->count = 0;
    enemies->capacity = 1;
    return enemies;
}

void enemies_resize(enemies_t* enemies, int new_size){
    while (new_size > enemies->capacity){
        enemies->capacity *= 2;
    }
    enemies->array = realloc(enemies->array, sizeof(enemy_t) * enemies->capacity);
    enemies->count = new_size;
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

void enemies_destroy(enemies_t* enemies) {
    free(enemies->array);
    free(enemies);
}

void enemies_clear(enemies_t* enemies) {
    enemies->count = 0;
}

void spawn_enemies(level_t* level, enemies_t* enemies, int enemies_in_room) {
    enemies_clear(enemies);
    for(int i = 0; i < level->room_grid_size.x; i++) {
        for(int j = 0; j < level->room_grid_size.y; j++) {
            if(level->room_grid[j][i] == NULL || (level->start_room_grid_position.x == i && level->start_room_grid_position.y == j)) continue;
            int countdown = enemies_in_room;
            while (countdown--) {
                int ex = ((double) rand() / RAND_MAX) * (level->room_grid[j][i]->size.x - 2) + 1;
                int ey = ((double) rand() / RAND_MAX) * (level->room_grid[j][i]->size.y - 2) + 1;
                int espeed = ((double) rand() / RAND_MAX) * (ENEMY_SPEED_MAX - ENEMY_SPEED_MIN + 1) + ENEMY_SPEED_MIN;
                int edamage =
                        ((double) rand() / RAND_MAX) * (ENEMY_DAMAGE_MAX - ENEMY_DAMAGE_MIN + 1) + ENEMY_DAMAGE_MIN;
                int evision =
                        ((double) rand() / RAND_MAX) * (ENEMY_VISION_MAX - ENEMY_VISION_MIN + 1) + ENEMY_VISION_MIN;
                int ehp = rand() % (ENEMY_HP_MAX - ENEMY_DAMAGE_MIN) + ENEMY_HP_MIN;
                char etype = rand() % 2;
                if (level->room_grid[j][i]->data[ey][ex] == '.') {
                    vector2_t outer = {.x = i, .y = j};
                    vector2_t inner = {.x = ex, .y = ey};
                    enemy_t enemy = {
                            .pos = get_level_position(level, outer, inner),
                            .speed = espeed,
                            .damage = edamage,
                            .vision_radius = evision,
                            .hp = ehp,
                            .maxhp = ehp,
                            .type = etype
                    };
                    enemies_add(enemies, enemy);
                }
            }
        }
    }
}

char is_target(vector2_t current, level_t *level, void *argument2){
    if (!level) return 1;
    if (equal(current, ((vector2_pair_t*)argument2)->first)){
        ((vector2_pair_t*)argument2)->second = VEC2_ONE;
        return 0;
    }
    return 1;
}

char can_see(vector2_t start, vector2_t end, int vision_radius, level_t *level){

    vector2_pair_t data = { end, VEC2_ZERO };

    make_action_along_the_line(start, end, vision_radius, level, &data, is_target);

    return (char)!equal(data.second, VEC2_ZERO);
}

void process_enemies(world_t *world) {

    enemies_t *enemies = world->enemies;
    player_t *player = world->player;
    level_t *level = world->level;
    pathfinder_t *pathfinder = world->pathfinder;
    bullets_t *bullets = world->bullets;
    unsigned long time = world->time;


    for(int i = 0; i < enemies->count; i++) {
        enemy_t *enemy = &(enemies->array[i]);
        if(time % enemy->speed == 0) {
            if (can_see(enemy->pos, player->pos, enemy->vision_radius, level)){
                switch (enemy->type){
                    case ENEMY_TYPE_MELEE: {
                        vector2_array_t *path = find_path(pathfinder, enemy->pos, player->pos, 1);
                        enemy->pos = path->data[path->size - 1];
                        destroy_vector2_array(path);
                        break;
                    }
                    case ENEMY_TYPE_RANGER:{
                        // Let's first check if we can fire at player
                        vector2_t direction = VEC2_ZERO;
                        if (enemy->pos.y == player->pos.y){
                            direction = enemy->pos.x > player->pos.x ? VEC2_LEFT : VEC2_RIGHT;
                        }else if (enemy->pos.x == player->pos.x){
                            direction = enemy->pos.y > player->pos.y ? VEC2_UP : VEC2_DOWN;
                        }
                        // If not - let's move
                        if (equal(direction, VEC2_ZERO)){

                            vector2_array_t *path1 = find_path(pathfinder, enemy->pos, VEC2(enemy->pos.x, player->pos.y), 1);
                            vector2_array_t *path2 = find_path(pathfinder, enemy->pos, VEC2(player->pos.x, enemy->pos.y), 1);

                            if (path1 || path2) {
                                vector2_array_t *right_path = path1 ? (path2 ? (path1->size > path2->size ? path2 : path1) : path1) : path2;
                                enemy->pos = right_path->data[right_path->size - 1];
                                if (path1){
                                    destroy_vector2_array(path1);
                                }
                                if (path2){
                                    destroy_vector2_array(path2);
                                }
                            }
                        }else{
                            // If do - let's shoot!
                            fire(bullets, enemy->pos, direction, enemy->damage, 1);
                        }
                    }
                }
            }


        }

        if(equal(enemies->array[i].pos, player->pos)) {
            player->health -= enemies->array[i].damage;
            enemies_remove(enemies, i);
            if(i > 0) i--;
        }
    }
}
