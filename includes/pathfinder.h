#pragma once

#include "heap.h"
#include "level.h"

typedef struct pathfinder_t{
    level_t* level;
    heap_t* heap;
    short** closed;
    heap_element_t*** heap_elements;
}pathfinder_t;

pathfinder_t* init_pathfinder(level_t* level);
void destroy_pathfinder(pathfinder_t* pathfinder);

void clear_previous_pathfinder_data(pathfinder_t* pathfinder);

vector2_array_t* find_path(pathfinder_t* pathfinder, vector2_t start, vector2_t end, int use_diagonals);
vector2_array_t* retrace_path(pathfinder_t* pathfinder, vector2_t start, vector2_t end);

int vector2_distance(vector2_t a, vector2_t b);