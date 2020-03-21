#include "leaf.h"

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
static volatile uint16_t *clock = (uint16_t *)0x0000046C; /*Clock always changes*/
static uint8_t *textMemory = (uint8_t *)0xB8000000L;
#endif
#if defined(__linux) || defined(linux)
Display * xdisplay;
Window mainWindow;
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

void initLeaf(void) {
#if defined(__linux) || defined(linux)
	xdisplay = XOpenDisplay(0);
	mainWindow = XRootWindow(xdisplay,0);
	XSelectInput(xdisplay,mainWindow,KeyReleaseMask);
#endif
	return;
}

void endLeaf(void) {
#if defined(__linux) || defined(linux)
	XFlush(xdisplay);
#endif
	return;
}
