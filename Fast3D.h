#pragma once
#include <stdint.h>

// Vertex structure
typedef struct _Vertex_ {
    double x, y, z;
} Vertex;

// Triangle structure
typedef struct _Triangle_ {
    Vertex *v1, *v2, *v3;
} Triangle;

// Square structure
typedef struct _Square_ {
    Vertex *v1, *v2, *v3, *v4;
} Square;

// Model structure
typedef struct _Model_ {
    double x, y, z; // Coordinates of the model in the World Coordinates System
    Vertex *vertices;   // All vertecies of a model
    Triangle *triangles; // All triangles of a model
} Model;

int render(uint32_t* framebuffer);
