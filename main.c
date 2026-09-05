// Includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

// Libraries
#include "WindowManager/WindowManager.h" // Window Manager
#ifdef _W_X11
#include <X11/Xlib.h>                    // X11 library (X Window System)
#endif

#include <Fast3D/Fast3D.h> // Fast3D library (Chaos Code Project 3D engine)
#include "cube.h"          // TEMP: Cube model (for testing and later use as template)

// Defines
// - Argument bitmap
#define ARG_RESTORE_SESSION 1
#define ARG_GUI_MODE 2
#define ARG_SCRIPT 4

// Global variables
// - General system variables
int frameRate = 60;          // Frame rate (FPS)
short args;                  // Argument bitmap
FILE* script_fptr;           // Script file handle
pthread_t env_thread_id;     // pthread_t for the environment thread
pthread_t control_thread_id; // pthread_t for the control thread
pthread_t *ai_thread_ids;    // Pointer to a dynamic array of pthread_t for AI threads
int AIThreadCount;           // Number of AI threads
int CTRL_ret;                // Return code of the control thread
int ENV_ret;                 // Return code of the environment thread
int* AI_ret = NULL;          // Pointer to a dynamic array of return codes for AI threads
bool sim_shutdown = false;   // Shutdown signal for AI threads
int return_code = 0;         // Shutdown signal to env_thread and control_thread

// Window Manager
WM__Window row;
#ifdef _GUI_SUPPORT
WM__Window cgui;
#endif

// - Window data
int env_window_width = 1920;
int env_window_height = 1080;

// Touch variables
// ### The touch input is made of two values dictating
// ### where the touch comes from
// ### - distance to the three closest nerve endings
// ### - the three closest nerves
// ### One input per vertex overlapping with the AI's model
Vertex *NerveEndings = NULL;        // All nerve endings
int Touch_NervesPerSignal = 0;      // Number of nerves per signal
double ***Touch = NULL;             // Pointer to the right touch signal buffer
int ***TouchNerves = NULL;          // Pointer to the right touch signal buffer (Nerve index)
int *Touch_Index = NULL;            // Pointer to the right touch signal buffer index
int *Touch_FirstFreeIndex = NULL;   // Pointer to the right first free index of the touch signal buffer
// - Basic Touch Variables
double **BTouch = NULL;             // Touch signal buffer
int **BTouchNerves = NULL;          // Touch signal buffer (Nerve index)
int BTouch_Index = 0;               // Shows the current index of the signal buffer
int BTouch_FirstFreeIndex = 0;      // Shows the first free position in the signal buffer
// - Extended Touch Variables
double **EXTouch = NULL;            // Extended Touch signal buffer (using malloc)
int **EXTouchNerves = NULL;         // Extended Touch signal buffer (Nerve index, using malloc)
int EXTouch_Index = 0;              // Shows the current index of the signal buffer
int EXTouch_FirstFreeIndex = 0;     // Shows the first free position in the signal buffer
// Noise variables
// - Noise Variables
double Noise[2];                    // Two noise signals (raw audio, two ears)
// Vision variables
// - Vision Variables
uint32_t Vision[1080][1920];        // Vision (1080x1920 pixel video feed)
// System variables
// - System messages variables
char* msg[10][256];                 // System messages (up to 255 characters (plus NULL), 10 messages)

// AI thread
void* ai_thread(void* arg) {
	int* ret_code = malloc(sizeof(int));
	while(sim_shutdown == false) {
		// Read input (touch, noise (speech), vision, system messages)
		// Process input
		// - Touch
	}
	*ret_code = 0;
	return ret_code; // Shutdown via exit code signal
}

// 3D environment_thread
void* env_thread(void* arg) {
	// Frame limiting variables
	time_t t1;
	time_t t2;
	init_cube();
	Renderer* renderer = Fast3D__init(env_window_width, env_window_height, 90.0);
	Fast3D__addMesh(renderer, &cube);
	WM__createImage(row, renderer->fb);
	while(return_code == 0) {
		t1 = clock();
		Fast3D__render(renderer);

		WM__updateImage(row);

		WM__updateWindow(row);

		t2 = clock();
		// Wait long enough to limit the frame rate to frameRate FPS
		while((t2 - t1) < (CLOCKS_PER_SEC / frameRate)) {
			t2 = clock();
		}
	}
	Fast3D__destroy(renderer);
	ENV_ret = 0;
	return NULL; // Shutdown via exit code signal
}

