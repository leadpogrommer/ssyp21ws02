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

vector2_t multi(vector2_t a, int magnitude){
    return (vector2_t) { .x = a.x * magnitude, .y = a.y * magnitude };
}
