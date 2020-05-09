#include "graphic.h"

void setPalette(paletteEntry * p, register unsigned short n) {
	register unsigned short i;
	for(i = 0; i < n; i++) {
		leafCurrentCtx->rgbPalette[i] = p[i];
	}
	return;
}

void updateScreen(void) {
	memset(videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
	return;
}
