#pragma once
// Vertex structure
typedef struct _Vertex_ {
    double x, y, z;
} Vertex;

// Edge structure
typedef struct _Edge_ {
    Vertex v1, v2;
} Edge;

// Triangle structure
typedef struct _Triangle_ {
    Edge e1, e2, e3;
} Triangle;

// 3D Object structure
typedef struct _Object3D_ {
    Triangle* triangles;
} Object3D;

int render();
