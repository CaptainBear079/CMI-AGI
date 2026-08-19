//
// CMI-AGI v0.0.0
// Chaotic Much Intelligent is a AI project focusing on AGI
//

// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Libraries
#ifdef _USE_X11
#include <X11/Xlib.h> // X11 library (X Window System)
#endif
#include "Fast3D.h" // Fast3D library (Chaos Code Project 3D engine)

// Defines
// - Argument bitmap defines
#define ARG_GUI_MODE 1
#define ARG_RESTORE_SESSION 2

// - Standard amount of nerve endings
#ifndef NERVE_ENDINGS
	#define NERVE_ENDINGS 1000
#endif

// - Standard touch signal buffer size
#ifndef TOUCH_SIGNALS
	#define TOUCH_SIGNALS 100
#endif

// - Standard nerve ending count per signal
#ifndef NERVES
	#define NERVES 3
#endif

// Global variables
// General system variables
short args;
int AIWorkerCount;
pthread_t env_thread_id;
pthread_t control_thread_id;
pthread_t *ai_thread_ids;
bool sim_shutdown = false;
int return_code;
void** ENV_RET = NULL;  // Pointer to the address of ENV_ret
void** CTRL_RET = NULL; // Pointer to the address of CTRL_ret
void*** AI_RET = NULL;  // Pointer to the address of the AI_ret array
int ENV_ret;
int CTRL_ret;
int *AI_ret = NULL;
// - X11
#ifdef _USE_X11
Display *display = NULL;
int screen;
Window root;
#endif
// 3D environment variables
// - X11
uint32_t* framebuffer;
int fb_height;
int fb_width;
#ifdef _USE_X11
GC gc;
XImage *image;
Window render_output_window;
XSetWindowAttributes row_xwa;
#endif
// GUI control variables
// - X11 (GUI support only)
#if defined(_GUI_SUPPORT) && defined(_USE_X11)
Window control_gui_window;
XSetWindowAttributes cguiw_xwa;
#endif
// Touch variables
// ### The touch input is made of two values dictating
// ### where the touch comes from
// ### - distance to the three closest nerve endings
// ### - the three closest nerves
// ### One input per vertex overlapping with the AI's model
Vertex NerveEndings[NERVE_ENDINGS]; // All nerve endings
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

// AI save file reading function
void read_ai_save() {
	return;
}

// AI thread
void* ai_thread(void* arg) {
	while(sim_shutdown == true) {
		// Read input (touch, noise (speech), vision, system messages)
		// Process input
		// - Touch
		while(*Touch_Index != *Touch_FirstFreeIndex) {
			for(int i = 0; i < Touch_NervesPerSignal; i++) {
				Touch[*Touch_Index][i];
			}
		}
	}
	AI_ret = 0;
	return &AI_ret; // Shutdown via exit code signal
}

// 3D environment_thread
void* env_thread(void* arg) {
	while(return_code == 0) {
		render(&framebuffer);
		image = XCreateImage(
			display,
			DefaultVisual(display, screen),
			DefaultDepth(display, screen),
			ZPixmap,
			0,
			(char*)framebuffer,
			fb_width,
			fb_height,
			32,
			0
		);

		XPutImage(
			display,
			render_output_window,
			gc,
			image,
			0, 0,
			0, 0,
			fb_width, fb_height
		);

		XFlush(display);
	}
	ENV_ret = 0;
	return &ENV_ret; // Shutdown via exit code signal
}

// Control thread
void *control_thread(void* arg) {
	while(return_code == 0) {
		#if defined(_USE_X11) && defined(_GUI_SUPPORT)
		XEvent ev;

		while(XNextEvent(display, &ev) == 0) {
			switch(ev.type) {
				case ButtonPress: {
					return 0;
				} break;
			}
		}
	}
	#endif
	CTRL_ret = 0;
	return &CTRL_RET;
}

