/*
 * Leaf-Engine GBA graphic
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

unsigned int setVideo(unsigned char v)
{
	*(unsigned long*)0x4000000 = 0x403; /*Set mode 3*/
	return (unsigned int)v;
}

void setPalette(paletteEntry * p, register unsigned short n)
{
	register unsigned short i;
	for(i = 0; i < n; i++)
	{
		leafCurrentCtx->rgbPalette[i] = p[i];
	}
	return;
}

void updateScreen(void)
{
	size_t i;
	for(i = 0; i < (160*240); i++)
	{
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
