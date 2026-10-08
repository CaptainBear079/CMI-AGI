#include "main.h"

// Global variables
// - General system variables
short args = 0;                         // Argument bitmap
Save save;                              // Save file data
int commands[5];                       // Command queue
pthread_t env_thread_id;                // pthread_t for the environment thread
int ENV_ret = 0;                        // Return code of the environment thread
pthread_t ctrl_thread_id;               // pthread_t for the control thread
int CTRL_ret = 0;                       // Return code of the control thread
bool exitFlag = false;                  // Shutdown signal to env_thread and control_thread

// Window Manager
WM windows;

// Entity Manager
EM entityManager;

// 3D environment_thread
void* env_thread(void* arg) {
	init_cube();
	Renderer* renderer = Fast3D__init(CMI__WINDOW_WIDTH, CMI__WINDOW_HEIGHT, 90.0);
	Fast3D__addMesh(renderer, &cube);
	WM__createImage(&windows, 0, renderer->fb);
	while(!exitFlag) {
		Fast3D__render(renderer);

		WM__updateImage(&windows, 0);

		WM__updateWindow(&windows);
	}
	Fast3D__destroy(renderer);
	ENV_ret = 0;
	return NULL; // Shutdown via exit code signal
}

// Control GUI thread
#ifdef _GUI_SUPPORT
void* control_thread(void* arg) {
	#ifdef _W_X11
	XEvent ev;
	while(XNextEvent(windows.display, &ev) == 0 && !exitFlag) {
		switch(ev.type) {
			case ButtonPress: {
				CTRL_ret = 0;
				return NULL;
			} break;
		}
	}
	#endif
	CTRL_ret = 0;
	return NULL;
}
#endif

// Control function
int control_function() {
	// Variables
	int command = 0;
	int i = 0;

	while(true) {
		usleep(10000);
		if(args & CMI__ARG_GUI_MODE) {
			command = commands[i];
		}
		switch(command){
			// No command
			case 0: break;

			// Create AI
			case 1: {} break;

			// Import AI
			case 2: {} break;

			// Export AI
			case 3: {} break;

			// Remove AI
			case 4: {} break;

			// Create 3D Model
			case 5: {} break;

			// Import 3D Model
			case 6: {} break;

			// Export 3D Model
			case 7: {} break;

			// Remove 3D Model
			case 8: {} break;

			// Start Simulation
			case 9: { EM__startSimulation(&entityManager); } break;

			// Kill Simulation
			case 10: { EM__quitSimulation(&entityManager); } break;

			// Quit
			case 11: { return 0; } break;
			default: { printf("Invalid command.\n"); } break;
		}
		if(args & CMI__ARG_GUI_MODE) {
			commands[i] = 0;
		}
		if(i >= 5) {
			i = 0;
		}
	}
	return -1; // Unexpected error
}

// Control thread
int main(int argc, char* argv[]) {
	//
	// Setup
	//
	printf("[Setup]: Interpreting argumentss...\n");
	// - Interpreting arguments
	for(int i = 1; i < argc; i++) {
		if(strcmp("--restore", argv[i]) == 0) {
			args = args | CMI__ARG_RESTORE_SESSION;
		}
		else if(strcmp("-gui", argv[i]) == 0) {
			args = args | CMI__ARG_GUI_MODE;
		}
	}
	if(args & CMI__ARG_RESTORE_SESSION) {
		loadSave(&save); // INDEV
	}

	printf("[Setup]: Preparing entity manager...\n");
	// - Prepare entity manager
	EM__init(&entityManager, (args & CMI__ARG_GUI_MODE));

	printf("[Setup]: Preparing window(s)...\n");
	// - Prepare windows (control window GUI mode only)
	//   - Setup Window Manager
	if(WM__useMultithreading) {
		return -1;
	}

	WM__openDisplay(&windows);

	#ifdef _GUI_SUPPORT
	windows.windows = calloc(2, sizeof(WM__Window));
	windows.windows[0].windowBorderWidth = 5;
	if(args & CMI__ARG_GUI_MODE) {
		windows.windows[1].windowBorderWidth = 5;
	}
	#else
	windows.windows = calloc(1, sizeof(WM__Window));
	windows.windows[0].windowBorderWidth = 5;
	if(args & CMI__ARG_GUI_MODE) {
		printf("[Setup]: GUI mode is not supported in this build.\n");
		return 1;
	}
	#endif

	windows.windows[0].windowWidth = CMI__WINDOW_WIDTH;
	windows.windows[0].windowHeight = CMI__WINDOW_HEIGHT;
	
	WM__createWindow(
		&windows, 0,
		BlackPixel(windows.display, windows.screen),
		WhitePixel(windows.display, windows.screen),
		NoEventMask, DefaultDepth(windows.display, windows.screen),
		DefaultVisual(windows.display, windows.screen),
		InputOutput
	);

	WM__createGraphicsContext(&windows, 0);

	#ifdef _GUI_SUPPORT
	if(args & CMI__ARG_GUI_MODE) {
		windows.windows[1].windowWidth = CMI__WINDOW_WIDTH;
		windows.windows[1].windowHeight = CMI__WINDOW_HEIGHT;
		WM__createWindow(
			&windows, 1,
			WhitePixel(windows.display, windows.screen),
			BlackPixel(windows.display, windows.screen),
			KeyPressMask | KeyReleaseMask | ButtonPressMask | PointerMotionMask,
			DefaultDepth(windows.display, windows.screen),
			DefaultVisual(windows.display, windows.screen),
			InputOutput
		);
	}
	#endif

	printf("[Setup]: Starting 3D environment... ");
	pthread_create(&env_thread_id, NULL, (void*)env_thread, NULL);
	if(args & CMI__ARG_GUI_MODE) {
		printf("Started!\n[Setup]: Starting control thread... ");
		pthread_create(&ctrl_thread_id, NULL, (void*)control_thread, NULL);
		printf("Started!\n");
	}
	else {
		printf("Started!\n");
	}
	
	printf("Setup complete.\n");

	//
	// Main loop
	//
	int exitCode = control_function();
	exitFlag = true;
	pthread_join(env_thread_id, NULL);
	if(args & CMI__ARG_GUI_MODE) {
		pthread_join(ctrl_thread_id, NULL);
	}

	//
	// Cleanup
	//
	WM__destroyWindow(&windows, 0);
	#ifdef _GUI_SUPPORT
	if(args & CMI__ARG_GUI_MODE) {
		WM__destroyWindow(&windows, 1);
	}
	#endif
	WM__closeDisplay(&windows);

	printf("[Environment]: Exit code: %d\n", ENV_ret);
	if(args & CMI__ARG_GUI_MODE) {
		printf("[GUI Control]: Exit code: %d\n", CTRL_ret);
	}
	printf("[Controller]: Exit code: %d\n", exitCode);
	return exitCode;
}
