#pragma once

// Standard libraries
#include <pthread.h>

// Local includes
#include "shared.h" // Shared definitions and types

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "WindowManager/WindowManager.h"
#include "Viewport/viewport.h"

// Global variables
short args = 0;             // Argument bitmap
int commands[6];            // Command queue
void** commandsArgs[6];     // Argument lists for commands
WM windows;                 // The window handler
pthread_t viewportThreadID; // pthread_t for the environment thread
pthread_t controlThreadID;  // pthread_t for the control thread
int viewportRet = 0;        // Return code of the environment thread
int controlRet = 0;         // Return code of the control thread
bool exitFlag = false;      // Shutdown signal to viewportThread and controlThread
