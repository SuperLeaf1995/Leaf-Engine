signed int leafContextCreate(leafContext * g) {
	leafCurrentCtx = g;
	
	if(g->vwide == 0) {
		g->vwide = 640;
	} if(g->vtall == 0) {
		g->vtall = 400;
	}
	
	/*Create video buffer*/
	g->videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(g->videoBuffer == NULL) {
		return -1;
	}
	
	/*Open the x11 display*/
	g->xdisplay = XOpenDisplay(0);
	if(g->xdisplay == NULL) {
		return -2;
	} /*error opening x11 display*/
	
	g->xwindow = XCreateSimpleWindow(g->xdisplay,RootWindow(g->xdisplay,
	BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay))),10,10,g->vwide,
	g->vtall,1,WhitePixel(g->xdisplay,DefaultScreen(g->xdisplay)),
	BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay)));
	
	XSelectInput(g->xdisplay,g->xwindow,ExposureMask|KeyReleaseMask); /*Select input...*/
	XMapWindow(g->xdisplay,g->xwindow); /*Display it (or map it)*/
	if(g->name != NULL) {
		XStoreName(g->xdisplay,g->xwindow,g->name); /*Set the window's title*/
	}
	
	/*Display the close button*/
	g->WM_DELETE_WINDOW = XInternAtom(g->xdisplay,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(g->xdisplay,g->xwindow,&g->WM_DELETE_WINDOW,1);
	g->xgraphic = XCreateGC(g->xdisplay,g->xwindow,0,0);
	
	/*Load a default VGA palette (DOSBox and other programs does this)*/
	g->rgbPalette = malloc(256*sizeof(paletteEntry));
	if(g->rgbPalette == NULL) {
		return -3;
	}
	
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	return 0;
}

signed int leafContextDestroy(leafContext * g) {
	if(g->videoBuffer != NULL) {
		free(g->videoBuffer);
	}
	if(g->name != NULL) {
		free(g->name);
	}
	return 0;
}
