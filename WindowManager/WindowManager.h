#pragma once

// X11 Window Manager Header
#ifdef _W_X11
#include <X11/Xlib.h>
#include "X11/WindowManagerX11.h"
typedef struct _WM_X11__Window_ {
	Display *display;
	int screen;
	Window root;
	Window window;
	XSetWindowAttributes xwa;
	GC gc;
	XImage* image;
	int windowX;
	int windowY;
	unsigned int windowBorderWidth;
	unsigned int windowWidth;
	unsigned int windowHeight;
	unsigned int class;
} WM_X11__Window;
#endif

// Window Manager Standard
#if defined(_W_X11) && defined(_WM_STD__X11)
#define WM__Window WM_X11__Window
#define WM__openDisplay(handler) WM_X11__openDisplay(handler)

#define WM__createWindow(handler, BGPixel, BPixel, eventMask, depth, visual, class) WM_X11__createWindow(handler, BGPixel, BPixel, eventMask, depth, visual, class)
#define WM__updateWindow(handler) WM_X11__updateWindow(handler)
#define WM__destroyWindow(handler) WM_X11__destroyWindow(handler)

#define WM__useMultithreading WM_X11__useMultithreading()

#define WM__createGraphicsContext(handler) WM_X11__createGC(handler)
#define WM__createImage(handler, framebuffer) WM_X11__createImage(handler, framebuffer)
#define WM__updateImage(handler) WM_X11__updateImage(handler)
#endif