// Control thread
void *control_thread(void* arg) {
	while(return_code == 0) {
		#ifdef _W_X11
		XEvent ev;

		while(XNextEvent(row.display, &ev) == 0) {
			switch(ev.type) {
				case ButtonPress: {
					CTRL_ret = 0;
					return NULL;
				} break;
			}
		}
		#endif
		usleep(1000);
	}
	CTRL_ret = 0;
	return NULL;
}

// Control function
int control_function() {
	// Variables
	int command = 0;
	while(true) {
		usleep(10000);
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
			case 9: {
				ai_thread_ids = malloc(AIThreadCount * sizeof(pthread_t));
				for(int i = 0; i < AIThreadCount; i++) {
					pthread_create(&ai_thread_ids[i], NULL, (void*)ai_thread, NULL);
				}
			} break;

			// Kill Simulation
			case 10: {
				AI_ret = malloc(AIThreadCount * sizeof(int));
				sim_shutdown = true;
				for(int i = 0; i < AIThreadCount; i++) {
					pthread_join(ai_thread_ids[i], (void**)&(AI_ret[i]));
				}
			} break;

			// Quit
			case 11: {
				return 0;
			} break;
			default: {
				printf("Invalid command.\n");
			} break;
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
			args = args | ARG_RESTORE_SESSION;
			break;
		}
		else if(strcmp("-s", argv[i]) == 0) {
			args = args | ARG_SCRIPT;
			script_fptr = fopen(argv[i + 1], "r");
			if(script_fptr == NULL) {
				fprintf(stderr, "[Setup]: \"-s\": Script couldn't be opened. Error Code: 0x0001\n");
				return 1;
			}
		}
		else if(strcmp("-gui", argv[i]) == 0) {
			args = args | ARG_GUI_MODE;
		}
	}

	printf("[Setup]: Preparing window(s)...\n");
	// - Prepare windows (control window GUI mode only)
	//   - Setup X11
	#ifdef _W_X11
	if(!WM__useMultithreading) {
		return -1;
	}

	WM__getInfo(&row);
	row.windowWidth = env_window_width;
	row.windowHeight = env_window_height;
	
	WM__createWindow(
		&row,
		BlackPixel(row.display, row.screen),
		WhitePixel(row.display, row.screen),
		NoEventMask, DefaultDepth(row.display, row.screen),
		DefaultVisual(row.display, row.screen),
		InputOutput
	);

	WM__createGraphicContext(&row);

	#ifdef _GUI_SUPPORT
	WM__getInfo(&cgui);
	WM__createWindow(
		cgui,
		WhitePixel(cgui.display, cgui.screen),
		BlackPixel(cgui.display, cgui.screen),
		KeyPressMask | KeyReleaseMask | ButtonPressMask | PointerMotionMask,
		DefaultDepth(cgui.display, cgui.screen),
		DefaultVisual(cgui.display, cgui.screen),
		InputOutput
	);
	#endif
	#endif

	printf("[Setup]: Starting 3D environment...\n");
	pthread_create(&env_thread_id, NULL, (void*)env_thread, NULL);

	printf("[Setup]: Starting control thread...\n");
	pthread_create(&control_thread_id, NULL, (void*)control_thread, NULL);

	//
	// Main loop
	//
	printf("Setup complete.\n");
	return_code = control_function();
	pthread_join(control_thread_id, NULL);
	pthread_join(env_thread_id, NULL);

	//
	// Cleanup
	//
	#ifdef _W_X11
	WM__destroyWindow(row);
	WM__closeDisplay(row);

	#ifdef _GUI_SUPPORT
	WM__destroyWindow(cgui);
	WM__closeDisplay(cgui);
	#endif
	#endif

	if(ENV_ret != 0) {
		printf("[Environment]: Exit code: %d\n", ENV_ret);
	}
	if(CTRL_ret != 0) {
		printf("[Control Thread]: Exit code: %d\n", CTRL_ret);
	}
	for(int i = 0; i < AIThreadCount; i++) {
		if(AI_ret != 0) {
			printf("[AI:%d]: Exit code: %d\n", i, AI_ret[i]);
		}
	}
	printf("[Controller]: Exit code: %d\n", return_code);
	return return_code;
}
