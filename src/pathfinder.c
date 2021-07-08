#include "pathfinder.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

pathfinder_t* init_pathfinder(level_t* level){
    pathfinder_t* pathfinder = malloc(sizeof(pathfinder_t));
    pathfinder->level = level;
    pathfinder->heap = init_heap(level->size.y * level->size.x);

    pathfinder->closed = malloc(sizeof(short*) * level->size.y);
    for (int i = 0; i < level->size.y; i++){
        pathfinder->closed[i] = calloc(sizeof(short), level->size.x);
    }

    pathfinder->heap_elements = malloc(sizeof(heap_element_t**) * level->size.y);
    for (int i = 0; i < level->size.y; i++){
        pathfinder->heap_elements[i] = calloc(sizeof(heap_element_t*), level->size.x);
        for (int j = 0; j < level->size.x; j++){
            pathfinder->heap_elements[i][j] = calloc(sizeof(heap_element_t), 1);
        }
    }

    return pathfinder;
}

// BEWARE - LEVEL MUST STILL EXIST WHEN YOU DESTROYING PATHFINDER
// So make sure you destroy pathfinder first
void destroy_pathfinder(pathfinder_t* pathfinder){
    for (int i = 0; i < pathfinder->level->size.y; i++){
        free(pathfinder->closed[i]);
        free(pathfinder->heap_elements[i]);
    }
    free(pathfinder->closed);
    free(pathfinder->heap_elements);
    free(pathfinder);
}

void clear_previous_pathfinder_data(pathfinder_t* pathfinder){
    clear_heap(pathfinder->heap);
    for (int i = 0; i < pathfinder->level->size.y; i++){
        memset(pathfinder->closed[i], 0, sizeof(short) * pathfinder->level->size.x);
        for (int j = 0; j < pathfinder->level->size.x; j++){
            pathfinder->heap_elements[i][j]->position = (vector2_t){ .x = j, .y = i };
            pathfinder->heap_elements[i][j]->parent = NULL;
            pathfinder->heap_elements[i][j]->g_cost = INT_MAX;
            pathfinder->heap_elements[i][j]->h_cost = INT_MAX;
            pathfinder->heap_elements[i][j]->heap_index = 0;
        }
    }
}

vector2_array_t* find_path(pathfinder_t* pathfinder, vector2_t start, vector2_t end, int use_diagonals){
    clear_previous_pathfinder_data(pathfinder);

    pathfinder->heap_elements[start.y][start.x]->g_cost = 0;
    add_element_to_heap(pathfinder->heap, pathfinder->heap_elements[start.y][start.x]);
    while (!is_heap_empty(pathfinder->heap)){
        heap_element_t* current = get_element_from_heap(pathfinder->heap);

        if (equal(current->position, end)){
            return retrace_path(pathfinder, start, end);
        }

        int count = 8;
        if (!use_diagonals){
            count = 4;
        }

        vector2_t directions[count];
        if (use_diagonals){
            get_full_eight_directions(directions);
        }else{
            get_shuffled_directions(directions);
        }

        for (int i = 0; i < count; i++){
            vector2_t neighbour_cell = sum(current->position, directions[i]);
            if (!is_valid_rect_index(neighbour_cell, pathfinder->level->size) ||
                pathfinder->closed[neighbour_cell.y][neighbour_cell.x] ||
                pathfinder->level->data[neighbour_cell.y][neighbour_cell.x] == '*'||
                    pathfinder->level->data[neighbour_cell.y][neighbour_cell.x] == 0){
                continue;
            }

            heap_element_t* neighbour = pathfinder->heap_elements[neighbour_cell.y][neighbour_cell.x];

            int new_g_cost = current->g_cost + vector2_distance(current->position, neighbour_cell);
            if (new_g_cost < neighbour->g_cost || !is_heap_contains(pathfinder->heap, neighbour)){
                neighbour->g_cost = new_g_cost;
                neighbour->h_cost = vector2_distance(end, neighbour_cell);
                neighbour->parent = current;

                if (!is_heap_contains(pathfinder->heap, neighbour)){
                    add_element_to_heap(pathfinder->heap, neighbour);
                }
            }
        }
        pathfinder->closed[current->position.y][current->position.x] = 1;
    }

    return NULL;
}

vector2_array_t* retrace_path(pathfinder_t* pathfinder, vector2_t start, vector2_t end){
    vector2_array_t* path = init_vector2_array();
    heap_element_t* current = pathfinder->heap_elements[end.y][end.x];
    while (!equal(current->position, start)){
        push_back_vector2(path, current->position);
        current = current->parent;
    }
    return path;
}

int vector2_distance(vector2_t a, vector2_t b){
    int dx = b.x > a.x ? b.x - a.x : a.x - b.x;
    int dy = b.y > a.y ? b.y - a.y : a.y - b.y;

    return dx > dy ? dy * 14 + (dx - dy) * 10 : dx * 14 + (dy - dx) * 10;
}