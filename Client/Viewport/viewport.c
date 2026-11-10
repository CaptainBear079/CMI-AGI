#include "../../shared.h"

extern Client* client;

void* viewportThread(void* arg) {
	// Variables
	int i = 0;

	// Initialize Fast3D
	Renderer* renderer = Fast3D__init(CMI__ENV_WINDOW_WIDTH, CMI__ENV_WINDOW_HEIGHT, 90.0);
	WM__createImage(&CMI_windows, 0, renderer->fb);

	// Render loop
	while(!CMI_exitFlag) {
		// Check for viewport updates
		switch(CMI_VP_eventQueue[i].type) {
			case CMI__VP_EVENT__NO_EVENT: break;
			case CMI__VP_EVENT__ADD_MODEL: {
				CMI_dimensions[CMI_VP_d].objects[CMI_VP_E__addModelPAYLOAD(i)->id].id = CMI_VP_E__addModelPAYLOAD(i)->id;
				CMI_Client__setUniqueID(CMI_VP_E__addModelPAYLOAD(i)->id);
				if(CMI_VP_E__addModelPAYLOAD(i)->type) {
					Fast3D__addMesh(renderer, CMI_VP_E__addModelPAYLOAD(i)->CMI_VP_EP_AM__Model.mesh);
				}
			} break;
			case CMI__VP_EVENT__REMOVE_MODEL: {} break;
			default: {
				fprintf(stderr, "[Viewport]: WARNING: Invalid event codes.\nIgnoring invalid event codes.\n");
			} break;
		}

		// Render viewport
		Fast3D__simpleRender(renderer, false);
		WM__updateImage(&CMI_windows, 0);
		WM__updateWindow(&CMI_windows);
	}

	// Shutdown via exit flag
	Fast3D__destroy(renderer);
	CMI_viewportRet = 0;
	return NULL;
}
