/*
 * Leaf-Engine graphic
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

#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
#include "dos\graphic.c"
#endif

#if defined(__APPLE2__)
#include "apple2/graphic.c"
#endif

#if defined(__GBA__)
#include "gba/graphic.c"
#endif

#if defined(__linux) || defined(linux)
#include "linux/graphic.c"
#endif

void Leaf_plotPixel(register unsigned short x, register unsigned short y, register unsigned char c)
{
	if(y >= leafCurrentCtx->vtall || x >= leafCurrentCtx->vwide) {
		return;
	}
	leafCurrentCtx->videoBuffer[(y*leafCurrentCtx->vwide)+x] = c;
	return;
}

void Leaf_plotLinearPixel(register unsigned short p,register unsigned char c)
{
	leafCurrentCtx->videoBuffer[p] = c;
	return;
}

unsigned char Leaf_fetchPixel(register unsigned short x,register unsigned short y)
{
	return leafCurrentCtx->videoBuffer[(y*leafCurrentCtx->vwide)+x];
}

unsigned char Leaf_fetchLinearPixel(register unsigned short p)
{
	return leafCurrentCtx->videoBuffer[p];
}

void Leaf_plotLine(register signed short sx, register signed short sy, register signed short ex, register signed short ey, register unsigned char c)
{
	signed short i,dx,dy,sdx,sdy,dxabs,dyabs,px,py,x,y;
	dx = ex-sx; dy = ey-sy;
	sdx = ((dx < 0) ? -1 : ((dx > 0) ? 1 : 0));
	sdy = ((dy < 0) ? -1 : ((dy > 0) ? 1 : 0));
	dxabs = abs(dx); dyabs = abs(dy);
	x = (dyabs>>1); y = (dxabs>>1);
	px = sx; py = sy;
	
	Leaf_plotPixel(px,py,c);
	
	if(dxabs >= dyabs) {
		for(i = 0; i < dxabs; i++) {
			y += dyabs;
			if(y >= dxabs) {
				y -= dxabs; py += sdy;
			}
			px += sdx;
			Leaf_plotPixel(px,py,c);
		}
	} else {
		for(i = 0; i < dyabs; i++) {
			x += dxabs;
			if(x >= dyabs) {
				x -= dyabs; px += sdx;
			}
			py += sdy;
			Leaf_plotPixel(px,py,c);
		}
	}
	return;
}

void Leaf_plotWireSquare(register signed short x1, register signed short y1, register signed short x2, register signed short y2, register unsigned char c)
{
	Leaf_plotLine(x1,y1,x2,y1,c); Leaf_plotLine(x1,y1,x1,y2,c);
	Leaf_plotLine(x1,y2,x2,y2,c); Leaf_plotLine(x2,y2,x2,y1,c);
	return;
}

void Leaf_plotWirePolygon(signed short * d, register unsigned short n, register unsigned char c)
{
	unsigned short i;
	for(i = 0; i < n-1; i++) {
		Leaf_plotLine(d[(i<<1)],d[((i<<1)+1)],d[((i<<1)+2)],d[((i<<1)+3)],c);
	}
	Leaf_plotLine(d[(i<<1)],d[(i<<1)+1],d[0],d[1],c);
	return;
}

void Leaf_drawImage(Leaf_Image * img, unsigned short x, unsigned short y)
{
	unsigned short i;
	unsigned short i2;
	for(i = 0; i < img->wide; i++) {
		for(i2 = 0; i2 < img->tall; i2++) {
			Leaf_plotPixel(x+i,y+i2,img->data[(i2*img->wide)+i]);
		}
	}
	return;
}

void Leaf_drawTiled(Leaf_Image * img, unsigned short x, unsigned short y, unsigned short ix, unsigned short iy)
{
	unsigned short i,i2;
	unsigned short tTall;

	for(i2 = 0; i2 < 16; i2++) {
		tTall = (((iy<<4)+i2)*img->wide);

		for(i = 0; i < 16; i++) {
			Leaf_plotPixel(x+i,y+i2,img->data[tTall+(ix<<4)+i]);
		}
	}
	return;
}
