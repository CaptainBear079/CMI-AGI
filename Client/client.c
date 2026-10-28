#include "client.h"

// Global variables
short args = 0;             // Argument bitmap
int commands[6];            // Command queue
void** commandsArgs[6];     // Argument lists for commands
WM windows;                 // The window handler
pthread_t viewportThreadID; // pthread_t for the environment thread
pthread_t controlThreadID;  // pthread_t for the control thread
int viewportRet = 0;        // Return code of the environment thread
int controlRet = 0;         // Return code of the control thread
bool exitFlag = false;      // Shutdown signal to viewportThread and controlThread

#ifdef _GUI_SUPPORT
// Control GUI thread
void* controlThread(void* arg) {
	#ifdef _W_X11
	XEvent ev;
	while(!exitFlag) {
		if(XPending(windows.display) > 0) {
			if(XNextEvent(windows.display, &ev) != 0) {
				exitFlag = true;
				fprintf(stderr, "[GUI Control]: Error from XNextEvent.\n");
				controlRet = -1;
				return NULL;
			}
		}
		switch(ev.type) {
			case ButtonPress: {
				commands[1] = 11;
				controlRet = 0;
				return NULL;
			} break;
		}
	}
	#endif
	controlRet = 0;
	return NULL;
}
#endif

// Control function
int controlFunction() {
	return 0;
}

int main(int argc, char* argv[]) {
	//
	// Setup
	//
	// - Interpreting arguments
	printf("[Client Setup]: Interpreting argumentss...\n");
	for(int i = 1; i < argc; i++) {
		if(strcmp("-gui", argv[i]) == 0) {
			args = args | CMI__ARG_GUI_MODE;
		}
		else {
			fprintf(stderr, "[Client Setup]: ERROR: Invalid or Uncomplete argument. Exit code: %d\n", CMI_ERROR__INVALID_ARG);
			return CMI_ERROR__INVALID_ARG;
		}
	}

	// Opening window(s)
	WM__openDisplay(&windows);
	if(!WM__useMultithreading) {
		fprintf(stderr, "[Client Setup]: ERROR: X11 multithreading unsupported. Exit code: %d\n", CMI_ERROR__NO_MULTITHREADING);
		return CMI_ERROR__NO_MULTITHREADING;
	}
	if(args & CMI__ARG_GUI_MODE) {
		// Opening control window
		#ifndef _GUI_SUPPORT
		fprintf(stderr, "[Client Setup]: ERROR: This build doesn't support GUI mode. Exit code: %d\n", CMI_ERROR__NO_GUI_SUPPORT);
		#else
		printf("[Client Setup]: Opening control window...\n");
		windows.windows = calloc(2, sizeof(WM__Window));
		windows.windows[0].windowBorderWidth = 5;
		windows.windows[1].windowBorderWidth = 5;
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
		#endif
	}
	else {
		printf("[Client Setup]: Running console mode...\n");
		windows.windows = calloc(1, sizeof(WM__Window));
		windows.windows[0].windowBorderWidth = 5;
	}

	// Opening 3D viewport
	printf("[Client Setup]: Opening 3D viewport...\n");
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

	printf("[Client Setup]: Starting 3D viewport... ");
	fflush(stdout);
	pthread_create(&viewportThreadID, NULL, (void*)viewportThread, NULL);
	if(args & CMI__ARG_GUI_MODE) {
		printf("Started!\n[Client Setup]: Starting control thread... ");
		fflush(stdout);
		pthread_create(&controlThreadID, NULL, (void*)controlThread, NULL);
	}
	printf("Started!\n");

	printf("[Client Setup]: Complete!");

	//
	// Main loop
	//
	int exitCode = controlFunction();
	exitFlag = true;
	pthread_join(viewportThreadID, NULL);
	if(args & CMI__ARG_GUI_MODE) {
		pthread_join(controlThreadID, NULL);
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

	printf("[Environment]: Exit code: %d\n", viewportRet);
	if(args & CMI__ARG_GUI_MODE) {
		printf("[GUI Control]: Exit code: %d\n", controlRet);
	}
	printf("[Controller]: Exit code: %d\n", exitCode);
	return exitCode;
}
