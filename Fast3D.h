#pragma once
// Includes
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

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
	char* name;
	char* path;
	Vertex* vertices;
	Triangle* triangles;
	Vertex pos;
	unsigned long long tCount;
} Mesh;

typedef struct _Renderer_ {
	// System variables
	char* id;
	short idLength;
	time_t then;
	double ElapsedTime;
	Vertex camera;
	Vertex lightDirection;

	// Mesh
	unsigned long long mCount;
	Mesh* mesh;
	Mesh* rotatedXMesh;
	Mesh* rotatedZMesh;
	Mesh* translatedWorldMesh;
	//Mesh* translatedCameraMesh;
	Mesh* projectedMesh;

	// Frame buffer
	uint32_t* fb;                   // Frame buffer
	int fbWidth;                    // Frame buffer width
	int fbHeight;                   // Frame buffer height
	unsigned long long fbTotalSize; // Total frame buffer size
	unsigned long long fbIndex;     // Index of the frame buffer

	// Normal generation
	Vertex normal;
	Vertex line1;
	Vertex line2;

	// Projection Matrix
	double ProjectionMatrix[4][4];
	double pmFOV;
	double pmNear;
	double pmFar;
	double pmAspectRatio;
	double pmFOVRad;

	// Rotation Matrix
	double RotationMatrixX[4][4];
	double RotationMatrixZ[4][4];
	double rmTheta;

	// Rasterization
	int rSX;
	int rBX;
	int rSY;
	int rBY;
	int rCurrentPixelX;
	int rCurrentPixelY;
} Renderer;

// Function declerations
Renderer* Fast3D__init(int fb_width, int fb_height, double FOV);
void Fast3D__destroy(Renderer* renderer);
int Fast3D__render(Renderer* renderer);
void Fast3D__addMesh(Renderer* renderer, Mesh* mesh);
