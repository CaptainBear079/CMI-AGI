// Includes
#include "Fast3D.h"

Vertex vertices[8];
Triangle triangles[12];

// OBJ loader function
Mesh* openOBJ(const char* path) {
	// Open .obj file
	FILE* file = fopen(path, "r");
	if(file == NULL) {
		printf("[Fast3D]: Could not open file \"%s\".\n", path);
		return NULL;
	}

	// Read and interpret file
	// - Count information
	char c;
	int v;      // Vertices
	int vt;     // Texture coordinates
	int vn;     // Vertices normal
	int f;      // Faces
	int g;      // Groups
	int s;      // Smoothings
	int o;      // Object names
	int mtllib; // Material libraries
	int usemtl; // Material assignments
	while((c = fgetc(file)) != EOF) {
		switch((char)c) {
			// v, vt, vn
			case (char)'v': {
				c = fgetc(file);
				if(c == ' ') {
					v++;
				}
				else if(c == 't') {
					vt++;
				}
				else if(c == 'n') {
					vn++;
				}
				else {
					printf("[Fast3D][openOBJ]: Unsupported OBJ feature in file \"%s\".\n", path);
					return NULL;
				}
			} break;
			// f
			case (char)'f': {
				f++;
			} break;
			// g
			case (char)'g': {
				g++;
			} break;
			// s
			case (char)'s': {
				s++;
			} break;
			// o
			case (char)'o': {
				o++;
			} break;
			// mtllib
			case (char)'m': {
				char ch[6];
				for(int i = 0; i < 5; i++) {
					c = fgetc(file);
					if(c == EOF) {
						printf("[Fast3D][openOBJ]: Unexpected EOF (End Of File) in \"%s\".\n", path);
						return NULL;
					}
					else {
						ch[i] = c;
					}
				}
				ch[5] = '\0';
				if(strcmp(ch, "tllib") == 0) {
					mtllib++;
				}
				else {
					printf("[Fast3D][openOBJ]: Unsupported OBJ feature in file \"%s\".\n", path);
					return NULL;
				}
			} break;
			// usemtl
			case (char)'u': {
				char ch[6];
				for(int i = 0; i < 5; i++) {
					c = fgetc(file);
					if(c == EOF) {
						printf("[Fast3D][openOBJ]: Unexpected EOF (End Of File) in \"%s\".\n", path);
						return NULL;
					}
					else {
						ch[i] = c;
					}
				}
				ch[5] = '\0';
				if(strcmp(ch, "semtl") == 0) {
					usemtl++;
				}
				else {
					printf("[Fast3D][openOBJ]: Unsupported OBJ feature in file \"%s\".\n", path);
					return NULL;
				}
			} break;
			default: break;
		}
	}

	rewind(file);

	// - Create buffers
	Mesh* mesh = malloc(sizeof(Mesh));
	mesh->vertices = malloc(v * sizeof(Vertex));
	mesh->triangles = malloc(f * sizeof(Triangle));

	fclose(file);
	return mesh;
}

// Matrix multiplication
void matrixMultiplication(Vertex* i, Vertex* o, double matrix[4][4]) {
	o->x = i->x * matrix[0][0] + i->y * matrix[1][0] + i->z * matrix[2][0] + matrix[3][0];
	o->y = i->x * matrix[0][1] + i->y * matrix[1][1] + i->z * matrix[2][1] + matrix[3][1];
	o->z = i->x * matrix[0][2] + i->y * matrix[1][2] + i->z * matrix[2][2] + matrix[3][2];
	double w = i->x * matrix[0][3] + i->y * matrix[1][3] + i->z * matrix[2][3] + matrix[3][3];
	if(w != 0.0) {
		o->x /= w;
		o->y /= w;
		o->z /= w;
	}
	return;
}

