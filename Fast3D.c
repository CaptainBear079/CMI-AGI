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
int matrixMultiplication(Vertex* i, Vertex* o, double matrix[4][4]) {
	o->x = i->x * matrix[0][0] + i->y * matrix[1][0] + i->z * matrix[2][0] + matrix[3][0];
	o->y = i->x * matrix[0][1] + i->y * matrix[1][1] + i->z * matrix[2][1] + matrix[3][1];
	o->z = i->x * matrix[0][2] + i->y * matrix[1][2] + i->z * matrix[2][2] + matrix[3][2];
	double w = i->x * matrix[0][3] + i->y * matrix[1][3] + i->z * matrix[2][3] + matrix[3][3];
	if(w != 0.0) {
		o->x /= w;
		o->y /= w;
		o->z /= w;
	}
}

// Render function
int render(uint32_t* framebuffer, int fb_width, int fb_height, double pFOV) {
	// Setup
	time_t now = time(NULL);
	ElapsedTime = difftime(now, then);
	fb_total_size = fb_width * fb_height;
	double rTheta = 1.0 * ElapsedTime;
	double ProjectionMatrix[4][4] = { 0.0 };
	double RotationMatrixX[4][4] = { 0.0 };
	RotationMatrixX[0][0] = cos(rTheta);
	RotationMatrixX[0][1] = sin(rTheta);
	RotationMatrixX[1][0] = -sin(rTheta);
	RotationMatrixX[1][1] = cos(rTheta);
	RotationMatrixX[2][2] = 1.0;
	RotationMatrixX[3][3] = 1.0;
	double RotationMatrixZ[4][4] = { 0.0 };
	RotationMatrixZ[0][0] = 1.0;
	RotationMatrixZ[1][1] = cos(rTheta * 0.5);
	RotationMatrixZ[1][2] = sin(rTheta * 0.5);
	RotationMatrixZ[2][1] = -sin(rTheta * 0.5);
	RotationMatrixZ[2][2] = cos(rTheta * 0.5);
	RotationMatrixZ[3][3] = 1.0;
	Mesh cube = { .triangles = malloc(12 * sizeof(Triangle)) };
	Mesh rotatedXCube = { .triangles = malloc(12 * sizeof(Triangle)) };
	Mesh rotatedZCube = { .triangles = malloc(12 * sizeof(Triangle)) };
	Mesh translatedCube = { .triangles = malloc(12 * sizeof(Triangle)) };
	Mesh projectedCube = { .triangles = malloc(12 * sizeof(Triangle)) };
	{
	cube.triangles[0] = (Triangle){
		(Vertex){ 0.0, 0.0, 0.0 },
		(Vertex){ 1.0, 0.0, 0.0 },
		(Vertex){ 0.0, 1.0, 0.0 }
	};
	cube.triangles[1] = (Triangle){
		(Vertex){ 0.0, 1.0, 0.0 },
		(Vertex){ 1.0, 1.0, 0.0 },
		(Vertex){ 1.0, 0.0, 0.0 }
	};
	cube.triangles[2] = (Triangle){
		(Vertex){ 0.0, 1.0, 0.0 },
		(Vertex){ 0.0, 0.0, 0.0 },
		(Vertex){ 0.0, 1.0, 1.0 }
	};
	cube.triangles[3] = (Triangle){
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 0.0, 0.0, 0.0 },
		(Vertex){ 0.0, 0.0, 1.0 }
	};
	cube.triangles[4] = (Triangle){
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 1.0, 1.0, 1.0 }
	};
	cube.triangles[5] = (Triangle){
		(Vertex){ 1.0, 1.0, 1.0 },
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 1.0 }
	};
	cube.triangles[6] = (Triangle){
		(Vertex){ 1.0, 1.0, 1.0 },
		(Vertex){ 1.0, 0.0, 1.0 },
		(Vertex){ 1.0, 1.0, 0.0 }
	};
	cube.triangles[7] = (Triangle){
		(Vertex){ 1.0, 1.0, 0.0 },
		(Vertex){ 1.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 0.0 }
	};
	cube.triangles[8] = (Triangle){
		(Vertex){ 0.0, 1.0, 0.0 },
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 1.0, 1.0, 0.0 }
	};
	cube.triangles[9] = (Triangle){
		(Vertex){ 1.0, 1.0, 1.0 },
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 1.0, 1.0, 0.0 }
	};
	cube.triangles[10] = (Triangle){
		(Vertex){ 0.0, 0.0, 0.0 },
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 0.0 }
	};
	cube.triangles[11] = (Triangle){
		(Vertex){ 1.0, 0.0, 1.0 },
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 0.0 }
	};
	}
	
	//Clear framebuffer
	for(int i = 0; i < fb_total_size; i++) {
		framebuffer[i] = (uint32_t)0;
	}

	// Prepare Projection Matrix
	double pNear = 0.02;
	double pFar = 4800.0;
	double pAspectRatio = fb_height/fb_width;
	double pFOVRad = 1.0/ tan(pFOV * 0.5 / 180.0 * 3.141592653589793);
	ProjectionMatrix[0][0] = pAspectRatio * pFOVRad;
	ProjectionMatrix[1][1] = pFOVRad;
	ProjectionMatrix[2][2] = pFar / (pFar - pNear);
	ProjectionMatrix[3][2] = (-pFar * pNear) / (pFar - pNear);
	ProjectionMatrix[2][3] = 1.0;
	ProjectionMatrix[3][3] = 0.0;

	// Apply Rotation (Rotation Matrix)
	for(int i = 0; i < 12; i++) {
		matrixMultiplication(&(cube.triangles[i].v[0]), &(rotatedXCube.triangles[i].v[0]), RotationMatrixX);
		matrixMultiplication(&(cube.triangles[i].v[1]), &(rotatedXCube.triangles[i].v[1]), RotationMatrixX);
		matrixMultiplication(&(cube.triangles[i].v[2]), &(rotatedXCube.triangles[i].v[2]), RotationMatrixX);
		matrixMultiplication(&(rotatedXCube.triangles[i].v[0]), &(rotatedZCube.triangles[i].v[0]), RotationMatrixZ);
		matrixMultiplication(&(rotatedXCube.triangles[i].v[1]), &(rotatedZCube.triangles[i].v[1]), RotationMatrixZ);
		matrixMultiplication(&(rotatedXCube.triangles[i].v[2]), &(rotatedZCube.triangles[i].v[2]), RotationMatrixZ);
	}
	
	// Translate
	for(int i = 0; i < 12; i++) {
		for(int j = 0; j < 3; j++) {
			translatedCube.triangles[i].v[j].x = rotatedZCube.triangles[i].v[j].x + cube.pos.x;
			translatedCube.triangles[i].v[j].y = rotatedZCube.triangles[i].v[j].x + cube.pos.y;
			translatedCube.triangles[i].v[j].z = rotatedZCube.triangles[i].v[j].x + cube.pos.z;
		}
	}

	// Apply Projection Matrix
	for(int i = 0; i < 12; i++) {
		matrixMultiplication(&(translatedCube.triangles[i].v[0]), &(projectedCube.triangles[i].v[0]), ProjectionMatrix);
		matrixMultiplication(&(translatedCube.triangles[i].v[1]), &(projectedCube.triangles[i].v[1]), ProjectionMatrix);
		matrixMultiplication(&(translatedCube.triangles[i].v[2]), &(projectedCube.triangles[i].v[2]), ProjectionMatrix);
	}

	// Scale to screen
	for(int i = 0; i < 12; i++) {
		projectedCube.triangles[i].v[0].x += 1.0;
		projectedCube.triangles[i].v[1].x += 1.0;
		projectedCube.triangles[i].v[2].x += 1.0;
		projectedCube.triangles[i].v[0].y += 1.0;
		projectedCube.triangles[i].v[1].y += 1.0;
		projectedCube.triangles[i].v[2].y += 1.0;

		projectedCube.triangles[i].v[0].x *= 0.5 * fb_width;
		projectedCube.triangles[i].v[1].x *= 0.5 * fb_width;
		projectedCube.triangles[i].v[2].x *= 0.5 * fb_width;
		projectedCube.triangles[i].v[0].y *= 0.5 * fb_height;
		projectedCube.triangles[i].v[1].y *= 0.5 * fb_height;
		projectedCube.triangles[i].v[2].y *= 0.5 * fb_height;
	}

	// Rasterization
	// - Find the smallest and biggest x and y
	int sX[12], bX[12];
	for(int i = 0; i < 12; i++) {
		sX[i] = projectedCube.triangles[i].v[0].x;
		if(projectedCube.triangles[i].v[1].x > sX[i]) {
			sX[i] = projectedCube.triangles[i].v[1].x;
		}
		if(projectedCube.triangles[i].v[2].x > sX[i]) {
			sX[i] = projectedCube.triangles[i].v[2].x;
		}

		bX[i] = projectedCube.triangles[i].v[0].x;
		if(projectedCube.triangles[i].v[1].x > bX[i]) {
			bX[i] = projectedCube.triangles[i].v[1].x;
		}
		if(projectedCube.triangles[i].v[2].x > bX[i]) {
			bX[i] = projectedCube.triangles[i].v[2].x;
		}
	}

	// Return without errors
	then = time(NULL);
	return 0;
}
