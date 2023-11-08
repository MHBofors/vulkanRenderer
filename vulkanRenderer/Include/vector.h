//
//  vector.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-11-08.
//

#ifndef vector_h
#define vector_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef PREFIX
#undef TYPE
#undef TYPED

#define PREFIX str
#define TYPE const char *
#define TYPED(expression) str ## _ ## expression

#define INIT_SIZE 32

typedef enum vector_return_type vector_return_type;

typedef struct TYPED(vector) TYPED(vector);

int TYPED(vector_alloc)(TYPED(vector) **pp_vector);
void TYPED(vector_free)(TYPED(vector) *p_vector);

uint32_t TYPED(vector_count)(TYPED(vector) *p_vector);
uint32_t TYPED(vector_size)(TYPED(vector) *p_vector);

static inline int vector_resize(TYPED(vector) *vector, uint32_t size);
int TYPED(vector_reserve)(TYPED(vector) *vector, uint32_t size);

int TYPED(vector_add)(TYPED(vector) *p_vector, TYPE *p_element);
int TYPED(vector_add_array)(TYPED(vector) *p_vector, uint32_t index);
void TYPED(vector_remove)(TYPED(vector) *p_vector, uint32_t index);

int TYPED(vector_set)(TYPED(vector) *p_vector, TYPE *p_element, uint32_t index);
TYPE *TYPED(vector_get_element)(TYPED(vector) *p_vector, uint32_t index);
TYPE *TYPED(vector_get_array)(TYPED(vector) *p_vector);

#endif /* vector_h */