double PointInTriangle(Vertex* a, Vertex* b, Vertex* c, Vertex* p) {
	double e1 = (b->x - a->x) * (p->y - a->y) - (b->y - a->y) * (p->x - a->x);
	double e2 = (c->x - b->x) * (p->y - b->y) - (c->y - b->y) * (p->x - b->x);
	double e3 = (a->x - c->x) * (p->y - c->y) - (a->y - c->y) * (p->x - c->x);

	return (e1 >= 0 && e2 >= 0 && e3 >= 0) ||
           (e1 <= 0 && e2 <= 0 && e3 <= 0);
}

Renderer* Fast3D__init(int fb_width, int fb_height, double FOV) {
	// Create renderer object
	Renderer* renderer = malloc(sizeof(Renderer));

	// Set frame buffer
	renderer->fbWidth = fb_width;
	renderer->fbHeight = fb_height;
	renderer->pmAspectRatio = fb_height / fb_width;
	renderer->fbTotalSize = fb_width * fb_height;
	renderer->fb = malloc(renderer->fbTotalSize * sizeof(uint32_t));

	// Set Projection Matrix
	renderer->pmNear = 0.02;
	renderer->pmFar = 4800.0;
	renderer->pmFOV = FOV;
	renderer->pmFOVRad = 1.0/ tan(FOV * 0.5 / 180.0 * 3.141592653589793);
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			renderer->ProjectionMatrix[i][j] = 0.0;
		}
	}
	renderer->ProjectionMatrix[0][0] = renderer->pmAspectRatio * renderer->pmFOVRad;
	renderer->ProjectionMatrix[1][1] = renderer->pmFOVRad;
	renderer->ProjectionMatrix[2][2] = renderer->pmFar / (renderer->pmFar - renderer->pmNear);
	renderer->ProjectionMatrix[3][2] = (-(renderer->pmFar) * renderer->pmNear) / (renderer->pmFar - renderer->pmNear);
	renderer->ProjectionMatrix[2][3] = 1.0;
	renderer->ProjectionMatrix[3][3] = 0.0;

	// Set then
	renderer->then = time(NULL);

	// Return the renderer
	return renderer;
}

