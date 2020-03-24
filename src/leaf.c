#if !defined(__no_current_dir)
#include "leaf.h"
#endif

#if defined(__no_current_dir)
#include "leaf/src/leaf.h"
#endif

#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
signed int leafErrorCatch(Display * d, XErrorEvent * e) {
	fprintf(stderr,"X11 Error occoured\n");
	return 0;
}
#endif

void seedRandom(void) {
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	srand(time(NULL));
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned short *clock = (unsigned short *)0x0000046CL;
	srand(*clock);
#endif
	return;
}

signed int generateRandom(void) {
	return rand();
}

signed int leafGameCreate(leafGame * g) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	/*do absolutely nothing important (yet)*/
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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

#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
unsigned short toRgb(register unsigned short r, register unsigned short g, register unsigned short b) {
	return (r+(g<<8)+(b<<16));
}
#endif

signed int leafEventUpdate(leafGame *g, leafEvent * e) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	union REGS in,out;
	in.x.ax = 0x0B; int86(0x33,&in,&out);
	l->cx = (int16_t)out.x.cx;
	l->cy = (int16_t)out.x.dx;
	in.x.ax = 0x03; int86(0x33,&in,&out);
	l->eventStatus += (out.x.bx<<1); /*shift to leave space for key bit*/
	if(kbhit) { l->eventStatus |= 1; } /*set keypress bit*/
	else { l->eventStatus ^= 1; }
	l->key = getch();
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	/*g->colormap = XCreateColormap(g->xdisplay,g->xwindow,g->xvinfo.visual,256);*/
	return -1; /*let know the program we are running in linux*/
#endif
}

void plotPixel(leafGame * g, register unsigned char x, register unsigned char y, register unsigned char c) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char * video = (unsigned char * )0xA0000000L;
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
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

signed int readImageBitmapHeader(FILE * stream, bmpHeader * e) {
	unsigned long headerSize,wide,tall,planes,bitsPerPixel,compression,sizeOfImage;
	unsigned long xPixelsPerMeter,yPixelsPerMeter,numberOfColors;
	unsigned long importantColors,sizeOfFile,reserved,offset,mask[4];
	if(!stream) { /*check if file is open*/
		return -1; /*nope, bail out!*/
	}
	fread(e->type,sizeof(unsigned short),1,stream); /*read file header*/
	/*check that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) != 0 && strncmp((const char *)e->type,"BA",2) != 0
	&& strncmp((const char *)e->type,"IC",2) != 0 && strncmp((const char *)e->type,"PT",2) != 0
	&& strncmp((const char *)e->type,"CI",2) != 0 && strncmp((const char *)e->type,"CP",2) != 0) {
		return -1; /*invalid bitmap!*/
	}
	fread(&sizeOfFile,sizeof(unsigned long),1,stream);
	fread(&reserved,sizeof(unsigned long),1,stream); /*reserved has an actual mean in OS/2*/
	fread(&offset,sizeof(unsigned long),1,stream);
	fread(&headerSize,sizeof(unsigned long),1,stream);
	e->sizeOfFile = sizeOfFile; /*save everything in the struct pointer thing*/
	e->reserved = reserved;
	e->offset = offset;
	e->headerSize = headerSize;
	/*check the header size*/
	if(headerSize == 40) { /*Windows 3.x bitmap*/
		fread(&wide,sizeof(int32_t),1,stream);
		fread(&tall,sizeof(int32_t),1,stream);
		fread(&planes,sizeof(unsigned short),1,stream);
		fread(&bitsPerPixel,sizeof(unsigned short),1,stream);
		fread(&compression,sizeof(unsigned long),1,stream);
		fread(&sizeOfImage,sizeof(unsigned long),1,stream);
		fread(&xPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&yPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&numberOfColors,sizeof(unsigned long),1,stream);
		fread(&importantColors,sizeof(unsigned long),1,stream);
	} else if(headerSize == 12) { /*OS/2 1.x bitmap*/
		fread(&wide,sizeof(unsigned short),1,stream);
		fread(&tall,sizeof(unsigned short),1,stream);
		fread(&planes,sizeof(unsigned short),1,stream);
		fread(&bitsPerPixel,sizeof(unsigned short),1,stream);
	} else if(headerSize >= 56 && headerSize <= 64) { /*Windows 95 bitmap*/
		fread(&wide,sizeof(int32_t),1,stream);
		fread(&tall,sizeof(int32_t),1,stream);
		fread(&planes,sizeof(unsigned short),1,stream);
		fread(&bitsPerPixel,sizeof(unsigned short),1,stream);
		fread(&compression,sizeof(unsigned long),1,stream);
		fread(&sizeOfImage,sizeof(unsigned long),1,stream);
		fread(&xPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&yPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&numberOfColors,sizeof(unsigned long),1,stream);
		fread(&importantColors,sizeof(unsigned long),1,stream);
		fread(&mask[0],sizeof(unsigned long),1,stream);
		fread(&mask[1],sizeof(unsigned long),1,stream);
		fread(&mask[2],sizeof(unsigned long),1,stream);
		fread(&mask[3],sizeof(unsigned long),1,stream);
		/*inmediately place masks, on the struct*/
		e->mask[0] = mask[0];
		e->mask[1] = mask[1];
		e->mask[2] = mask[2];
		e->mask[3] = mask[3];
	} else {
		return -2;
	}
	e->planes = planes;
	e->bitsPerPixel = bitsPerPixel;
	e->compression = compression;
	e->sizeOfImage = sizeOfImage;
	e->xPixelsPerMeter = xPixelsPerMeter;
	e->yPixelsPerMeter = yPixelsPerMeter;
	e->numberOfColors = numberOfColors;
	e->importantColors = importantColors;
	/*check if bit's are valid*/
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 24:
		case 32:
			break;
		default:
			return -3;
	}
	e->wide = wide;
	e->tall = tall;
	if(planes != 1 || numberOfColors > 256) { /*we did something wrong!*/
		return -4;
	}
	return 0;
}

