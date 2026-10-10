#include "main.h"

// Global variables
// - General system variables
short args = 0;                         // Argument bitmap
Save save;                              // Save file data
int commands[6];                        // Command queue
pthread_t env_thread_id;                // pthread_t for the environment thread
int ENV_ret = 0;                        // Return code of the environment thread
pthread_t ctrl_thread_id;               // pthread_t for the control thread
int CTRL_ret = 0;                       // Return code of the control thread
bool noNextCommand = false;             // Skip the printing of "\n$ "
bool exitFlag = false;                  // Shutdown signal to env_thread and control_thread

// Window Manager
WM windows;

// Entity Manager
EM entityManager;

// 3D environment_thread
void* env_thread(void* arg) {
	init_cube();
	Renderer* renderer = Fast3D__init(CMI__ENV_WINDOW_WIDTH, CMI__ENV_WINDOW_HEIGHT, 90.0);
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
	while(!exitFlag) {
		if(XPending(windows.display) > 0) {
			if(XNextEvent(windows.display, &ev) != 0) {
				exitFlag = true;
				fprintf(stderr, "[GUI Control]: Error from XNextEvent.\n");
				CTRL_ret = 1;
				return NULL;
			}
		}
		switch(ev.type) {
			case ButtonPress: {
				commands[1] = 11;
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

// Interpret command
int CTRL_FUNC__interpretCommand(char buffer[]) {
	int i = 0;
	char buf[6];
	buf[5] = '\0';
	while(i < 6) {
		if(buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\0') {
			buf[i] = '\0';
			break;
		}
		buf[i] = buffer[i];
		i++;
	}

	if(strcmp("start", buf) == 0) {
		return 9;
	}
	else if(strcmp("kill", buf) == 0) {
		return 10;
	}
	else if(strcmp("quit", buf) == 0) {
		return 11;
	}
	return 0;
}

// Control function
int control_function() {
	// Variables
	char buffer[257];
	buffer[256] = '\0';
	fd_set cmd, cmd_copy;
	FD_ZERO(&cmd);
	FD_SET(STDIN_FILENO, &cmd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;
	// Command interpretion
	printf("\n$ ");
	fflush(stdout);
	while(!exitFlag) {
		// Reset buffer
		for(int i = 0; i < 256; i++) {
			buffer[i] = '\0';
		}

		// Check for stdin input
		cmd_copy = cmd;
		if(select(STDIN_FILENO + 1, &cmd_copy, NULL, NULL, &timeout) < 0) {
			fprintf(stderr, "[Controller] Error while waiting for input from stdin.\n");
			return 1;
		}

		// Read input
		if(FD_ISSET(STDIN_FILENO, &cmd_copy)) {
			fgets(buffer, sizeof(buffer), stdin);
		}

		// Interpret command
		commands[0] = CTRL_FUNC__interpretCommand(buffer);

		// Check for and run commands (both stdin and control_thread)
		for(int i = 0; i < 6; i++) {
			noNextCommand = false;
			switch(commands[i]){
				// No command
				case 0: {
					noNextCommand = true;
				} break;

				// Create AI
				case 1: {
					EM__createEntity(&entityManager, entityManager.entityCount, "Test 1", EM_ENTITY__TYPE_CPLUGIN, "./build/dev-test-v0.0.1.so");
				} break;

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
				case 9: {
					printf("[Controller] Starting simulation...");
					fflush(stdout);
					EM__startSimulation(&entityManager);
					printf(" Success!\n");
				} break;

				// Kill Simulation
				case 10: {
					printf("[Controller] Ending simulation...");
					fflush(stdout);
					EM__quitSimulation(&entityManager);
					printf(" Success!\n");
				} break;

				// Quit
				case 11: {
					noNextCommand = true;
					printf("\n");
					return 0;
				} break;
				default: { printf("Invalid command.\n"); } break;
			}
			if(!noNextCommand) {
				printf("\n$ ");
				fflush(stdout);
			}
			commands[i] = 0;
		}
	}
	printf("\n");
	return 1; // Unexpected error, exitFlag set by other thread
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

	windows.windows[0].windowX = CMI__ENV_WINDOW_POS_X;
	windows.windows[0].windowY = CMI__ENV_WINDOW_POS_Y;
	windows.windows[0].windowWidth = CMI__ENV_WINDOW_WIDTH;
	if(args & CMI__ARG_GUI_MODE) {
		windows.windows[0].windowHeight = CMI__ENV_WINDOW_HEIGHT_GUI;
	}
	else {
		windows.windows[0].windowHeight = CMI__ENV_WINDOW_HEIGHT;
	}
	
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
		windows.windows[1].windowX = CMI__CTRL_WINDOW_POS_X;
		windows.windows[1].windowY = CMI__CTRL_WINDOW_POS_Y;
		windows.windows[1].windowWidth = CMI__CTRL_WINDOW_WIDTH;
		windows.windows[1].windowHeight = CMI__CTRL_WINDOW_HEIGHT;
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
