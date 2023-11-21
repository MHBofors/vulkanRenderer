//
//  dynamic_vector.c
//  DynamicArray
//
//  Created by Markus Höglin on 2023-09-28.
//

#include "dynamic_vector.h"

struct vector {
    void* array;
    size_t element_size;
    uint32_t element_count;
    uint32_t vector_size;
};

//typedef enum return_type{unsuccessful_execution = -1, successful_execution = 0} return_type;

int vector_alloc(dynamic_vector **vector, size_t size) {
    dynamic_vector *vector_new = malloc(sizeof(dynamic_vector));
    if(vector_new == NULL) {
        return -1;
    } else {
        vector_new->vector_size = VECTOR_INIT_SIZE;
        vector_new->element_count = 0;
        vector_new->element_size = size;
        vector_new->array = malloc(VECTOR_INIT_SIZE * size);
    
        if(vector_new->array != NULL) {
            *vector = vector_new;
            return 0;
        }
        free(vector_new);
        return -1;
    }
}

void vector_free(dynamic_vector *vector) {
    free(vector->array);
    free(vector);
}

uint32_t vector_count(dynamic_vector *vector) {
    return vector->element_count;
}

uint32_t vector_size(dynamic_vector *vector) {
    return vector->vector_size;
}

static inline int vector_resize(dynamic_vector *vector, uint32_t count) {
    void *pointer_new = realloc(vector->array, count * vector->element_size);
    if(pointer_new != NULL) {
        vector->array = pointer_new;
        vector->vector_size = count;
        return 0;
    }
    return -1;
}

int vector_add(dynamic_vector *vector, void *element_new) {
    if(vector->vector_size == vector->element_count) {
        int return_val = vector_resize(vector, 2 * vector->element_count);
        if(return_val != 0) {
            return return_val;
        }
    }
    
    vector->element_count++;
    vector_set(vector, element_new, vector->element_count - 1);
    return 0;
}

void vector_remove(dynamic_vector *vector, uint32_t index) {
    if(index < 0 || vector->element_count <= index) {
        return;
    }
    
    for(uint32_t i = index; i < vector->element_count; i++) {
        vector_set(vector, vector_get_element(vector, i + 1), i);
    }
    
    vector->element_count--;
}

void vector_set(dynamic_vector *vector, void *element, uint32_t index) {
    void *p_index = vector_get_element(vector, index);
    if(p_index == NULL) {
        return;
    }
    else {
        memcpy(p_index, element, vector->element_size);
    }
}

void *vector_get_element(dynamic_vector *vector, uint32_t index) {
    if(index < 0 || vector->element_count < index) {
        return NULL;
    }
    else {
        return (char*)vector->array + vector->element_size*index;
    }
}

void *vector_get_array(dynamic_vector *vector) {
    return vector->array;
}

int vector_add_array(dynamic_vector *vector, void *array, uint32_t array_count) {
    if(vector->vector_size < vector->element_count + array_count) {
        if(vector_reserve(vector, vector->element_count + array_count) == -1) {
            return -1;
        }
    }
    for(uint32_t i = 0; i < array_count; i++) {
        vector_add(vector, (char *)array + i*vector->element_size);
    }
    return 0;
}

int vector_reserve(dynamic_vector *vector, uint32_t count) {
    uint32_t new_count = 1;
    while(new_count <= count) {
        new_count <<= 1;
    }
    
    return vector_resize(vector, new_count);
}

