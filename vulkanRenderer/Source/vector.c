//
//  p_vector.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-08.
//

#include "vector.h"

typedef struct TYPED(vector) {
    uint32_t count;
    uint32_t size;
    TYPE *array;
} TYPED(vector);

int TYPED(vector_alloc)(TYPED(vector) **vector) {
    TYPED(vector) *new_vector = malloc(sizeof(TYPED(vector)));
    TYPE *array = malloc(sizeof(TYPE) * INIT_SIZE);
    if(new_vector != NULL && array != NULL) {
        new_vector->count = 0;
        new_vector->size = INIT_SIZE;
        new_vector->array = array;
        *vector = new_vector;
        return 0;
    }else {
        free(new_vector);
        free(array);
        return -1;
    }
}

void TYPED(vector_free)(TYPED(vector) *p_vector) {
    free(p_vector->array);
    free(p_vector);
}


uint32_t TYPED(vector_count)(TYPED(vector) *p_vector) {
    return p_vector->count;
}
uint32_t TYPED(vector_size)(TYPED(vector) *p_vector) {
    return p_vector->size;
}


static inline int vector_resize(TYPED(vector) *vector, uint32_t size) {
    TYPE *array = realloc(vector->array, sizeof(TYPE) * size);
    if(array != NULL) {
        vector->array = array;
        return 0;
    }else {
        free(array);
        return -1;
    }
}

int TYPED(vector_reserve)(TYPED(vector) *p_vector, uint32_t count) {
    if(p_vector->count < count) {
        uint32_t new_count = 1;
        while(new_count < count) {
            new_count <<= 1;
        }
        
        return vector_resize(p_vector, new_count);
    }else {
        return 0;
    }
}

int TYPED(vector_add)(TYPED(vector) *p_vector, TYPE *p_element) {
    if(p_vector->size == p_vector->count) {
        if(vector_resize(p_vector, 2 * p_vector->size) != 0) {
            return -1;
        }
    }
    
    p_vector->count++;
    TYPED(vector_set)(p_vector, p_element, p_vector->count - 1);
    return 0;
}
int TYPED(vector_add_array)(TYPED(vector) *p_vector, uint32_t index);
void TYPED(vector_remove)(TYPED(vector) *p_vector, uint32_t index) {
    if(index < 0 || p_vector->count <= index) {
        return;
    }
    
    for(uint32_t i = index; i < p_vector->count; i++) {
        TYPED(vector_set)(p_vector, TYPED(vector_get_element)(p_vector, i + 1), i);
    }
    
    p_vector->count--;
}


int TYPED(vector_set)(TYPED(vector) *p_vector, TYPE *p_element, uint32_t index) {
    TYPE *p_index = TYPED(vector_get_element)(p_vector, index);
    if(p_index == NULL) {
        return -1;
    }else {
        memcpy(p_index, p_element, sizeof(TYPE));
        return 0;
    }
}

TYPE *TYPED(vector_get_element)(TYPED(vector) *p_vector, uint32_t index) {
    if(index < 0 || p_vector->count < index) {
        return NULL;
    }
    else {
        return p_vector->array + index;
    }
}
TYPE *TYPED(vector_get_array)(TYPED(vector) *p_vector) {
    return p_vector->array;
}
