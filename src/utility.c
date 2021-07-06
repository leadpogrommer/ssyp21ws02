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

int equal(vector2_t a, vector2_t b){
    if (a.x == b.x && a.y == b.y) return 1;
    return 0;
}

vector2_t scale_accordingly(vector2_t a, vector2_t b){
    return (vector2_t) {.x = a.x * b.x, .y = a.y * b.y};
}

vectors_t* init_vectors(){
    vectors_t* vectors = malloc(sizeof(vectors));
    vectors->data = malloc(sizeof(vector2_t));
    int size = 0;
    int capacity = 1;
    return vectors;
}

void push_back_vector2(vectors_t* vectors, vector2_t vector){
    if (vectors->size == vectors->capacity){
        vectors->capacity *= 2;
        vectors->data = realloc(vectors->data, sizeof(vector2_t) * vectors->capacity);
    }

    vectors->data[vectors->size++] = vector;
}

void destroy_vectors(vectors_t* vectors){
    free(vectors->data);
    free(vectors);
}