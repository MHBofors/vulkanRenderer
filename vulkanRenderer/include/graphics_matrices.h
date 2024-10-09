//
//  graphics_matrices.h
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#ifndef graphics_matrices_h
#define graphics_matrices_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TOL 1E-6

typedef struct quaternion_t {
    float r;
    float i;
    float j;
    float k;
} quaternion_t;

typedef struct matrix_t {
    float array[4][4];
} matrix_t;

typedef struct transformation_t {
    float matrix[4][4];
} transformation_t;

#endif /* graphics_matrices_h */
