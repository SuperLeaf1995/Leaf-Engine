#include "graphic.h"

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

void updateScreen(void) {
	size_t i,i2,d;
	
	XNextEvent(leafCurrentCtx->xdisplay,&leafCurrentCtx->xevent);
	if(leafCurrentCtx->xevent.type == ClientMessage
	&& (unsigned int)leafCurrentCtx->xevent.xclient.data.l[0] == leafCurrentCtx->WM_DELETE_WINDOW) {
		leafCurrentCtx->ui = _exit_code;
	}
	
	if(videoBuffer == NULL) {
		return;
	} else if(leafCurrentCtx->vtall == 0) {
		return;
	} else if(leafCurrentCtx->vwide == 0) {
		return;
	}
	
	/*Draw in the window the video buffer*/
	for(i = 0; i < leafCurrentCtx->vwide; i++) {
		for(i2 = 0; i2 < leafCurrentCtx->vtall; i2++) {
			d = videoBuffer[((i2*leafCurrentCtx->vwide)+i)]; /*Point at the current pixel's address*/
			XSetForeground	(
								leafCurrentCtx->xdisplay,
								leafCurrentCtx->xgraphic,
								toRgb	(
											leafCurrentCtx->rgbPalette[d].r,
											leafCurrentCtx->rgbPalette[d].g,
											leafCurrentCtx->rgbPalette[d].b
										)
							);
			XDrawPoint(leafCurrentCtx->xdisplay,leafCurrentCtx->xwindow,leafCurrentCtx->xgraphic,i,i2);
		}
	}
	
	memset(videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
	return;
}
