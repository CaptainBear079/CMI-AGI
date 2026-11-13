#include "client.h"

Client* client;

#ifdef _GUI_SUPPORT
// Control GUI thread
void* controlThread(void* arg) {
	#ifdef _W_X11
	XEvent ev;
	while(!CMI_exitFlag) {
		if(XPending(CMI_windows.display) > 0) {
			if(XNextEvent(CMI_windows.display, &ev) != 0) {
				CMI_exitFlag = true;
				fprintf(stderr, "[GUI Control]: Error from XNextEvent.\n");
				CMI_controlRet = -1;
				return NULL;
			}
			switch(ev.type) {
				case ButtonPress: {
					CMI_commands[1] = 11;
					CMI_controlRet = 0;
					return NULL;
				} break;
			}
		}
		usleep(1000);
	}
	#endif
	CMI_controlRet = 0;
	return NULL;
}
#endif

// Control function Command interpreter (CTRL_FUNC__interpretCommand V2) Client Version
int CTRL_FUNC__interpretCommand(char buffer[]) {
	size_t t = 8;
	Token* tokens = tokenize(buffer, 8, &t);

	if(false) {
	}
	else {
		int temp = 0;
		if(strcmp("quit", tokens[0].text) == 0) {
			temp = CMI__CTRL_FUNC__QUIT;
		}
		
		for(int i2 = 0; i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return temp;
	}
	
	fprintf(stderr, "[Controller]: ERROR: No or Invalid command.");
	return 0; // No/Invalid command
}

// Control function
int controlFunction() {
	// Variables
	char buffer[257];
	buffer[256] = '\0';
	fd_set cmd, cmd_copy;
	FD_ZERO(&cmd);
	FD_SET(STDIN_FILENO, &cmd);
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;

	if(CMI_args & CMI__ARG_GUI_MODE) {
		// First command promt
		printf("\n $");
		fflush(stdout);
	}
	while(!CMI_exitFlag) {
		if(CMI_args & CMI__ARG_GUI_MODE) {
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

				// Interpret command
				CMI_commands[0] = CTRL_FUNC__interpretCommand(buffer);
			}
		}

		for(int i = 0; i < 6; i++) {
			if(CMI_commands[i] != 0) {
				switch(CMI_commands[i]) {
					// Server update
					case CMI__CTRL_FUNC__NET_UPDATE: {
						switch(CMI_eventQueue[i].type) {
							case CMI_SUE__NEW_MODEL: {
								CMI_dimensions[CMI_VP_d].objects[CMI_eventPayload(i).newModelPayload->id].type = CMI_OBJ__3D_MODEL;
							} break;
						}
					} break;
					case CMI__CTRL_FUNC__QUIT: {
						printf("\n");
						return 0;
					}
				}
			}
		}
	}
	return 0;
}

