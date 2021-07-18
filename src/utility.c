#include "utility.h"
#include "inventory.h"
#include "level.h"
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

vector2_t scaledown(vector2_t a, int magnitude){
    return (vector2_t) { .x = a.x / magnitude, .y = a.y / magnitude };
}

int cross(vector2_t a, vector2_t b){
    return a.x * b.y - a.y * b.x;
}

char equal(vector2_t a, vector2_t b){
    if (a.x == b.x && a.y == b.y) return 1;
    return 0;
}

vector2_t scale_accordingly(vector2_t a, vector2_t b){
    return (vector2_t) {.x = a.x * b.x, .y = a.y * b.y};
}

void get_shuffled_directions(vector2_t directions[4]){
    get_straight_directions(directions);

    for (int i = 0; i < 3; i++){
        int j = rand() % (4 - i) + i; // Pick random

        vector2_t tmp = directions[i]; // Swap two elements
        directions[i] = directions[j];
        directions[j] = tmp;
    }
}

void get_straight_directions(vector2_t directions[4]){
    directions[0] = VEC2_LEFT;
    directions[1] = VEC2_UP;
    directions[2] = VEC2_DOWN;
    directions[3] = VEC2_RIGHT;
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

void make_action_along_the_line(vector2_t start, vector2_t end, int max_depth, level_t *level, void *argument2,
                                char (*action)(vector2_t, level_t *, void *)){
    vector2_t diff = sub(end, start);
    vector2_t abs_diff = { .y = diff.y > 0 ? diff.y : -diff.y,  .x = diff.x > 0 ? diff.x : -diff.x};
    int y_step = diff.y > 0 ? 1 : diff.y == 0 ? 0 : -1;
    int x_step = diff.x > 0 ? 1 : diff.x == 0 ? 0 : -1;

    vector2_t current = start;
    int* bigger = abs_diff.y > abs_diff.x ? &(current.y) : &(current.x);
    int* smaller = abs_diff.y > abs_diff.x ? &(current.x) : &(current.y);;
    int bigger_step = abs_diff.y > abs_diff.x ? y_step : x_step;
    int smaller_step = abs_diff.y > abs_diff.x ? x_step : y_step;
    abs_diff = (vector2_t) { .x = MAX(abs_diff.y, abs_diff.x), .y = MIN(abs_diff.y, abs_diff.x) };

    int error = 0;
    char go_on = 1;
    for (int i = 0; i < max_depth && go_on; i++){
        *bigger += bigger_step;
        error += (abs_diff.y);
        if (error >= abs_diff.x){
            error -= abs_diff.x;
            *smaller += smaller_step;
        }

        go_on = action(current, level, argument2);
    }
}


#define TYPE int
#define NAME int
#include "generic_array.h"

#define TYPE vector2_t
#define NAME vector2
#include "generic_array.h"

#define TYPE vector2_pair_t
#define NAME vector2_pair
#include "generic_array.h"

#define TYPE item_t
#define NAME item
#include "generic_array.h"
