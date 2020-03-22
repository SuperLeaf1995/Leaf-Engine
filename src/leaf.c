#if !defined(__no_current_dir)
#include "leaf.h"
#endif

#if defined(__no_current_dir)
#include "leaf/src/leaf.h"
#endif

#if defined(__linux) || defined(linux)
signed int leafErrorCatch(Display * d, XErrorEvent * e) {
	fprintf(stderr,"X11 Error occoured\n");
	return 0;
}
#endif

void seedRandom(void) {
#if defined(__linux) || defined(linux)
	srand(time(NULL));
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned short *clock = (unsigned short *)0x0000046CL;
	srand(clock);
#endif
}

signed int generateRandom(void) {
	return rand();
}

signed int leafGameCreate(leafGame * g) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	/*do absolutely nothing important (yet)*/
#endif
#if defined(__linux) || defined(linux)
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
#endif
	g->vwide = 320; g->vtall = 200;
	return 0;
}

#if defined(__linux) || defined(linux)
unsigned short toRgb(register unsigned short r, register unsigned short g, register unsigned short b) {
	return (r+(g<<8)+(b<<16));
}
#endif

signed int leafEventUpdate(leafGame *g, leafEvent * e) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	union regs in,out;
	in.x.ax = 0x0B; int86(0x33,&in,&out);
	l->cx = (int16_t)out.x.cx;
	l->cy = (int16_t)out.x.dx;
	in.x.ax = 0x03; int86(0x33,&in,&out);
	l->eventStatus += (out.x.bx<<1); /*shift to leave space for key bit*/
	if(kbhit) { l->eventStatus |= 1; } /*set keypress bit*/
	else { l->eventStatus ^= 1; }
	l->key = getch();
#endif
#if defined(__linux) || defined(linux)
	register unsigned short i;
	register unsigned short i2;
	register unsigned short d;
	XNextEvent(g->xdisplay,&e->xevent);
	if((e->xevent.type == ClientMessage)&&((unsigned int)e->xevent.xclient.data.l[0] == g->WM_DELETE_WINDOW)) {
		e->ui = _exit_code; /*set UI as "exit_code"*/
	}
	/*Also update screen with the framebuffer*/
	if(g->video == NULL) { return -1; }
	else if(g->vtall == 0) { return -2; }
	else if(g->vwide == 0) { return -3; }
	for(i = 0; i < g->vwide; i++) {
		for(i2 = 0; i2 < g->vtall; i2++) {
			d = g->video[((i2*g->vwide)+i)]; /*point at the current pixel's address*/
			XSetForeground(g->xdisplay,g->xgraphic,toRgb(g->xpalette[d].r,g->xpalette[d].g,g->xpalette[d].b));
			XDrawPoint(g->xdisplay,g->xwindow,g->xgraphic,i,i2);
		}
	}
#endif
	return 0;
}

signed int leafGameDestroy(leafGame * g) {
#if defined(__linux) || defined(linux)
	XDestroyWindow(g->xdisplay,g->xwindow);
	XCloseDisplay(g->xdisplay); /*close xwindow*/
	if(g->video != NULL) { free(g->video); } /*free memory and avoid leaks*/
	if(g->xpalette != NULL) { free(g->xpalette); }
	XSetErrorHandler(NULL); /*restore error handler*/
#endif
	return 0;
}

signed int leafSetVideo(leafGame * g) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char video; union REGS in,out;
#endif
	if(g->videoConf == _video_auto) {
#if defined(__linux) || defined(linux)
		g->video = malloc((g->vwide*g->vtall)*sizeof(unsigned char));
		if(g->video == NULL) { return -1; } /*error on video allocation*/
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
		if(g->vwide == 320) {
			if(g->vtall == 200) {
				video = 0x13;
			}
		}
		in.h.al = video; in.h.ah = 0; /*set the video we want*/
		int86(0x10,&in,&out);
#endif
	}
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	return (int)video; /*return casted int video*/
#endif
#if defined(__linux) || defined(linux)
	/*g->colormap = XCreateColormap(g->xdisplay,g->xwindow,g->xvinfo.visual,256);*/
	return -1; /*let know the program we are running in linux*/
#endif
}

void plotPixel(leafGame * g, register unsigned char x, register unsigned char y, register unsigned char c) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char * video = (unsigned char * )0xA0000000L;
#endif
#if defined(__linux) || defined(linux)
	if(g->video != NULL && !(x > g->vwide) && !(y > g->vtall)) {
		g->video[(y*g->vwide)+x] = c; /*plot pixel in framebuffer*/
	}
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	video[(y*g->vwide)+x] = c;
#endif
	return;
}

void plotLinearPixel(leafGame * g, register unsigned short p,register unsigned char c) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char * video = (unsigned char * )0xA0000000L;
#endif
#if defined(__linux) || defined(linux)
	if(g->video != NULL && !(p > g->vwide*g->vtall)) {
		g->video[p] = c; /*plot pixel in framebuffer*/
	}
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	video[p] = c;
#endif
	return;
}

unsigned char fetchPixel(leafGame * g, register unsigned short x,register unsigned short y) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char * video = (unsigned char * )0xA0000000L;
#endif
#if defined(__linux) || defined(linux)
	if(g->video != NULL && !(x > g->vwide) && !(y > g->vtall)) {
		return g->video[(y*g->vwide)+x];
	} else {
		return 0;
	}
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	return video[(y*g->vwide)+x];
#endif
}

signed int setPalette(leafGame * g, paletteEntry * p, register unsigned short n) {
	register unsigned short i;
#if defined(__linux) || defined(linux)
	if(g->xpalette == NULL) { return -1; }
	for(i = 0; i < n; i++) {
		g->xpalette[i].r = p[i].r;
		g->xpalette[i].g = p[i].g;
		g->xpalette[i].b = p[i].b;
	}
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	outportb(0x03c8,0);
	for(i = 0; i < n; i++) {
		outportb(0x03c9,(p[i].r>>2));
		outportb(0x03c9,(p[i].g>>2));
		outportb(0x03c9,(p[i].b>>2));
	}
#endif
	return 0;
}
