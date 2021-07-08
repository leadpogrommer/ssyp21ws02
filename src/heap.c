#include "heap.h"
#include <stdlib.h>

heap_t* init_heap(int max_size){
    heap_t* heap = malloc(sizeof(heap_t));
    heap->max_size = max_size;
    heap->current_size = 0;
    heap->data = calloc(sizeof(heap_element_t*), heap->max_size);
    return heap;
}

void destroy_heap(heap_t* heap){
    free(heap->data);
    free(heap);
}

void clear_heap(heap_t* heap){
    heap->current_size = 0;
}

int is_heap_empty(heap_t* heap){
    return heap->current_size == 0;
}

void add_element_to_heap(heap_t* heap, heap_element_t* element){
    if (heap->current_size == heap->max_size){
        return; //TODO - resize heap/throw an error
    }

    heap->data[heap->current_size] = element;
    heap->data[heap->current_size]->heap_index = heap->current_size;
    heap->current_size++;
    bubble_up(heap, heap->current_size - 1);
}

void bubble_up(heap_t* heap, int element_index){
    if (element_index == 0){
        return;
    }
    int parent_index = (element_index - 1) / 2;
    if (compare_heap_elements(heap->data[element_index], heap->data[parent_index]) > 0){

        swap_elements_in_heap(heap, element_index, parent_index);
        bubble_up(heap, parent_index);
    }
}

heap_element_t* get_element_from_heap(heap_t* heap){
    heap_element_t* result = heap->data[0];
    heap->data[0] = heap->data[--heap->current_size];
    heap->data[0]->heap_index = 0;
    bubble_down(heap, 0);

    return result;
}

void bubble_down(heap_t* heap, int element_index){
    int left_child_index = element_index * 2 + 1;
    int right_child_index = element_index * 2 + 2;

    if (left_child_index >= heap->current_size){
        return;
    }

    int best = left_child_index;
    if (right_child_index < heap->current_size &&
        compare_heap_elements(heap->data[right_child_index], heap->data[best]) > 0){

        best = right_child_index;
    }

    if (compare_heap_elements(heap->data[best], heap->data[element_index]) > 0){
        swap_elements_in_heap(heap, best, element_index);
        bubble_down(heap, best);
    }
}

void swap_elements_in_heap(heap_t* heap, int a, int b){
    heap_element_t* tmp = heap->data[a];
    heap->data[a] = heap->data[b];
    heap->data[b] = tmp;
    heap->data[b]->heap_index = b;
    heap->data[a]->heap_index = a;
}

int is_heap_contains(heap_t* heap, heap_element_t* heap_element){
    return heap->data[heap_element->heap_index] && equal(heap->data[heap_element->heap_index]->position, heap_element->position);
}

int compare_heap_elements(heap_element_t* a, heap_element_t* b){
    if (a->g_cost + a->h_cost < b->g_cost + b->h_cost){
        return 1;
    }else if (a->g_cost + a->h_cost == b->g_cost + b->h_cost){
        return a->h_cost < b->h_cost ? 1 : -1;
    }else{
        return -1;
    }
}