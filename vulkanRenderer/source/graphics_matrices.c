//
//  graphics_matrices.c
//  vulkanRenderer
//
//  Created by Markus Höglin on 2023-10-27.
//

#include "graphics_matrices.h"

float inner_product(uint32_t n, float u[n], float v[n]) {
    float sum = 0;

    for(uint32_t i = 0; i < n; i++) {
        sum += u[i]*v[i];
    }
    return sum;
}

float vector_norm(uint32_t n, float u[n]) {
    return sqrt(inner_product(n, u, u));
}

void scalar_product(uint32_t n, float a, float u[n], float v[n]) {
    for(uint32_t i = 0; i < n; i++) {
        v[i] = a*u[i];
    }
}

void vector_normalise(uint32_t n, float u[n], float v[n]) {
    float norm = vector_norm(n, u);
    if(norm < TOL) {
        return;
    }
    scalar_product(n, 1/norm, u, v);
}
/*
void vector_add(uint32_t n, float u[n], float v[n], float w[n]) {
    for(uint32_t i = 0; i < n; i++) {
        w[i] = u[i]+v[i];
    }
}
*/
void vector_subtract(uint32_t n, float u[n], float v[n], float w[n]) {
    for(uint32_t i = 0; i < n; i++) {
        w[i] = u[i]-v[i];
    }
}

quaternion_t cross_product(quaternion_t p, quaternion_t q) {
    return (quaternion_t){
        .r = 0,
        .i = p.j*q.k - p.k*q.j,
        .j = p.k*q.i - p.i*q.k,
        .k = p.i*q.j - p.j*q.i
    };
}

quaternion_t quaternion_conjugate(quaternion_t q) {
    return (quaternion_t){.r = q.r, .i = -q.i, .j = -q.j, .k = -q.k};
}

quaternion_t quaternion_add(quaternion_t p, quaternion_t q) {
    return (quaternion_t){.r = p.r + q.r, .i = p.i + q.i, .j = p.j + q.j, .k = p.k + q.k};
}

float quaternion_norm(quaternion_t q) {
    return sqrt(q.r*q.r + q.i*q.i + q.j*q.j + q.k*q.k);
}

quaternion_t quaternion_scalar_product(float a, quaternion_t q) {
    return (quaternion_t){.r = a*q.r, .i = a*q.i, .j = a*q.j, .k = a*q.k};
}

quaternion_t quaternion_normalise(quaternion_t q) {
    float norm = quaternion_norm(q);
    if(norm < TOL) {
        return q;
    }

    return quaternion_scalar_product(1/norm, q);
}

quaternion_t quaternion_product(quaternion_t p, quaternion_t q) {
    return (quaternion_t){
        .r = p.r*q.r - p.i*q.i - p.j*q.j - p.k*q.k,
        .i = p.r*q.i + p.i*q.r + p.j*q.k - p.k*q.j,
        .j = p.r*q.j - p.i*q.k + p.j*q.r + p.k*q.i,
        .k = p.r*q.k + p.i*q.j - p.j*q.i + p.k*q.r
    };
}

quaternion_t quaternion_rotation(float angle, quaternion_t q) {
    float cosine = cos(angle/2);
    float sine = sin(angle/2);
    
    float inverse_norm = 1/sqrt(q.i*q.i + q.j*q.j + q.k*q.k);

    return (quaternion_t){
        .r = cosine,
        .i = sine*q.i*inverse_norm,
        .j = sine*q.j*inverse_norm,
        .k = sine*q.k*inverse_norm
    };
}

quaternion_t vector_division(quaternion_t p, quaternion_t q) {
    quaternion_t axis = {};
}