#pragma once

// Standard libraries
#include <pthread.h>

// Local includes
#include "../shared.h" // Shared definitions and types

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "WindowManager/WindowManager.h"
#include "Viewport/viewport.h"

// Global variables
extern short args;                 // Argument bitmap
extern int commands[6];            // Command queue
extern void** commandsArgs[6];     // Argument lists for commands
extern WM windows;                 // The window handler
extern pthread_t viewportThreadID; // pthread_t for the environment thread
extern pthread_t controlThreadID;  // pthread_t for the control thread
extern int viewportRet;            // Return code of the environment thread
extern int controlRet;             // Return code of the control thread
extern bool exitFlag;              // Shutdown signal to viewportThread and controlThread
