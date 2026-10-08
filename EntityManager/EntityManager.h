#pragma once
// Includes
#include <stdlib.h>
#include <pthread.h>
#include <Fast3D/Fast3D.h>


// Defines
#define EM_ENTITY__TYPE_CPLUGIN 0
#define EM_ENTITY__TYPE_ENTITY_ENGINE 1
/*#define EM_EVENT_EMPTY 0
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
} EM_Vision;*/

typedef struct _EM_Entity_ {
	pthread_t* thread;
	char* name;        // Entity name
	unsigned int id;   // Unique ID for the Entity
	unsigned int type; // Entity AI type (C plugin/Entity Engine)
	char* path;        // The path to the C Plugin/Entity Engine script
	void* input;       // Input data (dynamically defined at runtime)
	int inputCount;    // Amount of inputs
	void* output;      // Output data (dynamically defined at runtime)
	int outputCount;   // Amount of outputs
	void* data;        // Save data
} EM_Entity;

typedef struct _EM_ {
	unsigned long long tick;     // Primary clock
	unsigned long long tickYear; // Secondary clock
	EM_Entity** entities;        // Entities (AIs)
	int entityCount;             // Entity count
	/*EM_Event* events;
	int eventIndex;*/
	bool simActive;              // Simulation state flag
	int** entityRetCodes;        // Entity return codes
	bool printExitCodes;         // Exit code printing flag
} EM;

#include "../plugin.h"

void EM__init(EM* handler, bool printExitCodes);
void EM__createAIThread(EM* handler, unsigned int id, char* name, unsigned int type, char* path);
void EM__startSimulation(EM* handler);
void EM__quitSimulation(EM* handler);
