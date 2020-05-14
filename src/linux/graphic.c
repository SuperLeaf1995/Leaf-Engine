#include "graphic.h"

signed char setVideo(unsigned char v) {
	leafCurrentCtx->vwide = vtable[v][0];
	leafCurrentCtx->vtall = vtable[v][1];
	leafCurrentCtx->vvideo = vtable[v][2];
	
	leafCurrentCtx->videoBuffer = (unsigned char *)realloc(leafCurrentCtx->videoBuffer,(leafCurrentCtx->vwide*leafCurrentCtx->vtall));
	if(leafCurrentCtx->videoBuffer == NULL) {
		return -1;
	}
	return 0;
}

#define toRgb(r,g,b) (r+(g<<8)+(b<<16))

void setPalette(paletteEntry * p, register unsigned short n) {
	register unsigned short i;
	for(i = 0; i < n; i++) {
		leafCurrentCtx->rgbPalette[i].r = p[i].r;
		leafCurrentCtx->rgbPalette[i].g = p[i].g;
		leafCurrentCtx->rgbPalette[i].b = p[i].b;
	}
	return;
}

void updateEvent(void) {
	XNextEvent(leafCurrentCtx->xdisplay,&leafCurrentCtx->xevent);
	
	if(leafCurrentCtx->xevent.type == ClientMessage
	&& (unsigned int)leafCurrentCtx->xevent.xclient.data.l[0] == leafCurrentCtx->WM_DELETE_WINDOW) {
		leafCurrentCtx->ui = __LEAF_UI_EXIT_CODE;
	}
	return;
}

void updateScreen(void) {
	register size_t i,i2,d;
	
	if(leafCurrentCtx == NULL) {
		return;
	}
	
	/*Draw in the window the video buffer*/
	for(i = 0; i < leafCurrentCtx->vwide; i++) {
		for(i2 = 0; i2 < leafCurrentCtx->vtall; i2++) {
			/*Set the pixel colour according to the palette*/
			d = leafCurrentCtx->videoBuffer[((i2*leafCurrentCtx->vwide)+i)];
			
			XSetForeground(leafCurrentCtx->xdisplay,leafCurrentCtx->xgraphic,
			toRgb(leafCurrentCtx->rgbPalette[d].r,leafCurrentCtx->rgbPalette[d].g,leafCurrentCtx->rgbPalette[d].b));
			
			XDrawPoint(leafCurrentCtx->xdisplay,leafCurrentCtx->xwindow,leafCurrentCtx->xgraphic,i,i2);
		}
	}
	
	memset(leafCurrentCtx->videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
	return;
}
