#include "main.h"

// Global variables
// - General system variables
short args = 0;                         // Argument bitmap
Save save;                              // Save file data
int commands[6];                        // Command queue
void** commandsArgs[7];                 // Argument lists for commands
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
	Token tokens[8];
	int i = 0;
	int bi = 0;
	int t = 0;
	char buf[257];
	bool str = false;
	buf[256] = '\0';
	while(t < 8) {
		if(str) {
			if(bi != 0 && (buffer[i] == '\n' || buffer[i] == '\0')) {
				buf[bi] = '\0';
				tokens[t].text = malloc((i + 1) * sizeof(char));
				strcpy(tokens[t].text, buf);
				t++;
				break;
			}
			else if(buffer[i] == '\"') {
				str = false;
			}
			else {
				buf[bi] = buffer[i];
				bi++;
			}
		}
		else {
			if(bi != 0 && (buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\0')) {
				buf[bi] = '\0';
				tokens[t].text = malloc((i + 1) * sizeof(char));
				strcpy(tokens[t].text, buf);
				if(!(buffer[i] == ' ')) {
					break;
				}
				t++;
				bi = 0;
			}
			else if(buffer[i] == '\"') {
				str = true;
			}
			else {
				buf[bi] = buffer[i];
				bi++;
			}
		}
		i++;
	}

	if(strcmp("create", tokens[0].text) == 0) {
		if(strcmp("ai", tokens[1].text) == 0) {
			commandsArgs[0] = malloc(6 * sizeof(void**));
			commandsArgs[0][0] = malloc((strlen(tokens[2].text) + 1) * sizeof(char));
			strcpy((char*)commandsArgs[0][0], tokens[2].text);

			commandsArgs[0][1] = malloc(sizeof(unsigned int));
			if(strcmp("cplugin", tokens[3].text) == 0) {
				(*(unsigned int*)commandsArgs[0][1]) = EM_ENTITY__TYPE_CPLUGIN;
			}
			else if(strcmp("engine", tokens[3].text) == 0) {
				(*(unsigned int*)commandsArgs[0][1]) = EM_ENTITY__TYPE_ENTITY_ENGINE;
			}
			else {
				fprintf(stderr, "[Controller] ERROR: Invalid command.\n");
				return 0;
			}

			commandsArgs[0][2] = malloc((strlen(tokens[4].text) + 1) * sizeof(char));
			strcpy((char*)commandsArgs[0][2], tokens[4].text);

			commandsArgs[0][3] = malloc(sizeof(unsigned long long));
			(*(unsigned long long*)commandsArgs[0][3]) = atoll(tokens[5].text);

			commandsArgs[0][4] = malloc(sizeof(unsigned long long));
			(*(unsigned long long*)commandsArgs[0][4]) = atoll(tokens[6].text);

			commandsArgs[0][5] = malloc(sizeof(unsigned long long));
			(*(unsigned long long*)commandsArgs[0][5]) = atoll(tokens[7].text);

			int stringlen = strlen(tokens[4].text) + 6;
			char* string = malloc(stringlen * sizeof(char));
			strcpy(string, tokens[4].text);
			string[stringlen - 6] = '.';
			string[stringlen - 5] = 'i';
			string[stringlen - 4] = 'n';
			string[stringlen - 3] = 'f';
			string[stringlen - 2] = 'o';
			FILE* fptr = fopen(string, "r");
			int size = 0;
			char infoBuffer[257];
			infoBuffer[256] = '\0';
			bool reachedEOF = false;
			char c;
			int infoIndex = 0;
			while(!reachedEOF) {
				for(int ci = 0; ci < 18; ci++) {
					infoBuffer[ci] = fgetc(fptr);
					if(infoBuffer[ci] == '/') {
						infoBuffer[ci] = '\0';
						break;
					}
					else if(infoBuffer[ci] == EOF) {
						reachedEOF = true;
					}
				}
				if(strcmp(infoBuffer, "*") == 0) {
					size += sizeof(void*);
				}
				else if(strcmp(infoBuffer, "int") == 0) {
					size += sizeof(int);
				}
				else {
					fprintf(stderr, "[Controller] ERROR: Invalid or unsupported type in \"%s\".", string);
					return CMI__CTRL_FUNC__NO_COMMAND;
				}
				while(true) {
					c = fgetc(fptr);
					if(c == '#' || c == '\n') {
						break;
					}
					else if(c == EOF) {
						reachedEOF = true;
					}
				}
			}
			commandsArgs[0][6] = malloc(size);
			rewind(fptr);
			while(true) {
				c = fgetc(fptr);
				if(c == ':') {
					break;
				}
				else if(c == EOF) {
					reachedEOF = true;
				}
			}
			for(int ci = 0; ci < 5; ci++) {
				infoBuffer[ci] = fgetc(fptr);
				if(infoBuffer[ci] == '\n' || infoBuffer[ci] == '#') {
					infoBuffer[ci] = '\0';
				}
				else if(infoBuffer[ci] == EOF) {
					reachedEOF = true;
				}
			}
			if(strcmp(infoBuffer, "NULL") == 0) {
				void** temp = &(*commandsArgs[0][6]) + infoIndex;
				*temp = NULL;
				infoIndex += sizeof(void*);
			}
			else if(strcmp(infoBuffer, "stdin") == 0) {
				fgets(infoBuffer, 256, stdin);
				int* temp = &(*commandsArgs[0][6]) + infoIndex;
				*temp = atoi(infoBuffer);
			}

			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return CMI__CTRL_FUNC__CREATE_AI;
		}
		else if(strcmp("3d", tokens[1].text) == 0) {
			for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
				free(tokens[i2].text);
			}
			return CMI__CTRL_FUNC__CREATE_3D;
		}
	}
	else if(strcmp("start", tokens[0].text) == 0) {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return CMI__CTRL_FUNC__START_SIMULATION;
	}
	else if(strcmp("kill", tokens[0].text) == 0) {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return CMI__CTRL_FUNC__KILL_SIMULATION;
	}
	else if(strcmp("quit", tokens[0].text) == 0) {
		for(int i2 = 0; i2 < 8 && i2 < t; i2++) {
			free(tokens[i2].text);
		}
		return CMI__CTRL_FUNC__QUIT;
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

			// Interpret command
			commands[0] = CTRL_FUNC__interpretCommand(buffer);
		}

		// Check for and run commands (both stdin and control_thread)
		for(int i = 0; i < 6; i++) {
			noNextCommand = false;
			switch(commands[i]){
				// No command
				case CMI__CTRL_FUNC__NO_COMMAND: {
					noNextCommand = true;
				} break;

				// Create AI
				case CMI__CTRL_FUNC__CREATE_AI: {
					EM__createEntity(
						&entityManager, entityManager.entityCount,
						(char*)commandsArgs[i][0], *((unsigned int*)commandsArgs[i][1]), (char*)commandsArgs[i][2],
						*((unsigned long long*)commandsArgs[i][3]), *((unsigned long long*)commandsArgs[i][4]), *((unsigned long long*)commandsArgs[i][5])
					);
					EM__setDataSection(&entityManager, entityManager.entityCount - 1, commandsArgs[i][6]);
					for(int i2 = 0; i2 < 8; i2++) {
						free(commandsArgs[i][i2]);
					}
				} break;

				// Import AI
				case CMI__CTRL_FUNC__IMPORT_AI: {} break;

				// Export AI
				case CMI__CTRL_FUNC__EXPORT_AI: {} break;

				// Remove AI
				case CMI__CTRL_FUNC__REMOVE_AI: {} break;

				// Create 3D Model
				case CMI__CTRL_FUNC__CREATE_3D: {} break;

				// Import 3D Model
				case CMI__CTRL_FUNC__IMPORT_3D: {} break;

				// Export 3D Model
				case CMI__CTRL_FUNC__EXPORT_3D: {} break;

				// Remove 3D Model
				case CMI__CTRL_FUNC__REMOVE_3D: {} break;

				// Start Simulation
				case CMI__CTRL_FUNC__START_SIMULATION: {
					printf("[Controller] Starting simulation...");
					fflush(stdout);
					EM__startSimulation(&entityManager);
					printf(" Success!\n");
				} break;

				// Kill Simulation
				case CMI__CTRL_FUNC__KILL_SIMULATION: {
					printf("[Controller] Ending simulation...");
					fflush(stdout);
					EM__quitSimulation(&entityManager);
					printf(" Success!\n");
				} break;

				// Quit
				case CMI__CTRL_FUNC__QUIT: {
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