// Control function
int control_function() {
	// Commands:
	// - Create AI
	// - Import AI
	// - Export AI
	// - Remove AI
	// - Create 3D Model
	// - Import 3D Model
	// - Export 3D Model
	// - Remove 3D Model
	// - Start Simulation
	// - Kill Simulation

	// Variables
	int command = 0;
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
			ai_thread_ids = malloc(AIWorkerCount * sizeof(pthread_t));
			for(int i = 0; i < AIWorkerCount; i++) {
				pthread_create(&ai_thread_ids[i], NULL, (void*)ai_thread, NULL);
			}
		} break;

		// Kill Simulation
		case 10: {
			AI_ret = malloc(AIWorkerCount * sizeof(int));
			sim_shutdown = true;
			for(int i = 0; i < AIWorkerCount; i++) {
				pthread_join(ai_thread_ids[i], AI_RET[i]);
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
	return -1; // Unexpected error
}

// Control thread
int main(int argc, char* argv[]) {
	//
	// Security check
	//
	if(TOUCH_SIGNALS < 1 || NERVES < 3) {
		fprintf(stderr, "Compilation error: Invalid defines. Error Code: 0x0002\n");
		return 2;
	}

	//
	// Setup
	//
	printf("[Setup]: Interpreting arguments...\n");
	// - Interpreting arguments
	for(int i = 1; i < argc; i++) {
		if(strcmp("-gui", argv[i]) == 0) {
			args = args | ARG_GUI_MODE;
		}
		else if(strcmp("--restore", argv[i]) == 0) {
			args = args | ARG_RESTORE_SESSION;
			break;
		}
	}

	printf("[Setup]: Preparing window(s)...\n");
	// - Prepare windows (control window GUI mode only)
	//   - Setup X11
	#ifdef _USE_X11
	if((display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "X11: Can't open display. Error Code: 0x0001\n");
		return 1;
	}

	screen = XDefaultScreen(display);
	root = RootWindow(display, screen);
	row_xwa.background_pixel = BlackPixel(display, screen); // Black background
	row_xwa.border_pixel = WhitePixel(display, screen);     // White border
	row_xwa.event_mask = NoEventMask;                       // No events

	render_output_window = XCreateWindow(
		display,
		root,
		0, 0,
		1920, 1080,
		5,
		DefaultDepth(display, screen),
		InputOutput,
		DefaultVisual(display, screen),
		CWBackPixel | CWBorderPixel | CWEventMask,
		&row_xwa
	);
	XMapWindow(display, render_output_window);
	XFlush(display);

	gc = XCreateGC(display, render_output_window, 0, NULL);
	XSetFunction(display, gc, GXcopy);
	#endif

	#if defined(_GUI_SUPPORT) && defined(_USE_X11)
	cguiw_xwa.background_pixel = WhitePixel(display, screen);                                   // White background
	cguiw_xwa.border_pixel = BlackPixel(display, screen);                                       // Black border
	cguiw_xwa.event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask | PointerMotionMask; // Mouse and Keyboard events

	control_gui_window = XCreateWindow(
		display,
		root,
		0, 0,
		1920, 1080,
		5,
		DefaultDepth(display, screen),
		InputOutput,
		DefaultVisual(display, screen),
		CWBackPixel | CWBorderPixel | CWEventMask,
		&cguiw_xwa
	);
	XMapWindow(display, control_gui_window);
	XFlush(display);
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
	pthread_join(control_thread_id, CTRL_RET);
	pthread_join(env_thread_id, ENV_RET);

	//
	// Cleanup
	//
	#ifdef _USE_X11
	XUnmapWindow(display, render_output_window);
	XDestroyWindow(display, render_output_window);
	#endif

	#if defined(_GUI_SUPPORT) && defined(_USE_X11)
	XUnmapWindow(display, control_gui_window);
	XDestroyWindow(display, control_gui_window);
	#endif

	#ifdef _USE_X11
	XCloseDisplay(display);
	#endif

	if(ENV_ret != 0) {
		printf("[Environment]: Exit code: %d\n", ENV_ret);
	}
	if(CTRL_ret != 0) {
		printf("[Control Thread]: Exit code: %d\n", CTRL_ret);
	}
	for(int i = 0; i < AIWorkerCount; i++) {
		if(AI_ret != 0) {
			printf("[AI:%d]: Exit code: %d\n", i, AI_ret[i]);
		}
	}
	printf("[Controller]: Exit code: %d\n", return_code);
	return return_code;
}
