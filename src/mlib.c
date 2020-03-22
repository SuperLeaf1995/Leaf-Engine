#include "leaf.h"

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
static volatile uint16_t *clock = (uint16_t *)0x0000046C; /*Clock always changes*/
static uint8_t *textMemory = (uint8_t *)0xB8000000L;
#endif
#if defined(__linux) || defined(linux)
Display * xdisplay;
Window mainWindow;
int s;
#endif

/* ENGINE.C
 * Base stuff, like functions used for the internal of everything*/

void fskip(FILE *stream, uint64_t n) {
	static uint64_t i;
	for(i = 0; i < (n+1); i++) {
		fgetc(stream); /*Skip characters*/
	}
	return;
}

signed int initLeaf(void) {
#if defined(__linux) || defined(linux)
	xdisplay = XOpenDisplay(0);
	if(xdisplay == NULL) {
		fprintf(stderr,"Can't create X11 display\n");
		return 0;
	}
	s = DefaultScreen(xdisplay);
	mainWindow = XCreateSimpleWindow(xdisplay,RootWindow(xdisplay,s),10,10,640,400,1,BlackPixel(xdisplay,s),WhitePixel(xdisplay,s));
	XSelectInput(xdisplay,mainWindow,ExposureMask|KeyReleaseMask);
	XMapWindow(xdisplay,mainWindow);
	XStoreName(xdisplay,mainWindow,"LeafEngine game");
#endif
	return 1;
}

void endLeaf(void) {
#if defined(__linux) || defined(linux)
	XDestroyWindow(xdisplay,mainWindow);
	XCloseDisplay(xdisplay);
#endif
	return;
}
