#pragma once

#include "utility.h"

typedef struct heap_element_t{
    vector2_t position;
    struct heap_element_t* parent;
    int g_cost;
    int h_cost;
    int heap_index;
}heap_element_t;

typedef struct heap_t{
    int max_size, current_size;
    heap_element_t** data;
}heap_t;

heap_t* init_heap(int max_size);
void destroy_heap(heap_t* heap);

void clear_heap(heap_t* heap);

int is_heap_empty(heap_t* heap);

void add_element_to_heap(heap_t* heap, heap_element_t* element);
heap_element_t* get_element_from_heap(heap_t* heap);
int is_heap_contains(heap_t* heap, heap_element_t* heap_element);

void bubble_up(heap_t* heap, int element_index);
void bubble_down(heap_t* heap, int element_index);
void swap_elements_in_heap(heap_t* heap, int a, int b);

int compare_heap_elements(heap_element_t* a, heap_element_t* b);