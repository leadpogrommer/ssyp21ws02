#include <stdlib.h>

#define GLUE(x, y) x ## y
#define GLUE2(x, y) GLUE(x, y)
#define ARRAY_TYPE GLUE2(NAME, _array_t)

#ifdef ONLYDEFINE

typedef struct {
    TYPE * data;
    int size, capacity;
}ARRAY_TYPE;

ARRAY_TYPE* GLUE2(init_, GLUE2(NAME, _array))();
void GLUE2(destroy_, GLUE2(NAME, _array))(ARRAY_TYPE* array);
void GLUE2(push_back_, NAME)(ARRAY_TYPE* array, TYPE element);
void GLUE2(delete_, GLUE2(NAME, _from_array))(ARRAY_TYPE* array, int index);

#undef ONLYDEFINE
#else

ARRAY_TYPE* GLUE2(init_, GLUE2(NAME, _array))(){
    ARRAY_TYPE* array = malloc(sizeof(ARRAY_TYPE));
    array->size = 0;
    array->capacity = 1;
    array->data = malloc(sizeof(TYPE) * array->capacity);
    return array;
}

void GLUE2(destroy_, GLUE2(NAME, _array))(ARRAY_TYPE* array){
    free(array->data);
    free(array);
}

void GLUE2(push_back_, NAME)(ARRAY_TYPE* array, TYPE element){
    if (array->size == array->capacity){
        array->capacity *= 2;
        array->data = realloc(array->data, sizeof(TYPE) * array->capacity);
    }

    array->data[array->size++] = element;
}

void GLUE2(delete_, GLUE2(NAME, _from_array))(ARRAY_TYPE* array, int index){
    for (int i = index; i < array->size - 1; i++){
        array->data[i] = array->data[i+1];
    }
    array->size--;
}

#endif

#undef TYPE
#undef NAME