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

signed char Leaf_SetVideo(unsigned char v)
{
	*(unsigned long*)0x4000000 = 0x403; /*Set mode 3*/
	return (signed char)v;
}

void Leaf_SetPalette(Leaf_PaletteEntry * p,  unsigned short n)
{
	 unsigned short i;
	for(i = 0; i < n; i++)
	{
		Leaf_CurrentContext->rgbPalette[i] = p[i];
	}
	return;
}

void Leaf_UpdateScreen(void)
{
	size_t i;
	for(i = 0; i < (160*240); i++)
	{
		/*Why does this work?*/
		*(unsigned short *)(0x6000000+(i*sizeof(unsigned short))) = rgb16(
		Leaf_CurrentContext->rgbPalette[Leaf_CurrentContext->videoBuffer[i]].r,
		Leaf_CurrentContext->rgbPalette[Leaf_CurrentContext->videoBuffer[i]].g,
		Leaf_CurrentContext->rgbPalette[Leaf_CurrentContext->videoBuffer[i]].b
		);
	}
    return;
}

void Leaf_ClearScreen(void) {
    memset(Leaf_CurrentContext->videoBuffer,0,(size_t)Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); /* Clear our buffer */
    return;
}
