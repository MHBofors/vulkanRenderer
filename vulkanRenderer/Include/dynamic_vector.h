//
//  dynamic_vector.h
//  DynamicArray
//
//  Created by Markus Höglin on 2023-09-28.
//

#ifndef dynamic_vector_h
#define dynamic_vector_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define VECTOR_INIT_SIZE 8
#define WORD_SIZE 8

typedef struct vector dynamic_vector;

int vector_alloc(dynamic_vector **vector, size_t size);
void vector_free(dynamic_vector *vector);

uint32_t vector_count(dynamic_vector *vector);
uint32_t vector_size(dynamic_vector *vector);

static inline int vector_resize(dynamic_vector *vector, uint32_t count);
int vector_reserve(dynamic_vector *vector, uint32_t count);

int vector_add(dynamic_vector *vector, void *element_new);
int vector_add_array(dynamic_vector *vector, void *array, uint32_t count);
void vector_remove(dynamic_vector *vector, uint32_t index);

void vector_set(dynamic_vector *vector, void *element, uint32_t index);
void *vector_get_element(dynamic_vector *vector, uint32_t index);

void *vector_get_array(dynamic_vector *vector);



#endif /* dynamic_vector_h */
