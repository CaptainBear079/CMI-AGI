#pragma once

// Local includes
#include "../client.h"

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "../WindowManager/WindowManager.h"

// Structures
// CMI__VP_EVENT__ADD_MODEL Payload structure
typedef struct _VP__EVENT_PAYLOAD__ADD_MODEL_ {
    unsigned long long id; // Model id
    unsigned short type;   // Model type (Asset or Unique)
    union _VP_EP_AM__Model_ {
        unsigned long long id; // Asset id
        Mesh* mesh;            // Mesh
    } VP_EP_AM__Model;
} VP__EVENT_PAYLOAD__ADD_MODEL;

// Viewport event structure
typedef struct _VP__EVENT_ {
    unsigned short type; // Event type
    union _VP__EVENT__PAYLOAD_ {
        VP__EVENT_PAYLOAD__ADD_MODEL* addModelPAYLOAD; // Event payload for the CMI__VP_EVENT__ADD_MODEL
    } VP__EVENT__PAYLOAD;
} VP__EVENT;

// Global Variables
extern VP__EVENT* eventQueue; // Queue for events from the control function

// Viewport thread
void* viewportThread(void* arg);
