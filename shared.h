#pragma once

// Defines
// - Error exit codes
#define CMI_ERROR__INVALID_ARG -1
#define CMI_ERROR__NO_MULTITHREADING -2
#define CMI_ERROR__NO_GUI_SUPPORT -3

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
