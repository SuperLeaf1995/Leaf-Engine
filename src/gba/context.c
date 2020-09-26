/*
 * Leaf-Engine GBA context
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

signed int Leaf_ContextCreate(Leaf_Context * g)
{
	g->vwide = 240; g->vtall = 160;
	g->videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(g->videoBuffer == NULL)
	{
		return -1;
	}
	g->rgbPalette = (Leaf_PaletteEntry *)malloc(sizeof(Leaf_PaletteEntry)*256);
	if(g->rgbPalette == NULL)
	{
		return -2;
	}
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	Leaf_CurrentContext = g;
	return 0;
}

signed int Leaf_ContextDestroy(Leaf_Context * g)
{
	if(g->videoBuffer != NULL)
	{
		free(g->videoBuffer);
	}
	if(g->name != NULL)
	{
		free(g->name);
	}
	return 0;
}
