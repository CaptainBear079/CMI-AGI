#include "viewport.h"

void* viewportThread(void* arg) {
	// Initialize Fast3D
	Renderer* renderer = Fast3D__init(CMI__ENV_WINDOW_WIDTH, CMI__ENV_WINDOW_HEIGHT, 90.0);
	WM__createImage(&windows, 0, renderer->fb);

	// Render loop
	while(!exitFlag) {
		Fast3D__simpleRender(renderer, false);

		WM__updateImage(&windows, 0);

		WM__updateWindow(&windows);
	}

	// Shutdown via exit flag
	Fast3D__destroy(renderer);
	viewportRet = 0;
	return NULL;
}
