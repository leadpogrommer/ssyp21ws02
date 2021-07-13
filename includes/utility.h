#pragma once

#define VEC2_ONE ((vector2_t) {.x = 1, .y = 1})
#define VEC2_ZERO ((vector2_t) {.x = 0, .y = 0})
#define VEC2_LEFT ((vector2_t) { .x = -1, .y = 0})
#define VEC2_RIGHT ((vector2_t) { .x = 1, .y = 0})
#define VEC2_UP ((vector2_t) { .x = 0, .y = -1})
#define VEC2_DOWN ((vector2_t) { .x = 0, .y = 1})
#define VEC2_SQUARE(a) (scale(VEC2_ONE, (a)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

void fail_gracefully(const char* format_string, ...);

typedef struct vector2_t{
    int x, y;
}vector2_t;

vector2_t sum(vector2_t a, vector2_t b);
vector2_t sub(vector2_t a, vector2_t b);
vector2_t scale(vector2_t a, int magnitude);
vector2_t scaledown(vector2_t a, int magnitude);
vector2_t scale_accordingly(vector2_t a, vector2_t b);
int cross(vector2_t a, vector2_t b);
int equal(vector2_t a, vector2_t b);

void get_straight_directions(vector2_t directions[4]);
void get_shuffled_directions(vector2_t directions[4]);
void get_full_eight_directions(vector2_t directions[8]);

int is_valid_index(int index, int size);
int is_valid_rect_index(vector2_t index, vector2_t size);

typedef struct vector2_pair_t{
    vector2_t first, second;
}vector2_pair_t;

#define ONLYDEFINE
#define TYPE int
#define NAME int
#include "generic_array.h"

#define ONLYDEFINE
#define TYPE vector2_t
#define NAME vector2
#include "generic_array.h"

#define ONLYDEFINE
#define TYPE vector2_pair_t
#define NAME vector2_pair
#include "generic_array.h"