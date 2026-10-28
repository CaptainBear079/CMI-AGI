#pragma once

// Standard libraries
#include <pthread.h>

// Local includes
#include "../shared.h" // Shared definitions and types

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "SaveFileInterpreter/saveInterpreter.h"
#include "EntityManager/EntityManager.h"

// Global variables
extern short args;       // Argument bitmap
extern Save save;        // Save file data
extern EM entityManager; // Entity handle
