/*
 * Leaf-Engine DOS context
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

#include "context.h"

signed int leafContextCreate(leafContext * g)
{
#if defined(__DJGPP__)
	if(__djgpp_nearptr_enable() == 0) {
		fprintf(stderr,"Cannot enable nearptr\n");
		exit(-1);
	}
#endif /* __DJGPP__ */
	g->vwide = 320; g->vtall = 200;
	g->videoBuffer = (unsigned char *)malloc((g->vwide*g->vtall));
	
	if(g->videoBuffer == NULL)
	{
		return -1;
	}
	
	leafCurrentCtx = g;
	return 0;
}

signed int leafContextDestroy(leafContext * g)
{
	if(leafCurrentCtx->videoBuffer != NULL)
	{
		free(leafCurrentCtx->videoBuffer);
	}
	if(g->name != NULL)
	{
		free(g->name);
	}
#if defined(__DJGPP__)
	__djgpp_nearptr_disable();
#endif
	return 0;
}
