#include "Fast3D.h"
Mesh cube;
void init_cube() {
	cube.triangles = (Triangle*)malloc(12 * sizeof(Triangle));
	cube.tCount = 12;

	cube.triangles[0] = (Triangle){
		(Vertex){0.0f, 0.0f, 0.0f},
		(Vertex){0.0f, 1.0f, 0.0f},
		(Vertex){1.0f, 1.0f, 0.0f}
	};
	cube.triangles[1] = (Triangle){
		(Vertex){0.0f, 0.0f, 0.0f},
		(Vertex){1.0f, 1.0f, 0.0f},
		(Vertex){1.0f, 0.0f, 0.0f}
	};
	cube.triangles[2] = (Triangle){
		(Vertex){1.0f, 0.0f, 0.0f},
		(Vertex){1.0f, 1.0f, 0.0f},
		(Vertex){1.0f, 1.0f, 1.0f}
	};
	cube.triangles[3] = (Triangle){
		(Vertex){1.0f, 0.0f, 0.0f},
		(Vertex){1.0f, 1.0f, 1.0f},
		(Vertex){1.0f, 0.0f, 1.0f}
	};
	cube.triangles[4] = (Triangle){
		(Vertex){1.0f, 0.0f, 1.0f},
		(Vertex){1.0f, 1.0f, 1.0f},
		(Vertex){0.0f, 1.0f, 1.0f}
	};
	cube.triangles[5] = (Triangle){
		(Vertex){1.0f, 0.0f, 1.0f},
		(Vertex){0.0f, 1.0f, 1.0f},
		(Vertex){0.0f, 0.0f, 1.0f}
	};
	cube.triangles[6] = (Triangle){
		(Vertex){0.0f, 0.0f, 1.0f},
		(Vertex){0.0f, 1.0f, 1.0f},
		(Vertex){0.0f, 1.0f, 0.0f}
	};
	cube.triangles[7] = (Triangle){
		(Vertex){0.0f, 0.0f, 1.0f},
		(Vertex){0.0f, 1.0f, 0.0f},
		(Vertex){0.0f, 0.0f, 0.0f}
	};
	cube.triangles[8] = (Triangle){
		(Vertex){0.0f, 1.0f, 0.0f},
		(Vertex){0.0f, 1.0f, 1.0f},
		(Vertex){1.0f, 1.0f, 1.0f}
	};
	cube.triangles[9] = (Triangle){
		(Vertex){0.0f, 1.0f, 0.0f},
		(Vertex){1.0f, 1.0f, 1.0f},
		(Vertex){1.0f, 1.0f, 0.0f}
	};
	cube.triangles[10] = (Triangle){
		(Vertex){1.0f, 0.0f, 1.0f},
		(Vertex){0.0f, 0.0f, 1.0f},
		(Vertex){0.0f, 0.0f, 0.0f}
	};
	cube.triangles[11] = (Triangle){
		(Vertex){1.0f, 0.0f, 1.0f},
		(Vertex){0.0f, 0.0f, 0.0f},
		(Vertex){1.0f, 0.0f, 0.0f}
	};
}