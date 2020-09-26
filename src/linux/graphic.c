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

signed char Leaf_SetVideo(unsigned char v) {
	Leaf_CurrentContext->vwide = vtable[v][0];
	Leaf_CurrentContext->vtall = vtable[v][1];
	Leaf_CurrentContext->vvideo = vtable[v][2];

	Leaf_CurrentContext->videoBuffer = (unsigned char *)realloc(Leaf_CurrentContext->videoBuffer,(Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall));
	if(Leaf_CurrentContext->videoBuffer == NULL) {
		return -1;
	}
	return 0;
}

void Leaf_SetPalette(Leaf_PaletteEntry * p,  unsigned short n) {
	size_t i;
	for(i = 0; i < n; i++) {
		Leaf_CurrentContext->rgbPalette[i].b = p[i].r;
		Leaf_CurrentContext->rgbPalette[i].g = p[i].g;
		Leaf_CurrentContext->rgbPalette[i].r = p[i].b;
	}
	return;
}

extern int Leaf_eventGet(void);
extern void Leaf_EventUpdate(void);

void Leaf_UpdateScreen(void) {
	 size_t i,i2,d;
	 
	 Leaf_EventUpdate();

	/*Draw in the window the video buffer*/
	for(i = 0; i < Leaf_CurrentContext->vwide; i++) {
		for(i2 = 0; i2 < Leaf_CurrentContext->vtall; i2++) {
			/*Set the pixel colour according to the palette*/
			d = Leaf_CurrentContext->videoBuffer[((i2*Leaf_CurrentContext->vwide)+i)];

			XSetForeground(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xgraphic,
			toRgb(Leaf_CurrentContext->rgbPalette[d].r,Leaf_CurrentContext->rgbPalette[d].g,Leaf_CurrentContext->rgbPalette[d].b));

			XDrawPoint(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xwindow,Leaf_CurrentContext->xgraphic,i,i2);
		}
	}

	/*Flush away the rest of the stuff*/
	XFlush(Leaf_CurrentContext->xdisplay);
	return;
}

void Leaf_ClearScreen(void) {
    memset(Leaf_CurrentContext->videoBuffer,0,(size_t)Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); /* Clear our buffer */
    return;
}
