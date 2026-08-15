//
// CMI-AGI v0.0.0
// Chaotic Much Intelligent is a AI project focusing on AGI
//

// Includes
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

// Libraries
#ifdef _USE_X11
#include <X11/Xlib.h> // X11 library (X Window System)
#endif
#include "Fast3D.h" // Fast3D library (Chaos Code Project 3D engine)

// Defines
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
#ifdef _USE_X11
Display *display;
int screen;
Window root;
#endif
// 3D environment variables
#ifdef _USE_X11
Window render_output_window;
XSetWindowAttributes row_xwa;
#endif
// GUI control variables
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
double ***Touch = NULL;             // Pointer to the right touch signal buffer
int ***TouchNerves = NULL;          // Pointer to the right touch signal buffer (Nerve index)
int *Touch_Index = NULL;            // Pointer to the right touch signal buffer index
int *Touch_FirstFreeIndex = NULL;   // Pointer to the right first free index of the touch signal buffer
// - Basic Touch Variables
double **BTouch;                    // Touch signal buffer
int **BTouchNerves;                 // Touch signal buffer (Nerve index)
int BTouch_Index = 0;               // Shows the current index of the signal buffer
int BTouch_FirstFreeIndex = 0;      // Shows the first free position in the signal buffer
// - Extended Touch Variables
double **EXTouch;                   // Extended Touch signal buffer (using malloc)
int **EXTouchNerves;                // Extended Touch signal buffer (Nerve index, using malloc)
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
void ai_thread(void* arg) {
	// Read input (touch, noise (speech), vision, system messages)
	// Process input
	// - Touch
	while(*Touch_Index != *Touch_FirstFreeIndex) {
		for(int i = 0; i < NERVES; i++) {
			Touch[*Touch_Index][i];
		}
	}
	return;
}

// 3D environment_thread
void env_thread(void* arg) {
	return;
}

// Control thread
int main() {
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
	// - Prepare threads (AI, 3D environment, control console/gui)
	//   - Load AI model
	//     - Read save file
	//     - Set variables
	Touch = &BTouch;
	Touch_Index = &BTouch_Index;
	Touch_FirstFreeIndex = &BTouch_FirstFreeIndex;
	//   - Load 3D environment
	//   - Load 3D AI model
	//   - Load GUI assets (GUI mode only)
	//   - Prepare windows (control window GUI mode only)
	//     - Setup X11
	#ifdef _USE_X11
	if((display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "X11: Can't open ´display. Error Code: 0x0001\n");
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
	pthread_t env_thread_id;
	pthread_t ai_thread_id;
	pthread_create(&env_thread_id, NULL, (void*)env_thread, NULL);
	pthread_create(&ai_thread_id, NULL, (void*)ai_thread, NULL);

	//
	// Main loop
	//
	XEvent ev;

	while(XNextEvent(display, &ev) == 0) {
		switch(ev.type) {
			case ButtonPress: {
				break;
			}
		}
	}

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
	return 0;
}
