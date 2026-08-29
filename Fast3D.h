#pragma once
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

// Point
typedef struct _Point_ {
	double x, y;
} Point;

// Vertex structure
typedef struct _Vertex_ {
	double x, y, z;
} Vertex;

// Triangle structure
typedef struct _Triangle_ {
	Vertex v[3];
} Triangle;

// Mesh structure
typedef struct _Mesh_ {
	Vertex* vertices;
	Triangle* triangles;
	Vertex pos;
} Mesh;

// Global variables
unsigned long long fb_total_size;
time_t then;
double ElapsedTime;
double **ZBuffer;

// Function declerations
int render(uint32_t* framebuffer, int fb_width, int fb_height, double pFOV);
