#pragma once

// Standard libraries
#include <pthread.h>

// Local includes
#include "shared.h" // Shared definitions and types

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "SaveFileInterpreter/saveInterpreter.h"
#include "EntityManager/EntityManager.h"

// Global variables
short args = 0;   // Argument bitmap
Save save;        // Save file data
EM entityManager; // Entity handle
