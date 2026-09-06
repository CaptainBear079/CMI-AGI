#include "WindowManagerX11.h"

bool WM_X11__openDisplay(WM_X11* handler) {
	if((handler->display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "X11: Can't open display. Error Code: 0x0001\n");
		return 1;
	}

	handler->screen = XDefaultScreen(handler->display);
	handler->root = RootWindow(handler->display, handler->screen);
	return 0;
}

void WM_X11__closeDisplay(WM_X11* handler) {
	XCloseDisplay(handler->display);
}

void WM_X11__createWindow(WM_X11* handler, int WMIndex,
	unsigned long BGPixel, unsigned long BPixel, long long eventMask,
	int depth, Visual* visual, unsigned int class) {
	handler->windows[WMIndex].xwa.background_pixel = BGPixel;
	handler->windows[WMIndex].xwa.border_pixel = BPixel;
	handler->windows[WMIndex].xwa.event_mask = eventMask;
	handler->windows[WMIndex].class = class;

	handler->windows[WMIndex].window = XCreateWindow(
		handler->display,
		handler->root,
		handler->windows[WMIndex].windowX, handler->windows[WMIndex].windowY,
		handler->windows[WMIndex].windowWidth, handler->windows[WMIndex].windowHeight,
		handler->windows[WMIndex].windowBorderWidth,
		depth,
		handler->windows[WMIndex].class,
		visual,
		CWBackPixel | CWBorderPixel | CWEventMask,
		&handler->windows[WMIndex].xwa
	);
	XMapWindow(handler->display, handler->windows[WMIndex].window);
	XFlush(handler->display);
	return;
}

void WM_X11__updateWindow(WM_X11* handler) {
	XFlush(handler->display);
	return;
}

void WM_X11__destroyWindow(WM_X11* handler, int WMIndex) {
	XUnmapWindow(handler->display, handler->windows[WMIndex].window);
	XDestroyWindow(handler->display, handler->windows[WMIndex].window);
	return;
}

bool WM_X11__useMultithreading() {
	if (!XInitThreads()) {
        fprintf(stderr, "X11 does not support multithreading\n");
		return -1;
	}
	return 0;
}

void WM_X11__createGC(WM_X11* handler, int WMIndex) {
	handler->windows[WMIndex].gc = XCreateGC(handler->display, handler->windows[WMIndex].window, 0, NULL);
	XSetFunction(handler->display, handler->windows[WMIndex].gc, GXcopy);
	return;
}

void WM_X11__createImage(WM_X11* handler, int WMIndex, uint32_t* framebuffer) {
	handler->windows[WMIndex].image = XCreateImage(
		handler->display,
		DefaultVisual(handler->display, handler->screen),
		DefaultDepth(handler->display, handler->screen),
		ZPixmap,
		0,
		(char*)framebuffer,
		handler->windows[WMIndex].windowWidth,
		handler->windows[WMIndex].windowHeight,
		32,
		0
	);
	return;
}

void WM_X11__updateImage(WM_X11* handler, int WMIndex) {
	XPutImage(
		handler->display,
		handler->windows[WMIndex].window,
		handler->windows[WMIndex].gc,
		handler->windows[WMIndex].image,
		0, 0,
		0, 0,
		handler->windows[WMIndex].windowWidth, handler->windows[WMIndex].windowHeight
	);
	return;
}
