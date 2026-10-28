#pragma once

// X11 Window Manager Header
#ifdef _W_X11
#include <X11/Xlib.h>
#include "X11/WindowManagerX11.h"
#endif


// Window Manager Standard
#if defined(_W_X11) && defined(_WM_STD__X11)
#define WM WM_X11
#define WM__Window WM_X11__Window
#define WM__openDisplay(handler) WM_X11__openDisplay(handler)
#define WM__closeDisplay(handler) WM_X11__closeDisplay(handler)

#define WM__createWindow(handler, WMIndex, BGPixel, BPixel, eventMask, depth, visual, class) WM_X11__createWindow(handler, WMIndex, BGPixel, BPixel, eventMask, depth, visual, class)
#define WM__updateWindow(handler) WM_X11__updateWindow(handler)
#define WM__destroyWindow(handler, WMIndex) WM_X11__destroyWindow(handler, WMIndex)

#define WM__useMultithreading WM_X11__useMultithreading()

#define WM__createGraphicsContext(handler, WMIndex) WM_X11__createGC(handler, WMIndex)
#define WM__createImage(handler, WMIndex, framebuffer) WM_X11__createImage(handler, WMIndex, framebuffer)
#define WM__updateImage(handler, WMIndex) WM_X11__updateImage(handler, WMIndex)
#endif