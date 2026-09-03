#include "Fast3D.h"
Mesh cube;
void init_cube() {
	cube.triangles = (Triangle*)malloc(12 * sizeof(Triangle));
	cube.tCount = 12;

	// Standard olcEngine3D-style cube, with consistent winding so the mesh stays rigid
	cube.triangles[0] = (Triangle){
		(Vertex){ 0.0, 0.0, 0.0 },
		(Vertex){ 0.0, 1.0, 0.0 },
		(Vertex){ 1.0, 1.0, 0.0 }
	};
	cube.triangles[1] = (Triangle){
		(Vertex){ 0.0, 0.0, 0.0 },
		(Vertex){ 1.0, 1.0, 0.0 },
		(Vertex){ 1.0, 0.0, 0.0 }
	};

	cube.triangles[2] = (Triangle){
		(Vertex){ 1.0, 0.0, 0.0 },
		(Vertex){ 1.0, 1.0, 0.0 },
		(Vertex){ 1.0, 1.0, 1.0 }
	};
	cube.triangles[3] = (Triangle){
		(Vertex){ 1.0, 0.0, 0.0 },
		(Vertex){ 1.0, 1.0, 1.0 },
		(Vertex){ 1.0, 0.0, 1.0 }
	};

	cube.triangles[4] = (Triangle){
		(Vertex){ 1.0, 0.0, 1.0 },
		(Vertex){ 1.0, 1.0, 1.0 },
		(Vertex){ 0.0, 1.0, 1.0 }
	};
	cube.triangles[5] = (Triangle){
		(Vertex){ 1.0, 0.0, 1.0 },
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 0.0, 0.0, 1.0 }
	};

	cube.triangles[6] = (Triangle){
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 0.0, 1.0, 0.0 }
	};
	cube.triangles[7] = (Triangle){
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 0.0, 1.0, 0.0 },
		(Vertex){ 0.0, 0.0, 0.0 }
	};

	cube.triangles[8] = (Triangle){
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 0.0 }
	};
	cube.triangles[9] = (Triangle){
		(Vertex){ 0.0, 0.0, 1.0 },
		(Vertex){ 1.0, 0.0, 0.0 },
		(Vertex){ 0.0, 0.0, 0.0 }
	};

	cube.triangles[10] = (Triangle){
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 1.0, 1.0, 1.0 },
		(Vertex){ 1.0, 1.0, 0.0 }
	};
	cube.triangles[11] = (Triangle){
		(Vertex){ 0.0, 1.0, 1.0 },
		(Vertex){ 1.0, 1.0, 0.0 },
		(Vertex){ 0.0, 1.0, 0.0 }
	};
}