paletteEntry * readImageBitmapPalette(FILE *stream, bmpHeader * b) {
	unsigned short i;
	paletteEntry * pal;
	if(b->bitsPerPixel == 8 || b->bitsPerPixel == 4 || b->bitsPerPixel == 2
	|| b->bitsPerPixel == 1) { /*check if it really has palette...*/
		b->paletteEntries = (1<<(b->bitsPerPixel)); /*number of palette entries to read*/
		pal = (paletteEntry *)malloc(sizeof(paletteEntry)*b->paletteEntries);
		if(pal == NULL) { return NULL; }
		for(i = 0; i < b->paletteEntries; i++) { /*read the palette thing*/
			pal[i].b = fgetc(stream); /*bitmap has reverse RGB order for each entry*/
			pal[i].g = fgetc(stream);
			pal[i].r = fgetc(stream);
			if(b->headerSize >= 40) { /*later versions require 4 byte palette elements*/
				fgetc(stream); /*padding*/
			}
		}
		return (paletteEntry *)pal; /*return the allocated palette*/
	} else {
		return NULL; /*16 and 24 bit bitmaps dosent has palette*/
	}
}

unsigned char * readImageBitmapData(FILE *stream, bmpHeader * b) {
	unsigned long i,i2;
	unsigned short hold;
	unsigned char *data;
	if(b->tall == 0 || b->wide == 0) {
		return 0;
	}
	data = (unsigned char *)malloc((b->wide*b->tall)); /*allocate data for bitmap*/
	if(data == NULL) { return 0; }
	switch(b->compression) {
		case 0: /*no compression*/
			switch(b->bitsPerPixel) {
				case 8: /*256 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = hold;
						}
						if(((((b->wide*8)+7)>>3)&3)) { /*skip padding (dword)*/
							fseek(stream,SEEK_CUR,3-((((b->wide*8)+7)>>3)&3));
						}
					}
					break;
				case 4: /*16 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = (hold>>4)&0x0F;
							i2++;
							data[(i2+((b->tall-i)*b->wide))] = hold&0x0F;
						}
						if(((((b->wide*4)+7)>>3)&3)) {
							fseek(stream,SEEK_CUR,3-((((b->wide*4)+7)>>3)&3));
						}
					}
					break;
				case 2: /*4 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							if((i2&3) == 0) {
								fread(&hold,sizeof(unsigned char),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = (hold>>6)&3;
							hold = hold<<2;
						}
						if(((((b->wide*2)+7)>>3)&3)) {
							fseek(stream,SEEK_CUR,3-((((b->wide*2)+7)>>3)&3));
						}
					}
					break;
				case 1: /*2 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							if(!(i2&7)) {
								fread(&hold,sizeof(unsigned char),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = ((hold>>7)&1);
							hold <<= 1;
						}
						if(((((b->wide*1)+7)>>3)&3)) {
							fseek(stream,SEEK_CUR,3-((((b->wide*1)+7)>>3)&3));
						}
					}
					break;
				default:
					free(data); /*de allocate data if we have invalid setup*/
					return 0;
			}
			break;
		default:
			free(data); /*de allocate data if we have invalid setup*/
			return 0;
	}
	return (unsigned char * )data;
}