// Render function
int Fast3D__render(Renderer* renderer) {
	// Measure Elapse time
	time_t now = time(NULL);
	renderer->ElapsedTime = difftime(now, renderer->then);

	//Clear framebuffer
	for(unsigned int i = 0; i < renderer->fbTotalSize; i++) {
		renderer->fb[i] = (uint32_t)0;
	}

	// Prepare Rotation Matrix
	renderer-> rmTheta = 0.1 * renderer->ElapsedTime;
	renderer->RotationMatrixX[0][0] = cos(renderer->rmTheta);
	renderer->RotationMatrixX[0][1] = sin(renderer->rmTheta);
	renderer->RotationMatrixX[1][0] = -sin(renderer->rmTheta);
	renderer->RotationMatrixX[1][1] = cos(renderer->rmTheta);
	renderer->RotationMatrixX[2][2] = 1.0;
	renderer->RotationMatrixX[3][3] = 1.0;
	renderer->RotationMatrixZ[0][0] = 1.0;
	renderer->RotationMatrixZ[1][1] = cos(renderer->rmTheta * 0.5);
	renderer->RotationMatrixZ[1][2] = sin(renderer->rmTheta * 0.5);
	renderer->RotationMatrixZ[2][1] = -sin(renderer->rmTheta * 0.5);
	renderer->RotationMatrixZ[2][2] = cos(renderer->rmTheta * 0.5);
	renderer->RotationMatrixZ[3][3] = 1.0;

	// Render Mesh
	for(unsigned int m = 0; m < renderer->mCount; m++) {
		// Apply Rotation (Rotation Matrix)
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			matrixMultiplication(
				&(renderer->mesh[m].triangles[t].v[0]),
				&(renderer->rotatedXMesh[m].triangles[t].v[0]),
				renderer->RotationMatrixX
			);
			matrixMultiplication(
				&(renderer->mesh[m].triangles[t].v[1]),
				&(renderer->rotatedXMesh[m].triangles[t].v[1]),
				renderer->RotationMatrixX
			);
			matrixMultiplication(
				&(renderer->mesh[m].triangles[t].v[2]),
				&(renderer->rotatedXMesh[m].triangles[t].v[2]),
				renderer->RotationMatrixX
			);
			
			matrixMultiplication(
				&(renderer->rotatedXMesh[m].triangles[t].v[0]),
				&(renderer->rotatedZMesh[m].triangles[t].v[0]),
				renderer->RotationMatrixZ
			);
			matrixMultiplication(
				&(renderer->rotatedXMesh[m].triangles[t].v[1]),
				&(renderer->rotatedZMesh[m].triangles[t].v[1]),
				renderer->RotationMatrixZ
			);
			matrixMultiplication(
				&(renderer->rotatedXMesh[m].triangles[t].v[2]),
				&(renderer->rotatedZMesh[m].triangles[t].v[2]),
				renderer->RotationMatrixZ
			);
		}

		// Translate
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			for(int v = 0; v < 3; v++) {
				// Translate to world space
				renderer->translatedWorldMesh[m].triangles[t].v[v].x = renderer->rotatedZMesh[m].triangles[t].v[v].x + renderer->mesh[m].pos.x;
				renderer->translatedWorldMesh[m].triangles[t].v[v].y = renderer->rotatedZMesh[m].triangles[t].v[v].y + renderer->mesh[m].pos.y;
				renderer->translatedWorldMesh[m].triangles[t].v[v].z = renderer->rotatedZMesh[m].triangles[t].v[v].z + renderer->mesh[m].pos.z;

				// Translate to camera space
				renderer->translatedCameraMesh[m].triangles[t].v[v].x = renderer->translatedWorldMesh[m].triangles[t].v[v].x + renderer->camera.pos.x;
				renderer->translatedCameraMesh[m].triangles[t].v[v].y = renderer->translatedWorldMesh[m].triangles[t].v[v].y + renderer->camera.pos.y;
				renderer->translatedCameraMesh[m].triangles[t].v[v].z = renderer->translatedWorldMesh[m].triangles[t].v[v].z + renderer->camera.pos.z;
			}
		}

		// Apply Projection Matrix
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			matrixMultiplication(
				&(renderer->translatedCameraMesh[m].triangles[t].v[0]),
				&(renderer->projectedMesh[m].triangles[t].v[0]),
				renderer->ProjectionMatrix);
			matrixMultiplication(
				&(renderer->translatedCameraMesh[m].triangles[t].v[1]),
				&(renderer->projectedMesh[m].triangles[t].v[1]),
				renderer->ProjectionMatrix);
			matrixMultiplication(
				&(renderer->translatedCameraMesh[m].triangles[t].v[2]),
				&(renderer->projectedMesh[m].triangles[t].v[2]),
				renderer->ProjectionMatrix);
		}

		// Scale to screen
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			renderer->projectedMesh[m].triangles[t].v[0].x += 1.0;
			renderer->projectedMesh[m].triangles[t].v[1].x += 1.0;
			renderer->projectedMesh[m].triangles[t].v[2].x += 1.0;
			renderer->projectedMesh[m].triangles[t].v[0].y += 1.0;
			renderer->projectedMesh[m].triangles[t].v[1].y += 1.0;
			renderer->projectedMesh[m].triangles[t].v[2].y += 1.0;

			renderer->projectedMesh[m].triangles[t].v[0].x *= 0.5 * renderer->fbWidth;
			renderer->projectedMesh[m].triangles[t].v[1].x *= 0.5 * renderer->fbWidth;
			renderer->projectedMesh[m].triangles[t].v[2].x *= 0.5 * renderer->fbWidth;
			renderer->projectedMesh[m].triangles[t].v[0].y *= 0.5 * renderer->fbHeight;
			renderer->projectedMesh[m].triangles[t].v[1].y *= 0.5 * renderer->fbHeight;
			renderer->projectedMesh[m].triangles[t].v[2].y *= 0.5 * renderer->fbHeight;
		}

		// Rasterization
		// - Find the smallest and biggest x and y
		renderer->rSX = malloc(renderer->mesh[m].tCount * sizeof(int));
		renderer->rBX = malloc(renderer->mesh[m].tCount * sizeof(int));
		renderer->rSY = malloc(renderer->mesh[m].tCount * sizeof(int));
		renderer->rBY = malloc(renderer->mesh[m].tCount * sizeof(int));
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			renderer->rSX[t] = renderer->projectedMesh[m].triangles[t].v[0].x;
			if(renderer->projectedMesh[m].triangles[t].v[1].x < renderer->rSX[t]) {
				renderer->rSX[t] = renderer->projectedMesh[m].triangles[t].v[1].x;
			}
			if(renderer->projectedMesh[m].triangles[t].v[2].x < renderer->rSX[t]) {
				renderer->rSX[t] = renderer->projectedMesh[m].triangles[t].v[2].x;
			}

			renderer->rBX[t] = renderer->projectedMesh[m].triangles[t].v[0].x;
			if(renderer->projectedMesh[m].triangles[t].v[1].x > renderer->rBX[t]) {
				renderer->rBX[t] = renderer->projectedMesh[m].triangles[t].v[1].x;
			}
			if(renderer->projectedMesh[m].triangles[t].v[2].x > renderer->rBX[t]) {
				renderer->rBX[t] = renderer->projectedMesh[m].triangles[t].v[2].x;
			}

			renderer->rSY[t] = renderer->projectedMesh[m].triangles[t].v[0].y;
			if(renderer->projectedMesh[m].triangles[t].v[1].y < renderer->rSY[t]) {
				renderer->rSY[t] = renderer->projectedMesh[m].triangles[t].v[1].y;
			}
			if(renderer->projectedMesh[m].triangles[t].v[2].y < renderer->rSY[t]) {
				renderer->rSY[t] = renderer->projectedMesh[m].triangles[t].v[2].y;
			}

			renderer->rBY[t] = renderer->projectedMesh[m].triangles[t].v[0].y;
			if(renderer->projectedMesh[m].triangles[t].v[1].y > renderer->rBY[t]) {
				renderer->rBY[t] = renderer->projectedMesh[m].triangles[t].v[1].y;
			}
			if(renderer->projectedMesh[m].triangles[t].v[2].y > renderer->rBY[t]) {
				renderer->rBY[t] = renderer->projectedMesh[m].triangles[t].v[2].y;
			}
		}

		// - Check if pixel center is in the triangle
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			renderer->rCurrentPixelX = renderer->rSX[t];
			renderer->rCurrentPixelY = renderer->rSY[t];
			while(true) {
				if(renderer->rCurrentPixelX > renderer->rBX[t]) {
					renderer->rCurrentPixelX = renderer->rSX[t];
					renderer->rCurrentPixelY++;
				}
				if(renderer->rCurrentPixelY > renderer->rBY[t]) {
					break;
				}
				if(
					PointInTriangle(
						&(renderer->projectedMesh[m].triangles[t].v[0]),
						&(renderer->projectedMesh[m].triangles[t].v[1]),
						&(renderer->projectedMesh[m].triangles[t].v[2]),
						&((Vertex){ renderer->rCurrentPixelX + 0.5, renderer->rCurrentPixelY + 0.5, 0.0 })
					)
				) {
					renderer->fb[((renderer->fbHeight - renderer->rCurrentPixelY) * renderer->fbWidth) - (renderer->fbWidth - renderer->rCurrentPixelX)] = 0xFFFFFFFF;
				}
				renderer->rCurrentPixelX++;
			}
		}
		free(renderer->rSX);
		free(renderer->rBX);
		free(renderer->rSY);
		free(renderer->rBY);
	}

	// Record current time
	renderer->then = time(NULL);
	// Return without errors
	return 0;
}
