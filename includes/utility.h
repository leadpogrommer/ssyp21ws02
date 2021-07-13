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

typedef struct vector2_array_t{
    vector2_t* data;
    int size, capacity;
}vector2_array_t;

vector2_array_t* init_vector2_array();
void push_back_vector2(vector2_array_t* array, vector2_t element);
void destroy_vector2_array(vector2_array_t* array);

typedef struct vector2_pair_t{
    vector2_t first, second;
}vector2_pair_t;

typedef struct vector2_pair_array_t{
    vector2_pair_t* data;
    int size, capacity;
}vector2_pair_array_t;

vector2_pair_array_t* init_vector2_pair_array();
void push_back_vector2_pair(vector2_pair_array_t* array, vector2_pair_t element);
void destroy_vector2_pair_array(vector2_pair_array_t* array);
void delete_last_vector2_pair(vector2_pair_array_t* array);

typedef struct int_array_t{
    int* data;
    int size, capacity;
}int_array_t;

int_array_t* init_int_array();
void push_back_int(int_array_t* array, int element);
void delete_element(int_array_t* array, int index);
void destroy_int_array(int_array_t* array);
