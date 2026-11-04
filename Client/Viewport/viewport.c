#include "viewport.h"

void* viewportThread(void* arg) {
	// Variables
	int i = 0;

	// Initialize Fast3D
	Renderer* renderer = Fast3D__init(CMI__ENV_WINDOW_WIDTH, CMI__ENV_WINDOW_HEIGHT, 90.0);
	WM__createImage(&windows, 0, renderer->fb);

	// Render loop
	while(!exitFlag) {
		// Check for viewport updates
		switch(eventQueue[i].type) {
			case CMI__VP_EVENT__NO_EVENT: break;
			case CMI__VP_EVENT__ADD_MODEL: {} break;
			case CMI__VP_EVENT__REMOVE_MODEL: {} break;
			default: {
				fprintf(stderr, "[Viewport]: WARNING: Invalid event codes.\nIgnoring invalid event codes.\n");
			} break;
		}

		// Render viewport
		Fast3D__simpleRender(renderer, false);
		WM__updateImage(&windows, 0);
		WM__updateWindow(&windows);
	}

	// Shutdown via exit flag
	Fast3D__destroy(renderer);
	viewportRet = 0;
	return NULL;
}
