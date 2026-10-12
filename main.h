// Includes
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/select.h>

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
//   - Environment
#define CMI__ENV_WINDOW_WIDTH 1920
#define CMI__ENV_WINDOW_HEIGHT 1080
//#define CMI__ENV_WINDOW_HEIGHT_GUI 720
#define CMI__ENV_WINDOW_HEIGHT_GUI 648
#define CMI__ENV_WINDOW_POS_X 0
#define CMI__ENV_WINDOW_POS_Y 0
//   - Control
#define CMI__CTRL_WINDOW_WIDTH 1920
//#define CMI__CTRL_WINDOW_HEIGHT 360
#define CMI__CTRL_WINDOW_HEIGHT 325
//#define CMI__CTRL_WINDOW_POS_X 720
#define CMI__CTRL_WINDOW_POS_X 755
#define CMI__CTRL_WINDOW_POS_Y 0

// - Control function commands
#define CMI__CTRL_FUNC__NO_COMMAND 0
#define CMI__CTRL_FUNC__CREATE_AI 1
#define CMI__CTRL_FUNC__IMPORT_AI 2
#define CMI__CTRL_FUNC__EXPORT_AI 3
#define CMI__CTRL_FUNC__REMOVE_AI 4
#define CMI__CTRL_FUNC__CREATE_3D 5
#define CMI__CTRL_FUNC__IMPORT_3D 6
#define CMI__CTRL_FUNC__EXPORT_3D 7
#define CMI__CTRL_FUNC__REMOVE_3D 8
#define CMI__CTRL_FUNC__START_SIMULATION 9
#define CMI__CTRL_FUNC__KILL_SIMULATION 10
#define CMI__CTRL_FUNC__QUIT 11
