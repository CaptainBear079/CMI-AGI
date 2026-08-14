//
// CMI-AGI v0.0.0
// Chaotic Much Intelligent is a AI project focusing on AGI
//

// Includes
#include <stdio.h>

// Global variables
// - Basic Touch Variables (extended version uses malloc incase the list of nerves looks like this "0.1, 0.32, 0.64, 0.64" so that all third places can be processed)
double Touch[100][3];         // Touch (up to 100 signals)
int Touch_Index = 0;          // Shows the current index
int Touch_FirstFreeIndex = 0; // Shows the first free position for the signals
// - Noise Variables
double Noise[2];              // Two noise signals (raw audio, two ears)
// - Vision Variables
uint32_t Vision[100][100];    // Vision (100x100 pixel video feed)
// - System messages variables
char* msg[10][256];               // System messages (up to 255 characters (plus NULL), 10 messages)

// AI thread
void ai_thread() {
	// Read input (touch, noise (speech), vision, system messages)
	// Process input
	// - Touch
	// # The touch input is made of one value dictating where the touch comes from (distance to the three closest nerve endings)
	// # One input per vertex overlapping with the AI's model
	double **tTouch;
	return;
}

// Control thread
int main() {
	// Setup
	// - Prepare threads (AI, 3D environment, control console/gui)
	//   - Load AI model
	//   - Load 3D environment
	//   - Load 3D AI model
	//   - Load GUI assets (GUI mode only)
	// Main loop
	// Cleanup
	return 0;
}