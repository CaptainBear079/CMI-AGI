#pragma once
#include <Fast3D/Fast3D.h>

#define EM_EVENT_EMPTY 0
#define EM_EVENT_SHUTDOWN 1

typedef struct _EM_Event_ {
	int event;
} EM_Event;

typedef struct _Neuron_ {
} Neuron;

// The touch signal is made of
// - The indexes of the three closest nerves
// - The distance of the OBJ to the saved nerves
// One signal per vertex overlapping with the entities model
// Basic Touch Signal
typedef struct _EM_Basic_Touch_Signal_ {
	unsigned long long nerves[3];
	double distance[3];
} EM_Basic_Touch_Signal;
// Extended Touch Signal
typedef struct _EM_Ex_Touch_Signal_ {
	unsigned long long* nerves;
	double* distance;
	int nervesPerSignal;
} EM_Ex_Touch_Signal;

// Vision
typedef struct _EM_Vision_ {
	uint32_t* FullVision;
	uint32_t* eyes;
} EM_Vision;

typedef struct _EM_Entity_ {
	char* name;
	unsigned int id;
	//
	// Touch variables
	Vertex *NerveEndings;                          // All nerve endings
	int TSBIndex;                                  // Current index of the touch signal buffer
	int TSBFirstFreeIndex;                         // First free/unused touch signal buffer index
	union {
	// - Basic Touch Variables
	EM_Basic_Touch_Signal* BasicTouchSignalBuffer; // Basic touch signal buffer
	// - Extended Touch Variables
	EM_Ex_Touch_Signal* ExTouchSignalBuffer;       // Extended touch signal buffer
	} TouchSignalBuffer;
	// Noise variables
	// Vision variables
	EM_Vision vision;                              // Vision
	// Telepathic communication
	int channel;                                   // Which telepathic text output is used
	char msg[1025];                                // Telepathic text output
} EM_Entity;

typedef struct _EM_ {
	EM_Entity* entities;
	int entityCount;
	EM_Event* events;
	int eventIndex;
} EM;

void EM__createAIThread(EM* handler);
void EM__startSimulation(EM* handler);
