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

#include "context.h"

signed int Leaf_ContextCreate(Leaf_Context * g) {
	fd_set is;
	
	Leaf_CurrentContext = g;
	
	Leaf_CurrentContext->name = NULL;
	Leaf_CurrentContext->rgbPalette = NULL;
	Leaf_CurrentContext->vwide = 640;
	Leaf_CurrentContext->vtall = 400;
	
	/*Create video buffer*/
	Leaf_CurrentContext->videoBuffer = calloc(Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall,sizeof(unsigned char));
	if(Leaf_CurrentContext->videoBuffer == NULL) {
		return -1;
	}
	
	/*Open the x11 display*/
	Leaf_CurrentContext->xdisplay = XOpenDisplay(0);
	/*error opening x11 display*/
	if(Leaf_CurrentContext->xdisplay == NULL) {
		return -2;
	}
	
	/*Create a "simple" window*/
	Leaf_CurrentContext->xwindow = XCreateSimpleWindow(Leaf_CurrentContext->xdisplay,RootWindow(Leaf_CurrentContext->xdisplay,
	BlackPixel(Leaf_CurrentContext->xdisplay,DefaultScreen(Leaf_CurrentContext->xdisplay))),10,10,Leaf_CurrentContext->vwide,
	Leaf_CurrentContext->vtall,1,WhitePixel(Leaf_CurrentContext->xdisplay,DefaultScreen(Leaf_CurrentContext->xdisplay)),
	BlackPixel(Leaf_CurrentContext->xdisplay,DefaultScreen(Leaf_CurrentContext->xdisplay)));
	
	/*Select all inputs*/
	XSelectInput(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xwindow,
	ExposureMask
	|KeyPressMask
	|PointerMotionMask
	|ButtonPressMask
	|ButtonReleaseMask
	|KeyReleaseMask
	|PointerMotionMask
	|StructureNotifyMask
	);
	
	/*Map our window*/
	XMapWindow(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xwindow);
	
	/*Display the close button*/
	g->WM_DELETE_WINDOW = XInternAtom(Leaf_CurrentContext->xdisplay,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xwindow,&g->WM_DELETE_WINDOW,1);
	g->xgraphic = XCreateGC(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xwindow,0,0);
	
	/*Create a dummy palette*/
	Leaf_CurrentContext->rgbPalette = malloc(sizeof(Leaf_PaletteEntry)*256);
	if(Leaf_CurrentContext->rgbPalette == NULL) {
		return -3;
	}
	
	/*Wait until the window is mapped into the screen*/
	for(;;) {
		XNextEvent(Leaf_CurrentContext->xdisplay,&Leaf_CurrentContext->xevent);
		if(Leaf_CurrentContext->xevent.type == MapNotify) {
			break;
		}
	}
	
	/*Connect to our display*/
	Leaf_CurrentContext->xFd = ConnectionNumber(Leaf_CurrentContext->xdisplay);
	
	FD_ZERO(&is);
	FD_SET(Leaf_CurrentContext->xFd,&is);

#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	return 0;
}

signed int Leaf_ContextDestroy(Leaf_Context * g) {
	if(Leaf_CurrentContext == NULL
	|| Leaf_CurrentContext != g) {
		return 0;
	}
	if(Leaf_CurrentContext->videoBuffer != NULL) {
		free(Leaf_CurrentContext->videoBuffer);
	}
	if(Leaf_CurrentContext->name != NULL) {
		free(Leaf_CurrentContext->name);
	}
	if(Leaf_CurrentContext->rgbPalette != NULL) {
		free(Leaf_CurrentContext->rgbPalette);
	}
	/*Free the graphic context*/
	XFreeGC(Leaf_CurrentContext->xdisplay,Leaf_CurrentContext->xgraphic);
	/*Free the display context*/
	XCloseDisplay(Leaf_CurrentContext->xdisplay);
	Leaf_CurrentContext = NULL;
	
	/*Kill the key handler thread*/
	/*pthread_kill(key_thread,SIGKILL);*/
	
	return 0;
}
