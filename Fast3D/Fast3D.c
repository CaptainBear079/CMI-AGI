// Includes
#include "Fast3D.h"

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
inline __attribute__((always_inline))void Fast3D__matrixMultiplication(Vertex* i, Vertex* o, double matrix[4][4]) {
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

inline __attribute__((always_inline)) double Fast3D__PointInTriangle(Vertex* a, Vertex* b, Vertex* c, Vertex* p) {
	double e1 = (b->x - a->x) * (p->y - a->y) - (b->y - a->y) * (p->x - a->x);
	double e2 = (c->x - b->x) * (p->y - b->y) - (c->y - b->y) * (p->x - b->x);
	double e3 = (a->x - c->x) * (p->y - c->y) - (a->y - c->y) * (p->x - c->x);

	return (e1 >= 0 && e2 >= 0 && e3 >= 0) ||
           (e1 <= 0 && e2 <= 0 && e3 <= 0);
}

inline __attribute__((always_inline)) uint32_t Fast3D__toRGB(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}

inline __attribute__((always_inline)) unsigned long long Fast3D__getFBIndex(Renderer* renderer, int x, int y) {
	return ((renderer->fbHeight - y) * renderer->fbWidth) - (renderer->fbWidth - x);
}

inline __attribute__((always_inline)) void Fast3D__drawLine(Renderer* renderer, Vertex a, Vertex b) {
	int x0 = (int)lround(a.x);
	int y0 = (int)lround(a.y);
	int x1 = (int)lround(b.x);
	int y1 = (int)lround(b.y);
	int dx = abs(x1 - x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;
	int error = dx + dy;

	while(true) {
		if(x0 >= 0 && x0 < renderer->fbWidth && y0 >= 0 && y0 < renderer->fbHeight) {
			renderer->fbIndex = Fast3D__getFBIndex(renderer, x0, y0);
			renderer->fb[renderer->fbIndex] = Fast3D__toRGB(255, 255, 255);
		}

		if(x0 == x1 && y0 == y1) {
			break;
		}

		int doubledError = 2 * error;
		if(doubledError >= dy) {
			error += dy;
			x0 += sx;
		}
		if(doubledError <= dx) {
			error += dx;
			y0 += sy;
		}
	}
}

inline __attribute__((always_inline)) void Fast3D__rasterize(Renderer* renderer, int m, int t, double dp) {
	// Rasterization
	// - Find the smallest and biggest x and y
	renderer->rSX = 0;
	renderer->rBX = 0;
	renderer->rSY = 0;
	renderer->rBY = 0;
	renderer->rSX = renderer->projectedMesh[m].triangles[t].v[0].x;
	if(renderer->projectedMesh[m].triangles[t].v[1].x < renderer->rSX) {
		renderer->rSX = renderer->projectedMesh[m].triangles[t].v[1].x;
	}
	if(renderer->projectedMesh[m].triangles[t].v[2].x < renderer->rSX) {
		renderer->rSX = renderer->projectedMesh[m].triangles[t].v[2].x;
	}

	renderer->rBX = renderer->projectedMesh[m].triangles[t].v[0].x;
	if(renderer->projectedMesh[m].triangles[t].v[1].x > renderer->rBX) {
		renderer->rBX = renderer->projectedMesh[m].triangles[t].v[1].x;
	}
	if(renderer->projectedMesh[m].triangles[t].v[2].x > renderer->rBX) {
		renderer->rBX = renderer->projectedMesh[m].triangles[t].v[2].x;
	}

	renderer->rSY = renderer->projectedMesh[m].triangles[t].v[0].y;
	if(renderer->projectedMesh[m].triangles[t].v[1].y < renderer->rSY) {
		renderer->rSY = renderer->projectedMesh[m].triangles[t].v[1].y;
	}
	if(renderer->projectedMesh[m].triangles[t].v[2].y < renderer->rSY) {
		renderer->rSY = renderer->projectedMesh[m].triangles[t].v[2].y;
	}

	renderer->rBY = renderer->projectedMesh[m].triangles[t].v[0].y;
	if(renderer->projectedMesh[m].triangles[t].v[1].y > renderer->rBY) {
		renderer->rBY = renderer->projectedMesh[m].triangles[t].v[1].y;
	}
	if(renderer->projectedMesh[m].triangles[t].v[2].y > renderer->rBY) {
		renderer->rBY = renderer->projectedMesh[m].triangles[t].v[2].y;
	}

	// - Check if pixel center is in the triangle
	renderer->rCurrentPixelX = renderer->rSX;
	renderer->rCurrentPixelY = renderer->rSY;
	while(true) {
		if(renderer->rCurrentPixelX > renderer->rBX) {
			renderer->rCurrentPixelX = renderer->rSX;
			renderer->rCurrentPixelY++;
		}
		if(renderer->rCurrentPixelY > renderer->rBY) {
			break;
		}
		if(
			Fast3D__PointInTriangle(
				&(renderer->projectedMesh[m].triangles[t].v[0]),
				&(renderer->projectedMesh[m].triangles[t].v[1]),
				&(renderer->projectedMesh[m].triangles[t].v[2]),
				&((Vertex){ renderer->rCurrentPixelX + 0.5, renderer->rCurrentPixelY + 0.5, 0.0 })
			)
		) {
			if(renderer->rCurrentPixelX < 0 || renderer->rCurrentPixelX >= renderer->fbWidth || renderer->rCurrentPixelY < 0 || renderer->rCurrentPixelY >= renderer->fbHeight) {
				renderer->rCurrentPixelX++;
				continue;
			}
			renderer->fbIndex = ((renderer->fbHeight - renderer->rCurrentPixelY) * renderer->fbWidth) - (renderer->fbWidth - renderer->rCurrentPixelX);
			renderer->fb[renderer->fbIndex] = Fast3D__toRGB((255 * dp), (255 * dp), (255 * dp));
			}
		renderer->rCurrentPixelX++;
	}
}

inline __attribute__((always_inline)) void Fast3D__drawTriangle(Renderer* renderer, int m, int t, bool fill, double dp) {
	if(fill) {
		Fast3D__rasterize(renderer, m, t, dp);
	}
	else {
		// Draw triangle edges
		Fast3D__drawLine(renderer, renderer->projectedMesh[m].triangles[t].v[0], renderer->projectedMesh[m].triangles[t].v[1]);
		Fast3D__drawLine(renderer, renderer->projectedMesh[m].triangles[t].v[1], renderer->projectedMesh[m].triangles[t].v[2]);
		Fast3D__drawLine(renderer, renderer->projectedMesh[m].triangles[t].v[2], renderer->projectedMesh[m].triangles[t].v[0]);
	}
}

Renderer* Fast3D__init(int fb_width, int fb_height, double FOV) {
	// Create renderer object
	Renderer* renderer = malloc(sizeof(Renderer));

	// Set frame buffer
	renderer->fbWidth = fb_width;
	renderer->fbHeight = fb_height;
	renderer->fbTotalSize = fb_width * fb_height;
	renderer->fb = malloc(renderer->fbTotalSize * sizeof(uint32_t));

	// Set Projection Matrix
	renderer->pmNear = 0.02;
	renderer->pmFar = 4800.0;
	renderer->pmFOV = FOV;
	renderer->pmAspectRatio = (double)fb_height / (double)fb_width;
	renderer->pmFOVRad = 1.0 / tan(FOV * 0.5 / 180.0 * 3.141592653589793);
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

	// Set Mesh
	renderer->mCount = 0;
	renderer->mesh = NULL;
	renderer->rotatedXMesh = NULL;
	renderer->rotatedZMesh = NULL;
	renderer->translatedWorldMesh = NULL;
	//renderer->translatedCameraMesh = NULL;
	renderer->projectedMesh = NULL;

	// Set Rotation Matrix
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			renderer->RotationMatrixX[i][j] = 0.0;
			renderer->RotationMatrixZ[i][j] = 0.0;
		}
	}
	renderer->rmTheta = 0.0;

	// Set Rasterization
	renderer->rSX = 0;
	renderer->rBX = 0;
	renderer->rSY = 0;
	renderer->rBY = 0;
	renderer->rCurrentPixelX = 0;
	renderer->rCurrentPixelY = 0;

	// Set Camera
	renderer->camera.x = 0.0;
	renderer->camera.y = 0.0;
	renderer->camera.z = 0.0;

	// Set light
	renderer->lightDirection = (Vertex){ 0.0, 0.0, -1.0 };

	// Set then
	renderer->then = clock();

	// Return the renderer
	return renderer;
}

