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

void Leaf_DrawPixel( unsigned short x,  unsigned short y,  unsigned char c)
{
	if(y >= Leaf_CurrentContext->vtall || x >= Leaf_CurrentContext->vwide) {
		return;
	}
	Leaf_CurrentContext->videoBuffer[(y*Leaf_CurrentContext->vwide)+x] = c;
	return;
}

void Leaf_DrawLinearPixel( unsigned short p, unsigned char c)
{
	Leaf_CurrentContext->videoBuffer[p] = c;
	return;
}

unsigned char Leaf_GetPixel( unsigned short x, unsigned short y)
{
	return Leaf_CurrentContext->videoBuffer[(y*Leaf_CurrentContext->vwide)+x];
}

unsigned char Leaf_GetLinearPixel( unsigned short p)
{
	return Leaf_CurrentContext->videoBuffer[p];
}

void Leaf_DrawLine( signed short sx,  signed short sy,  signed short ex,  signed short ey,  unsigned char c)
{
	signed short i,dx,dy,sdx,sdy,dxabs,dyabs,px,py,x,y;
	dx = ex-sx; dy = ey-sy;
	sdx = ((dx < 0) ? -1 : ((dx > 0) ? 1 : 0));
	sdy = ((dy < 0) ? -1 : ((dy > 0) ? 1 : 0));
	dxabs = abs(dx); dyabs = abs(dy);
	x = (dyabs>>1); y = (dxabs>>1);
	px = sx; py = sy;
	
	Leaf_DrawPixel(px,py,c);
	
	if(dxabs >= dyabs) {
		for(i = 0; i < dxabs; i++) {
			y += dyabs;
			if(y >= dxabs) {
				y -= dxabs; py += sdy;
			}
			px += sdx;
			Leaf_DrawPixel(px,py,c);
		}
	} else {
		for(i = 0; i < dyabs; i++) {
			x += dxabs;
			if(x >= dyabs) {
				x -= dyabs; px += sdx;
			}
			py += sdy;
			Leaf_DrawPixel(px,py,c);
		}
	}
	return;
}

void Leaf_DrawWireSquare( signed short x1,  signed short y1,  signed short x2,  signed short y2,  unsigned char c)
{
	Leaf_DrawLine(x1,y1,x2,y1,c); Leaf_DrawLine(x1,y1,x1,y2,c);
	Leaf_DrawLine(x1,y2,x2,y2,c); Leaf_DrawLine(x2,y2,x2,y1,c);
	return;
}

void Leaf_DrawWirePolygon(signed short * d,  unsigned short n,  unsigned char c)
{
	unsigned short i;
	for(i = 0; i < n-1; i++) {
		Leaf_DrawLine(d[(i<<1)],d[((i<<1)+1)],d[((i<<1)+2)],d[((i<<1)+3)],c);
	}
	Leaf_DrawLine(d[(i<<1)],d[(i<<1)+1],d[0],d[1],c);
	return;
}

void Leaf_DrawImage(Leaf_Image * img, unsigned short x, unsigned short y)
{
	unsigned short i;
	unsigned short i2;
	for(i = 0; i < img->wide; i++) {
		for(i2 = 0; i2 < img->tall; i2++) {
			Leaf_DrawPixel(x+i,y+i2,img->data[(i2*img->wide)+i]);
		}
	}
	return;
}

void Leaf_DrawTiled(Leaf_Image * img, unsigned short x, unsigned short y, unsigned short ix, unsigned short iy)
{
	unsigned short i,i2;
	unsigned short tTall;

	for(i2 = 0; i2 < 16; i2++) {
		tTall = (((iy<<4)+i2)*img->wide);

		for(i = 0; i < 16; i++) {
			Leaf_DrawPixel(x+i,y+i2,img->data[tTall+(ix<<4)+i]);
		}
	}
	return;
}
