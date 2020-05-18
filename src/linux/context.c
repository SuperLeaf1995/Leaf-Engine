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

signed int Leaf_ContextCreate(leafContext * g) {
	fd_set is;
	
	leafCurrentCtx = g;
	
	leafCurrentCtx->vwide = 640;
	leafCurrentCtx->vtall = 400;
	
	/*Create video buffer*/
	leafCurrentCtx->videoBuffer = calloc(leafCurrentCtx->vwide*leafCurrentCtx->vtall,sizeof(unsigned char));
	if(leafCurrentCtx->videoBuffer == NULL) {
		return -1;
	}
	
	/*Open the x11 display*/
	leafCurrentCtx->xdisplay = XOpenDisplay(0);
	/*error opening x11 display*/
	if(leafCurrentCtx->xdisplay == NULL) {
		return -2;
	}
	
	/*Create a "simple" window*/
	leafCurrentCtx->xwindow = XCreateSimpleWindow(leafCurrentCtx->xdisplay,RootWindow(leafCurrentCtx->xdisplay,
	BlackPixel(leafCurrentCtx->xdisplay,DefaultScreen(leafCurrentCtx->xdisplay))),10,10,leafCurrentCtx->vwide,
	leafCurrentCtx->vtall,1,WhitePixel(leafCurrentCtx->xdisplay,DefaultScreen(leafCurrentCtx->xdisplay)),
	BlackPixel(leafCurrentCtx->xdisplay,DefaultScreen(leafCurrentCtx->xdisplay)));
	
	/*Select all inputs*/
	XSelectInput(leafCurrentCtx->xdisplay,leafCurrentCtx->xwindow,
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
	XMapWindow(leafCurrentCtx->xdisplay,leafCurrentCtx->xwindow);
	
	/*Display the close button*/
	g->WM_DELETE_WINDOW = XInternAtom(leafCurrentCtx->xdisplay,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(leafCurrentCtx->xdisplay,leafCurrentCtx->xwindow,&g->WM_DELETE_WINDOW,1);
	g->xgraphic = XCreateGC(leafCurrentCtx->xdisplay,leafCurrentCtx->xwindow,0,0);
	
	/*Create a dummy palette*/
	leafCurrentCtx->rgbPalette = malloc(sizeof(paletteEntry)*256);
	if(leafCurrentCtx->rgbPalette == NULL) {
		return -3;
	}
	
	/*Wait until the window is mapped into the screen*/
	for(;;) {
		XNextEvent(leafCurrentCtx->xdisplay,&leafCurrentCtx->xevent);
		if(leafCurrentCtx->xevent.type == MapNotify) {
			break;
		}
	}
	
	/*Connect to our display*/
	leafCurrentCtx->xFd = ConnectionNumber(leafCurrentCtx->xdisplay);
	
	FD_ZERO(&is);
	FD_SET(leafCurrentCtx->xFd,&is);

#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	return 0;
}

signed int Leaf_ContextDestroy(leafContext * g) {
	if(leafCurrentCtx == NULL
	|| leafCurrentCtx != g) {
		return 0;
	}
	if(leafCurrentCtx->videoBuffer != NULL) {
		free(leafCurrentCtx->videoBuffer);
	}
	if(leafCurrentCtx->name != NULL) {
		free(leafCurrentCtx->name);
	}
	if(leafCurrentCtx->rgbPalette != NULL) {
		free(leafCurrentCtx->rgbPalette);
	}
	/*Free the graphic context*/
	XFreeGC(leafCurrentCtx->xdisplay,leafCurrentCtx->xgraphic);
	/*Free the display context*/
	XCloseDisplay(leafCurrentCtx->xdisplay);
	leafCurrentCtx = NULL;
	return 0;
}
