signed int leafContextCreate(leafContext * g) {
	size_t i;
	g->xdis
	g->vwide = 320; g->vtall = 200;
	
	/*Create video buffer*/
	videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(videoBuffer == NULL) {
		return -1;
	}
	
	/*Open the x11 display*/
	g->xdisplay = XOpenDisplay(0);
	if(g->xdisplay == NULL) {
		return -2;
	} /*error opening x11 display*/
	g->xwindow = XCreateSimpleWindow(g->xdisplay,RootWindow(g->xdisplay,BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay))),10,10,320,200,1,WhitePixel(g->xdisplay,DefaultScreen(g->xdisplay)),BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay)));
	XSelectInput(g->xdisplay,g->xwindow,ExposureMask|KeyReleaseMask); /*Select input...*/
	XMapWindow(g->xdisplay,g->xwindow); /*Display it (or map it)*/
	XStoreName(g->xdisplay,g->xwindow,g->name); /*Set the window's title*/
	
	/*Display the close button*/
	g->WM_DELETE_WINDOW = XInternAtom(g->xdisplay,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(g->xdisplay,g->xwindow,&g->WM_DELETE_WINDOW,1);
	g->xgraphic = XCreateGC(g->xdisplay,g->xwindow,0,0);
	
	/*Load a default VGA palette (DOSBox and other programs does this)*/
	g->xpalette = malloc(255*sizeof(paletteEntry));
	if(g->xpalette == NULL) {
		return -3;
	}
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	leafCurrentCtx = g;
	return 0;
}

	unsigned short i;
	g->xdisplay = XOpenDisplay(0); /*open the x11 display*/
	if(g->xdisplay == NULL) { return -1; } /*error opening x11 display*/
	g->xwindow = XCreateSimpleWindow(g->xdisplay,RootWindow(g->xdisplay,BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay))),10,10,320,200,1,WhitePixel(g->xdisplay,DefaultScreen(g->xdisplay)),BlackPixel(g->xdisplay,DefaultScreen(g->xdisplay)));
	XSelectInput(g->xdisplay,g->xwindow,ExposureMask|KeyReleaseMask); /*select input...*/
	XMapWindow(g->xdisplay,g->xwindow); /*display it (or map it)*/
	XStoreName(g->xdisplay,g->xwindow,g->name); /*set the window's title*/
	/*display the close button*/
	g->WM_DELETE_WINDOW = XInternAtom(g->xdisplay,"WM_DELETE_WINDOW",False);
	XSetWMProtocols(g->xdisplay,g->xwindow,&g->WM_DELETE_WINDOW,1);
	g->xgraphic = XCreateGC(g->xdisplay,g->xwindow,0,0);
	/*Load a default VGA palette (DOSBox and other programs does this)*/
	g->xpalette = malloc(255*sizeof(paletteEntry));
	if(g->xpalette == NULL) { return -2; }
	for(i = 0; i < 64; i++) {
		g->xpalette[i].r = i;
		g->xpalette[i+64].g = 0;
		g->xpalette[i+64].b = 0;
	}
	for(i = 0; i < 64; i++) {
		g->xpalette[i+64].r = 0;
		g->xpalette[i+64].g = i;
		g->xpalette[i+64].b = 0;
	}
	for(i = 0; i < 64; i++) {
		g->xpalette[i+128].r = 0;
		g->xpalette[i+128].g = 0;
		g->xpalette[i+128].b = i;
	}
	for(i = 0; i < 64; i++) {
		g->xpalette[i+192].r = i;
		g->xpalette[i+192].g = i;
		g->xpalette[i+192].b = i;
	}
	XSetErrorHandler(leafErrorCatch);

signed int leafContextDestroy(leafContext * g) {
	if(videoBuffer != NULL) {
		free(videoBuffer);
	}
	if(g->name != NULL) {
		free(g->name);
	}
	return 0;
}
