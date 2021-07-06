#pragma once

#define VEC2_ONE ((vector2_t) {.x = 1, .y = 1})
#define VEC2_ZERO ((vector2_t) {.x = 0, .y = 0})
#define VEC2_LEFT ((vector2_t) { .x = -1, .y = 0})
#define VEC2_RIGHT ((vector2_t) { .x = 1, .y = 0})
#define VEC2_UP ((vector2_t) { .x = 0, .y = -1})
#define VEC2_DOWN ((vector2_t) { .x = 0, .y = 1})

void fail_gracefully(const char* format_string, ...);

typedef struct vector2_t{
    int x, y;
}vector2_t;

typedef struct vectors_t{
    vector2_t* data;
    int size, capacity;
}vectors_t;

vector2_t sum(vector2_t a, vector2_t b);
vector2_t sub(vector2_t a, vector2_t b);
vector2_t scale(vector2_t a, int magnitude);
vector2_t scale_accordingly(vector2_t a, vector2_t b);
int equal(vector2_t a, vector2_t b);

vector2_t* get_shuffled_directions();

int is_valid_index(int index, int size);
int is_valid_rect_index(vector2_t index, vector2_t size);