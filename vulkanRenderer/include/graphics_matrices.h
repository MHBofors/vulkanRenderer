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

#endif /* graphics_matrices_h */
