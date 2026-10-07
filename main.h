// Includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

// Libraries
#include <Fast3D/Fast3D.h> // Fast3D library (Chaos Code Project 3D engine)
#include "Fast3D/cube.h"          // TEMP: Cube model (for testing and later use as template)

// Modules
#include "SaveFileInterpreter/saveInterpreter.h" // Save File Interpreter
#include "WindowManager/WindowManager.h"         // Window Manager
#include "EntityManager/EntityManager.h"         // Entity Manager

// Defines
// - Argument bitmap
#define CMI__ARG_RESTORE_SESSION 1
#define CMI__ARG_GUI_MODE 2
#define CMI__ARG_SCRIPT 4
// - Window standard values
#define CMI__WINDOW_WIDTH 1920
#define CMI__WINDOW_HEIGHT 1080
