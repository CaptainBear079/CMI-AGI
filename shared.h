#pragma once

// Standard libraries
#include <stdbool.h>
#include <pthread.h>

// Library header includes
#include <Fast3D.h> // Fast3D render engine

// Modules
#include "WindowManager/WindowManager.h"

// Global Structures
// CMI__VP_EVENT__ADD_MODEL Payload structure
typedef struct _CMI_VP__EVENT_PAYLOAD__ADD_MODEL_ {
    unsigned long long id; // Model id
    bool type;             // Model type (Asset or Unique)
    union _CMI_VP_EP_AM__Model_ {
        unsigned long long id; // Asset id
        Mesh* mesh;            // Mesh
    } CMI_VP_EP_AM__Model;
} CMI_VP__EVENT_PAYLOAD__ADD_MODEL;

// Viewport event structure
typedef struct _CMI_VP__EVENT_ {
    unsigned short type; // Event type
    union _CMI_VP__EVENT__PAYLOAD_ {
        CMI_VP__EVENT_PAYLOAD__ADD_MODEL* addModelPAYLOAD; // Event payload for the CMI__VP_EVENT__ADD_MODEL
    } CMI_VP__EVENT__PAYLOAD;
} CMI_VP__EVENT;

typedef struct _Object_ {} Object;

typedef struct _Dimension_ {
    Object* objects;
} Dimension;

typedef struct _World_ {
    Dimension* dimensions;
} World;

typedef struct _Server_ {
    World serverWorld; // Server side world data
} Server;

typedef struct _Client_ {
    // Setup
    short args;                 // Argument bitmap

    // Queues
    int commands[6];            // Command queue
    void** commandsArgs[6];     // Argument lists for commands
    CMI_VP__EVENT* eventQueue;  // Viewport event queue

    // Data
    World world;                // Local world data (synced via updates from the server)
    WM windows;                 // The window handler

    // Threads
    pthread_t viewportThreadID; // pthread_t for the environment thread
    pthread_t controlThreadID;  // pthread_t for the control thread
    int viewportRet;            // Return code of the environment thread
    int controlRet;             // Return code of the control thread
    bool exitFlag;              // Shutdown signal to viewportThread and controlThread

    // Indexes
    unsigned long long d;       // Dimension index for the renderer
} Client;

// Defines
// - Variable defines
#define CMI_client client
#define CMI_args CMI_client->args
#define CMI_commands CMI_client->commands
#define CMI_world CMI_client->world
#define CMI_dimensions CMI_world.dimensions
#define CMI_windows CMI_client->windows
#define CMI_viewportThreadID CMI_client->viewportThreadID
#define CMI_controlThreadID CMI_client->controlThreadID
#define CMI_viewportRet CMI_client->viewportRet
#define CMI_controlRet CMI_client->controlRet
#define CMI_exitFlag CMI_client->exitFlag
#define CMI_VP_eventQueue CMI_client->eventQueue
#define CMI_VP_d CMI_client->d
#define CMI_VP_E__addModelPAYLOAD(i) CMI_VP_eventQueue[i].CMI_VP__EVENT__PAYLOAD.addModelPAYLOAD

// - Error exit codes
#define CMI_ERROR__INVALID_ARG -1
#define CMI_ERROR__NO_MULTITHREADING -2
#define CMI_ERROR__NO_GUI_SUPPORT -3
#define CMI_ERROR__INVALID_COMMAND -4

// - Argument bitmap
#define CMI__ARG_RESTORE_SESSION 1
#define CMI__ARG_GUI_MODE 2

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

// - Viewport event code
#define CMI__VP_EVENT__NO_EVENT 0
#define CMI__VP_EVENT__ADD_MODEL 1
#define CMI__VP_EVENT__REMOVE_MODEL 2
