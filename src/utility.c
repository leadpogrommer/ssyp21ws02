#include "utility.h"
#include <ncurses.h>
#include <stdlib.h>

void fail_gracefully(const char* format_string, ...){
    endwin();
    va_list args;
    va_start(args, format_string);
    vprintf(format_string, args);
    va_end(args);
    getchar();
    exit(1);
}

vector2_t sum(vector2_t a, vector2_t b){
    return (vector2_t) { .x = a.x + b.x, .y = a.y + b.y };
}

/**
 *
 * @param a Vector from which subtracting
 * @param b Vector which is subtracted
 * @return
 */
vector2_t sub(vector2_t a, vector2_t b){
    return (vector2_t) { .x = a.x - b.x, .y = a.y - b.y };
}

vector2_t scale(vector2_t a, int magnitude){
    return (vector2_t) { .x = a.x * magnitude, .y = a.y * magnitude };
}

int cross(vector2_t a, vector2_t b){
    return a.x * b.y - a.y * b.x;
}

int equal(vector2_t a, vector2_t b){
    if (a.x == b.x && a.y == b.y) return 1;
    return 0;
}

vector2_t scale_accordingly(vector2_t a, vector2_t b){
    return (vector2_t) {.x = a.x * b.x, .y = a.y * b.y};
}

vector2_array_t* init_vector2_array(){
    vector2_array_t* array = malloc(sizeof(vector2_array_t));
    array->size = 0;
    array->capacity = 1;
    array->data = malloc(sizeof(vector2_t) * array->capacity);
    return array;
}

void push_back_vector2(vector2_array_t* array, vector2_t vector){
    if (array->size == array->capacity){
        array->capacity *= 2;
        array->data = realloc(array->data, sizeof(vector2_t) * array->capacity);
    }

    array->data[array->size++] = vector;
}

void destroy_vector2_array(vector2_array_t* array){
    free(array->data);
    free(array);
}

vector2_t* get_shuffled_directions(){
    vector2_t* result = malloc(sizeof(vector2_t) * 4);
    result[0] = VEC2_LEFT;
    result[1] = VEC2_UP;
    result[2] = VEC2_DOWN;
    result[3] = VEC2_RIGHT;

    for (int i = 0; i < 3; i++){
        int j = rand() % (4 - i) + i; // Pick random

        vector2_t tmp = result[i]; // Swap two elements
        result[i] = result[j];
        result[j] = tmp;
    }

    return result;
}

void get_full_eight_directions(vector2_t directions[8]){
    directions[0] = VEC2_LEFT;
    directions[1] = VEC2_RIGHT;
    directions[2] = VEC2_DOWN;
    directions[3] = VEC2_UP;
    directions[4] = sum(VEC2_LEFT, VEC2_UP);
    directions[5] = sum(VEC2_RIGHT, VEC2_UP);
    directions[6] = sum(VEC2_RIGHT, VEC2_DOWN);
    directions[7] = sum(VEC2_LEFT, VEC2_DOWN);
}

int is_valid_index(int index, int size){
    return index >= 0 && index < size;
}

int is_valid_rect_index(vector2_t index, vector2_t size){
    return is_valid_index(index.x, size.x) && is_valid_index(index.y, size.y);
}