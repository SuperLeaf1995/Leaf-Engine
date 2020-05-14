/*
 * Leaf-Engine GNU Linux context
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
 
 /*
 * Leaf-Engine GNU Linux context
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

signed int leafContextCreate(leafContext * g)
{
	size_t i;
	
	leafCurrentCtx = g;
	
	if(g->vwide == 0) {
		g->vwide = 640;
	} if(g->vtall == 0) {
		g->vtall = 400;
	}
	
	/*Create video buffer*/
	g->videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(g->videoBuffer == NULL)
	{
		return -1;
	}
	
	/*Open the x11 display*/
	g->xdisplay = XOpenDisplay(0);
	/*error opening x11 display*/
	if(g->xdisplay == NULL)
	{
		return -2;
	}
	
	g->xwindow = XCreateSimpleWindow(g->xdisplay,RootWindow(g->xdisplay,
	BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay))),10,10,g->vwide,
	g->vtall,1,WhitePixel(g->xdisplay,DefaultScreen(g->xdisplay)),
	BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay)));
	
	/*Select all inputs*/
	XSelectInput(g->xdisplay,g->xwindow,
	ExposureMask
	|KeyPressMask
	|PointerMotionMask
	|ButtonPressMask
	|ButtonReleaseMask
	|KeyReleaseMask
	|PointerMotionMask
	);
	
	XMapWindow(g->xdisplay,g->xwindow); /*Display it (or map it)*/
	if(g->name != NULL)
	{
		XStoreName(g->xdisplay,g->xwindow,g->name); /*Set the window's title*/
	}
	
	/*Display the close button*/
	g->WM_DELETE_WINDOW = XInternAtom(g->xdisplay,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(g->xdisplay,g->xwindow,&g->WM_DELETE_WINDOW,1);
	g->xgraphic = XCreateGC(g->xdisplay,g->xwindow,0,0);
	
	/*Load a default VGA palette (DOSBox and other programs does this)*/
	g->rgbPalette = malloc(sizeof(paletteEntry)*256);
	if(g->rgbPalette == NULL)
	{
		return -3;
	}
	
	for(i = 0; i < 256; i++)
	{
		g->rgbPalette[i].r = 0;
		g->rgbPalette[i].g = 0;
		g->rgbPalette[i].b = 0;
	}
	
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	return 0;
}

signed int leafContextDestroy(leafContext * g)
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