int main(int argc, char* argv[]) {
	//
	// Setup
	//
	// Init client
	client = malloc(sizeof(Client));
	client->args = 0;

	client->eventQueue = malloc(16 * sizeof(CMI_VP__EVENT));

	client->viewportRet = 0;
	client->controlRet = 0;

	client->exitFlag = false;

	client->d = 0;

	// - Interpreting arguments
	printf("[Client Setup]: Interpreting argumentss...\n");
	for(int i = 1; i < argc; i++) {
		if(strcmp("-gui", argv[i]) == 0) {
			CMI_args = CMI_args | CMI__ARG_GUI_MODE;
		}
		else {
			fprintf(stderr, "[Client Setup]: ERROR: Invalid or Uncomplete argument. Exit code: %d\n", CMI_ERROR__INVALID_ARG);
			return CMI_ERROR__INVALID_ARG;
		}
	}

	// Opening window(s)
	WM__openDisplay(&CMI_windows);
	if(!WM__useMultithreading) {
		fprintf(stderr, "[Client Setup]: ERROR: X11 multithreading unsupported. Exit code: %d\n", CMI_ERROR__NO_MULTITHREADING);
		return CMI_ERROR__NO_MULTITHREADING;
	}
	if(CMI_args & CMI__ARG_GUI_MODE) {
		// Opening control window
		#ifndef _GUI_SUPPORT
		fprintf(stderr, "[Client Setup]: ERROR: This build doesn't support GUI mode. Exit code: %d\n", CMI_ERROR__NO_GUI_SUPPORT);
		#else
		printf("[Client Setup]: Opening control window...\n");
		CMI_windows.windows = calloc(2, sizeof(WM__Window));
		CMI_windows.windows[0].windowBorderWidth = 5;
		CMI_windows.windows[1].windowBorderWidth = 5;
		CMI_windows.windows[1].windowX = CMI__CTRL_WINDOW_POS_X;
		CMI_windows.windows[1].windowY = CMI__CTRL_WINDOW_POS_Y;
		CMI_windows.windows[1].windowWidth = CMI__CTRL_WINDOW_WIDTH;
		CMI_windows.windows[1].windowHeight = CMI__CTRL_WINDOW_HEIGHT;

		WM__createWindow(
			&CMI_windows, 1,
			WhitePixel(CMI_windows.display, CMI_windows.screen),
			BlackPixel(CMI_windows.display, CMI_windows.screen),
			KeyPressMask | KeyReleaseMask | ButtonPressMask | PointerMotionMask,
			DefaultDepth(CMI_windows.display, CMI_windows.screen),
			DefaultVisual(CMI_windows.display, CMI_windows.screen),
			InputOutput
		);
		#endif
	}
	else {
		printf("[Client Setup]: Running console mode...\n");
		CMI_windows.windows = calloc(1, sizeof(WM__Window));
		CMI_windows.windows[0].windowBorderWidth = 5;
	}

	// Opening 3D viewport
	printf("[Client Setup]: Opening 3D viewport...\n");
	CMI_windows.windows[0].windowX = CMI__ENV_WINDOW_POS_X;
	CMI_windows.windows[0].windowY = CMI__ENV_WINDOW_POS_Y;
	CMI_windows.windows[0].windowWidth = CMI__ENV_WINDOW_WIDTH;
	if(CMI_args & CMI__ARG_GUI_MODE) {
		CMI_windows.windows[0].windowHeight = CMI__ENV_WINDOW_HEIGHT_GUI;
	}
	else {
		CMI_windows.windows[0].windowHeight = CMI__ENV_WINDOW_HEIGHT;
	}

	WM__createWindow(
		&CMI_windows, 0,
		BlackPixel(CMI_windows.display, CMI_windows.screen),
		WhitePixel(CMI_windows.display, CMI_windows.screen),
		NoEventMask, DefaultDepth(CMI_windows.display, CMI_windows.screen),
		DefaultVisual(CMI_windows.display, CMI_windows.screen),
		InputOutput
	);

	WM__createGraphicsContext(&CMI_windows, 0);

	printf("[Client Setup]: Starting 3D viewport... ");
	fflush(stdout);
	pthread_create(&CMI_viewportThreadID, NULL, (void*)viewportThread, NULL);
	if(CMI_args & CMI__ARG_GUI_MODE) {
		printf("Started!\n[Client Setup]: Starting control thread... ");
		fflush(stdout);
		pthread_create(&CMI_controlThreadID, NULL, (void*)controlThread, NULL);
	}
	printf("Started!\n");

	printf("[Client Setup]: Complete!");

	//
	// Main loop
	//
	int exitCode = controlFunction();
	CMI_exitFlag = true;
	pthread_join(CMI_viewportThreadID, NULL);
	if(CMI_args & CMI__ARG_GUI_MODE) {
		pthread_join(CMI_controlThreadID, NULL);
	}

	//
	// Cleanup
	//
	WM__destroyWindow(&CMI_windows, 0);
	#ifdef _GUI_SUPPORT
	if(CMI_args & CMI__ARG_GUI_MODE) {
		WM__destroyWindow(&CMI_windows, 1);
	}
	#endif
	WM__closeDisplay(&CMI_windows);

	printf("[Viewport]: Exit code: %d\n", CMI_viewportRet);
	if(CMI_args & CMI__ARG_GUI_MODE) {
		printf("[GUI Control]: Exit code: %d\n", CMI_controlRet);
	}
	printf("[Controller]: Exit code: %d\n", exitCode);
	return exitCode;
}
