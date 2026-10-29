#pragma once

// Standard libraries
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>

// Local includes
#include "../shared.h" // Shared definitions and types

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "SaveFileInterpreter/saveInterpreter.h"
#include "EntityManager/EntityManager.h"

// Global variables
extern short args;                 // Argument bitmap
extern Save save;                  // Save file data
extern int commands[6];            // Command queue
extern void** commandsArgs[6];     // Argument lists for commands
extern EM entityManager;           // Entity handle
extern bool noNextCommand = false; // Skip the printing of "\n$ "
extern bool exitFlag = false;      // Shutdown flag