void Fast3D__destroy(Renderer* renderer) {
	free(renderer->fb);
	free(renderer->mesh);
	free(renderer->rotatedXMesh);
	free(renderer->rotatedZMesh);
	free(renderer->translatedWorldMesh);
	//free(renderer->translatedCameraMesh);
	free(renderer->projectedMesh);
	free(renderer);
	return;
}

// Render function
int Fast3D__render(Renderer* renderer) {
	// Measure Elapse time
	clock_t now = clock();
	renderer->ElapsedTime = (double)(now - renderer->then) / (double)CLOCKS_PER_SEC;
	renderer->then = now;

	double temp = 0.0;
	double temp2 = 0.0;

	//Clear framebuffer
	for(unsigned int i = 0; i < renderer->fbTotalSize; i++) {
		renderer->fb[i] = (uint32_t)0;
	}

	// Prepare Rotation Matrix
	renderer-> rmTheta += 0.8 * renderer->ElapsedTime;
	renderer->RotationMatrixZ[0][0] = cos(renderer->rmTheta);
	renderer->RotationMatrixZ[0][1] = sin(renderer->rmTheta);
	renderer->RotationMatrixZ[1][0] = -sin(renderer->rmTheta);
	renderer->RotationMatrixZ[1][1] = cos(renderer->rmTheta);
	renderer->RotationMatrixZ[2][2] = 1.0;
	renderer->RotationMatrixZ[3][3] = 1.0;

	renderer->RotationMatrixX[0][0] = 1.0;
	renderer->RotationMatrixX[1][1] = cos(renderer->rmTheta * 0.5);
	renderer->RotationMatrixX[1][2] = sin(renderer->rmTheta * 0.5);
	renderer->RotationMatrixX[2][1] = -sin(renderer->rmTheta * 0.5);
	renderer->RotationMatrixX[2][2] = cos(renderer->rmTheta * 0.5);
	renderer->RotationMatrixX[3][3] = 1.0;

	// Render Mesh
	for(unsigned int m = 0; m < renderer->mCount; m++) {
		// Apply Rotation (Rotation Matrix)
		for(unsigned int t = 0; t < renderer->mesh[m].tCount; t++) {
			Fast3D__matrixMultiplication(
				&(renderer->mesh[m].triangles[t].v[0]),
				&(renderer->rotatedZMesh[m].triangles[t].v[0]),
				renderer->RotationMatrixZ
			);
			Fast3D__matrixMultiplication(
				&(renderer->mesh[m].triangles[t].v[1]),
				&(renderer->rotatedZMesh[m].triangles[t].v[1]),
				renderer->RotationMatrixZ
			);
			Fast3D__matrixMultiplication(
				&(renderer->mesh[m].triangles[t].v[2]),
				&(renderer->rotatedZMesh[m].triangles[t].v[2]),
				renderer->RotationMatrixZ
			);
			
			Fast3D__matrixMultiplication(
				&(renderer->rotatedZMesh[m].triangles[t].v[0]),
				&(renderer->rotatedXMesh[m].triangles[t].v[0]),
				renderer->RotationMatrixX
			);
			Fast3D__matrixMultiplication(
				&(renderer->rotatedZMesh[m].triangles[t].v[1]),
				&(renderer->rotatedXMesh[m].triangles[t].v[1]),
				renderer->RotationMatrixX
			);
			Fast3D__matrixMultiplication(
				&(renderer->rotatedZMesh[m].triangles[t].v[2]),
				&(renderer->rotatedXMesh[m].triangles[t].v[2]),
				renderer->RotationMatrixX
			);

			// Translate
			for(int v = 0; v < 3; v++) {
				// Translate to world space
				renderer->translatedWorldMesh[m].triangles[t].v[v] = renderer->rotatedXMesh[m].triangles[t].v[v];
				renderer->translatedWorldMesh[m].triangles[t].v[v].z += 3.0;

				/*// Translate to camera space
				renderer->translatedCameraMesh[m].triangles[t].v[v].x = renderer->translatedWorldMesh[m].triangles[t].v[v].x + renderer->camera.pos.x;
				renderer->translatedCameraMesh[m].triangles[t].v[v].y = renderer->translatedWorldMesh[m].triangles[t].v[v].y + renderer->camera.pos.y;
				renderer->translatedCameraMesh[m].triangles[t].v[v].z = renderer->translatedWorldMesh[m].triangles[t].v[v].z + renderer->camera.pos.z;*/
			}

			// Generate normals
			renderer->line1.x = renderer->translatedWorldMesh[m].triangles[t].v[1].x - renderer->translatedWorldMesh[m].triangles[t].v[0].x;
			renderer->line1.y = renderer->translatedWorldMesh[m].triangles[t].v[1].y - renderer->translatedWorldMesh[m].triangles[t].v[0].y;
			renderer->line1.z = renderer->translatedWorldMesh[m].triangles[t].v[1].z - renderer->translatedWorldMesh[m].triangles[t].v[0].z;
			renderer->line2.x = renderer->translatedWorldMesh[m].triangles[t].v[2].x - renderer->translatedWorldMesh[m].triangles[t].v[0].x;
			renderer->line2.y = renderer->translatedWorldMesh[m].triangles[t].v[2].y - renderer->translatedWorldMesh[m].triangles[t].v[0].y;
			renderer->line2.z = renderer->translatedWorldMesh[m].triangles[t].v[2].z - renderer->translatedWorldMesh[m].triangles[t].v[0].z;

			renderer->normal.x = renderer->line1.y * renderer->line2.z - renderer->line1.z * renderer->line2.y;
			renderer->normal.y = renderer->line1.z * renderer->line2.x - renderer->line1.x * renderer->line2.z;
			renderer->normal.z = renderer->line1.x * renderer->line2.y - renderer->line1.y * renderer->line2.x;

			temp = sqrt(renderer->normal.x * renderer->normal.x + renderer->normal.y * renderer->normal.y + renderer->normal.z * renderer->normal.z);
			renderer->normal.x /= temp;
			renderer->normal.y /= temp;
			renderer->normal.z /= temp;

			if(renderer->normal.x * (renderer->translatedWorldMesh[m].triangles[t].v[0].x - renderer->camera.x) +
			   renderer->normal.y * (renderer->translatedWorldMesh[m].triangles[t].v[0].y - renderer->camera.y) +
			   renderer->normal.z * (renderer->translatedWorldMesh[m].triangles[t].v[0].z - renderer->camera.z) < 0.0) {
				// Apply light
				temp = sqrt(renderer->lightDirection.x * renderer->lightDirection.x + renderer->lightDirection.y * renderer->lightDirection.y + renderer->lightDirection.z * renderer->lightDirection.z);
				renderer->lightDirection.x /= temp;
				renderer->lightDirection.y /= temp;
				renderer->lightDirection.z /= temp;

				temp2 = renderer->normal.x * renderer->lightDirection.x + renderer->normal.y * renderer->lightDirection.y + renderer->normal.z * renderer->lightDirection.z;

				// Apply Projection Matrix
				Fast3D__matrixMultiplication(
					&(renderer->translatedWorldMesh[m].triangles[t].v[0]),
					&(renderer->projectedMesh[m].triangles[t].v[0]),
					renderer->ProjectionMatrix);
				Fast3D__matrixMultiplication(
					&(renderer->translatedWorldMesh[m].triangles[t].v[1]),
					&(renderer->projectedMesh[m].triangles[t].v[1]),
					renderer->ProjectionMatrix);
				Fast3D__matrixMultiplication(
					&(renderer->translatedWorldMesh[m].triangles[t].v[2]),
					&(renderer->projectedMesh[m].triangles[t].v[2]),
					renderer->ProjectionMatrix);

				// Scale to screen
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

				Fast3D__drawTriangle(renderer, m, t, true, temp2);
			}
		}
	}

	// Return without errors
	return 0;
}

