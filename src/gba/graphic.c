#include "graphic.h"

unsigned int setVideo(unsigned char v) {
	*(unsigned long*)0x4000000 = 0x403; /*Set mode 3*/
	return (unsigned int)v;
}

void setPalette(paletteEntry * p, register unsigned short n) {
	register unsigned short i;
	for(i = 0; i < n; i++) {
		leafCurrentCtx->rgbPalette[i] = p[i];
	}
	return;
}

void updateScreen(void) {
	size_t i;
	for(i = 0; i < (160*240); i++) {
		/*Why does this work?*/
		*(unsigned short *)(0x6000000+(i*sizeof(unsigned short))) = rgb16(
		leafCurrentCtx->rgbPalette[videoBuffer[i]].r,
		leafCurrentCtx->rgbPalette[videoBuffer[i]].g,
		leafCurrentCtx->rgbPalette[videoBuffer[i]].b
		);
	}
	memset(videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
	return;
}
