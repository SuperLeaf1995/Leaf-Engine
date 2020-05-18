/*
 * Leaf-Engine GNU Linux graphic
 * Copyright (C) 2020 Jesus A. Diaz <jesusantonio30122016@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include "graphic.h"

#define toRgb(r,g,b) (r+(g<<8)+(b<<16))

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

void setPalette(paletteEntry * p, register unsigned short n) {
	size_t i;
	for(i = 0; i < n; i++) {
		leafCurrentCtx->rgbPalette[i].r = p[i].b;
		leafCurrentCtx->rgbPalette[i].g = p[i].g;
		leafCurrentCtx->rgbPalette[i].b = p[i].r;
	}
	return;
}

void updateScreen(void) {
	register size_t i,i2,d;
	
	XNextEvent(leafCurrentCtx->xdisplay,&leafCurrentCtx->xevent);
	
	if(leafCurrentCtx->xevent.type == ClientMessage
	&& (unsigned int)leafCurrentCtx->xevent.xclient.data.l[0] == leafCurrentCtx->WM_DELETE_WINDOW) {
		leafCurrentCtx->ui = __LEAF_UI_EXIT_CODE;
	}
	
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
	
	/*Flush away the rest of the stuff*/
	XFlush(leafCurrentCtx->xdisplay);
	
	memset(leafCurrentCtx->videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
	return;
}