void Fast3D__addMesh(Renderer* renderer, Mesh* mesh) {
	if(renderer->mCount == 0) {
		renderer->mesh = malloc(sizeof(Mesh));
		renderer->mesh[0] = *mesh;
		renderer->rotatedXMesh = malloc(sizeof(Mesh));
		renderer->rotatedXMesh[0].triangles = malloc(mesh->tCount * sizeof(Triangle));
		renderer->rotatedZMesh = malloc(sizeof(Mesh));
		renderer->rotatedZMesh[0].triangles = malloc(mesh->tCount * sizeof(Triangle));
		renderer->translatedWorldMesh = malloc(sizeof(Mesh));
		renderer->translatedWorldMesh[0].triangles = malloc(mesh->tCount * sizeof(Triangle));
		//renderer->translatedCameraMesh = malloc(sizeof(Mesh));
		//renderer->translatedCameraMesh[0].triangles = malloc(mesh->tCount * sizeof(Triangle));
		renderer->projectedMesh = malloc(sizeof(Mesh));
		renderer->projectedMesh[0].triangles = malloc(mesh->tCount * sizeof(Triangle));
	}
	else {
		renderer->mesh = realloc(renderer->mesh, (renderer->mCount + 1) * sizeof(Mesh));
		renderer->mesh[renderer->mCount] = *mesh;
		renderer->rotatedXMesh = realloc(renderer->rotatedXMesh, (renderer->mCount + 1) * sizeof(Mesh));
		renderer->rotatedXMesh[renderer->mCount].triangles = malloc(mesh->tCount * sizeof(Triangle));
		renderer->rotatedZMesh = realloc(renderer->rotatedZMesh, (renderer->mCount + 1) * sizeof(Mesh));
		renderer->rotatedZMesh[renderer->mCount].triangles = malloc(mesh->tCount * sizeof(Triangle));
		renderer->translatedWorldMesh = realloc(renderer->translatedWorldMesh, (renderer->mCount + 1) * sizeof(Mesh));
		renderer->translatedWorldMesh[renderer->mCount].triangles = malloc(mesh->tCount * sizeof(Triangle));
		//renderer->translatedCameraMesh = realloc(renderer->translatedCameraMesh, (renderer->mCount + 1) * sizeof(Mesh));
		//renderer->translatedCameraMesh[renderer->mCount].triangles = malloc(mesh->tCount * sizeof(Triangle));
		renderer->projectedMesh = realloc(renderer->projectedMesh, (renderer->mCount + 1) * sizeof(Mesh));
		renderer->projectedMesh[renderer->mCount].triangles = malloc(mesh->tCount * sizeof(Triangle));
	}
	renderer->mCount++;
}
