#include <unistd.h>
#include <stdlib.h>
#include <level.h>
#include <enemies.h>
#include <bullet.h>

bullets_t* bullets_init() {
    bullets_t* bullets = malloc(sizeof(bullets_t));
    bullets->array = malloc(sizeof(bullet_t));
    bullets->count = 0;
    bullets->capacity = 1;
    return bullets;
}

void bullets_add(bullets_t* bullets, vector2_t pos, vector2_t vel) {
    if(bullets->count + 1 > bullets->capacity) {
        bullets->array = realloc(bullets->array, sizeof(bullet_t) * bullets->capacity * 2);
        bullets->capacity *= 2;
    }
    bullet_t bullet = { .pos = pos, .vel = vel };
    bullets->array[bullets->count] = bullet;
    bullets->count++;
}

void bullets_remove(bullets_t* bullets, int pos) {
    for(int i = pos; i < bullets->count - 1; i++) {
        bullets->array[i] = bullets->array[i + 1];
    }
    bullets->count--;
}

void bullets_clear(bullets_t* bullets) {
    bullets->count = 0;
}

void bullets_destroy(bullets_t* bullets) {
    free(bullets->array);
    free(bullets);
}

void fire(bullets_t* bullets, player_t* player, vector2_t vel) {
    bullets_add(bullets, player->pos, vel);
}

void process_bullets(bullets_t* bullets, enemies_t* enemies, level_t* level, unsigned long long time) {
    for(int i = 0; i < bullets->count; i++) {
        if(time % BULLET_SPEED == 0) bullets->array[i].pos = sum(bullets->array[i].pos, bullets->array[i].vel);
        char tile = level->data[bullets->array[i].pos.y][bullets->array[i].pos.x];
        if(tile == '*' || tile == 0) {
            bullets_remove(bullets, i);
            if(i > 0) i--;
        } else for(int j = 0; j < enemies->count; j++) {
            if(equal(bullets->array[i].pos, enemies->array[j].pos)) {
                enemies_remove(enemies, j);
                if(j > 0) j--;
                bullets_remove(bullets, i);
                if(i > 0) i--;
                break;
            }
        }
    }
}