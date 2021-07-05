#pragma once

#define VEC2_LEFT ((vector2_t) { .x = -1, .y = 0})
#define VEC2_RIGHT ((vector2_t) { .x = 1, .y = 0})
#define VEC2_UP ((vector2_t) { .x = 0, .y = -1})
#define VEC2_DOWN ((vector2_t) { .x = 0, .y = 1})

void fail_gracefully(const char* format_string, ...);

typedef struct vector2_t{
    int x, y;
}vector2_t;

vector2_t sum(vector2_t a, vector2_t b);
vector2_t sub(vector2_t a, vector2_t b);
vector2_t multi(vector2_t a, int magnitude);