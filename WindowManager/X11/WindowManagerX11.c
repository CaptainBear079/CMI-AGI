#include "WindowManagerX11.h"

bool WM_X11__openDisplay(WM_X11__Window* handler) {
	if((handler->display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "X11: Can't open display. Error Code: 0x0001\n");
		return 1;
	}

	handler->screen = XDefaultScreen(handler->display);
	handler->root = RootWindow(handler->display, handler->screen);
	return;
}

void WM_X11__closeDisplay(WM_X11__Window* handler) {
	XCloseDisplay(handler->display);
}

void WM_X11__createWindow(WM_X11__Window* handler,
	unsigned long BGPixel, unsigned long BPixel, long long eventMask,
	int depth, Visual* visual, unsigned int class) {
	handler->xwa.background_pixel = BGPixel;
	handler->xwa.border_pixel = BPixel;
	handler->xwa.event_mask = eventMask;
	handler->class = class;

	handler->window = XCreateWindow(
		handler->display,
		handler->root,
		handler->windowX, handler->windowY,
		handler->windowWidth, handler->windowHeight,
		handler->windowBorderWidth,
		depth,
		handler->class,
		visual,
		CWBackPixel | CWBorderPixel | CWEventMask,
		&handler->xwa
	);
	XMapWindow(handler->display, handler->window);
	XFlush(handler->display);
	return;
}

void WM_X11__updateWindow(WM_X11__Window* handler) {
	XFlush(handler->display);
	return;
}

void WM_X11__destroyWindow(WM_X11__Window* handler) {
	XUnmapWindow(handler->display, handler);
	XDestroyWindow(handler->display, handler);
	return;
}

bool WM_X11__useMultithreading() {
	if (!XInitThreads()) {
        fprintf(stderr, "X11 does not support multithreading\n");
		return -1;
	}
	return 0;
}

void WM_X11__createGC(WM_X11__Window* handler) {
	handler->gc = XCreateGC(handler->display, handler->window, 0, NULL);
	XSetFunction(handler->display, handler->gc, GXcopy);
	return;
}

void WM_X11__createImage(WM_X11__Window* handler, uint32_t* framebuffer) {
	handler->image = XCreateImage(
		handler->display,
		DefaultVisual(handler->display, handler->screen),
		DefaultDepth(handler->display, handler->screen),
		ZPixmap,
		0,
		(char*)framebuffer,
		handler->windowWidth,
		handler->windowHeight,
		32,
		0
	);
	return;
}

void WM_X11__updateImage(WM_X11__Window* handler) {
	XPutImage(
		handler->display,
		handler->window,
		handler->gc,
		handler->image,
		0, 0,
		0, 0,
		handler->windowWidth, handler->windowHeight
	);
	return;
}
