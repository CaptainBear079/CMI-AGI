#pragma once

// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <X11/Xlib.h>

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

bool WM_X11__openDisplay(WM_X11__Window* handler);
void WM_X11__closeDisplay(WM_X11__Window* handler);

void WM_X11__createWindow(WM_X11__Window* handler,
	unsigned long BGPixel, unsigned long BPixel, long long eventMask,
	int depth, Visual* visual, unsigned int class);
void WM_X11__updateWindow(WM_X11__Window* handler);
void WM_X11__destroyWindow(WM_X11__Window* handler);

bool WM_X11__useMultithreading();

void WM_X11__createGC(WM_X11__Window* handler);
void WM_X11__createImage(WM_X11__Window* handler, uint32_t* framebuffer);
void WM_X11__updateImage(WM_X11__Window* handler);
