#if !defined(__no_current_dir)
#include "leaf.h"
#endif

#if defined(__no_current_dir)
#include "leaf/src/leaf.h"
#endif

#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
signed int leafErrorCatch(Display * d, XErrorEvent * e) {
	fprintf(stderr,"X11 Error occoured\n");
	if(e == NULL) {
		return -1;
	}
	if(d == NULL) {
		return -2;
	}
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
	XSetErrorHandler(leafErrorCatch);
#endif
	g->videoConf = _video_auto;
	g->vwide = 320; g->vtall = 200;
	return 0;
}

#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
unsigned short toRgb(register unsigned short r, register unsigned short g, register unsigned short b) {
	return ((b)+(g<<8)+(r<<16));
}
#endif

signed int leafEventUpdate(leafGame *g, leafEvent * e) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	union REGS in,out;
	in.x.ax = 0x0B; int86(0x33,&in,&out);
	l->cx = (signed short)out.x.cx;
	l->cy = (signed short)out.x.dx;
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
	register unsigned short rgb;
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
			d = g->video[((i2*g->vwide)+i)]; /*get current pixel color*/
			rgb = toRgb(g->xpalette[d].r,g->xpalette[d].g,g->xpalette[d].b); /*color in emulated palette*/
			XSetForeground(g->xdisplay,g->xgraphic,rgb);
			XDrawPoint(g->xdisplay,g->xwindow,g->xgraphic,i,i2); /*draw it on the X11*/
			XFlush(g->xdisplay);
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
	unsigned char video;
	union REGS in,out;
	if(g->videoConf == _video_auto) {
		if(g->vwide == 320) {
			if(g->vtall == 200) {
				video = 0x13;
			}
		}
		in.h.al = video; in.h.ah = 0; /*set the video we want*/
		int86(0x10,&in,&out);
	}
	return (signed int)video; /*return casted signed int video*/
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	if(g->videoConf == _video_auto) {
		g->video = malloc((g->vwide*g->vtall)*sizeof(unsigned char));
		if(g->video == NULL) { /*error on video allocation*/
			return VideoErrorOnSet;
		}
	}
	return VideoUsesLinux; /*let know the program we are running in linux*/
#endif
}

void plotPixel(leafGame * g, register unsigned short x, register unsigned short y, register unsigned char c) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char * video = (unsigned char * )0xA0000000L;
	video[(y*g->vwide)+x] = c;
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	if(g->video != NULL && !(x > g->vwide) && !(y > g->vtall)) {
		g->video[(y*g->vwide)+x] = c; /*plot pixel in framebuffer*/
	}
#endif
	return;
}

void plotLinearPixel(leafGame * g, register unsigned short p,register unsigned char c) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	unsigned char * video = (unsigned char * )0xA0000000L;
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	if(g->video != NULL && !(p > (g->vwide*g->vtall))) {
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
	outportb(0x03c8,0); /*send to the vga registers that we are going to send palette data*/
	for(i = 0; i < n; i++) {
		outportb(0x03c9,(p[i].r>>2));
		outportb(0x03c9,(p[i].g>>2));
		outportb(0x03c9,(p[i].b>>2));
	}
#endif
	return 0;
}

signed int readImageBitmapHeader(FILE * stream, bmpHeader * e) {
	unsigned long sizeOfFile;
	unsigned long reserved;
	unsigned long offset;
	unsigned long headerSize;
	signed long wide;
	signed long tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned long compression;
	unsigned long sizeOfImage;
	unsigned long xPixelsPerMeter;
	unsigned long yPixelsPerMeter;
	unsigned long numberOfColors;
	unsigned long importantColors;
	unsigned long mask[4];
	if(!stream) { /*check if file is open*/
		return BitmapErrorFile; /*nope, bail out!*/
	}
	fread(e->type,sizeof(unsigned short),1,stream); /*read file header*/
	/*check that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) != 0 && strncmp((const char *)e->type,"BA",2) != 0
	&& strncmp((const char *)e->type,"IC",2) != 0 && strncmp((const char *)e->type,"PT",2) != 0
	&& strncmp((const char *)e->type,"CI",2) != 0 && strncmp((const char *)e->type,"CP",2) != 0) {
		return BitmapErrorSignature; /*invalid bitmap!*/
	}
	if(fread(&sizeOfFile,sizeof(unsigned long),1,stream) != 1) {
		return BitmapErrorFileReadSizeOfFile;
	}
	if(fread(&reserved,sizeof(unsigned long),1,stream) != 1) { /*reserved has an actual mean in OS/2*/
		return BitmapErrorFileReadReserved;
	}
	fread(&offset,sizeof(unsigned long),1,stream);
	fread(&headerSize,sizeof(unsigned long),1,stream);
	/*check the header size*/
	if(headerSize == 40) { /*Windows 3.x bitmap*/
		fread(&wide,sizeof(signed long),1,stream);
		fread(&tall,sizeof(signed long),1,stream);
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
		fread(&wide,sizeof(signed long),1,stream);
		fread(&tall,sizeof(signed long),1,stream);
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
		e->mask[0] = mask[0]; /*inmediately place masks, on the struct*/
		e->mask[1] = mask[1];
		e->mask[2] = mask[2];
		e->mask[3] = mask[3];
	} else {
		return BitmapErrorInvalidHeader;
	}
	/*check if bit's are valid*/
	if((bitsPerPixel != 1)
	&& (bitsPerPixel != 2)
	&& (bitsPerPixel != 4)
	&& (bitsPerPixel != 8)
	&& (bitsPerPixel != 16)
	&& (bitsPerPixel != 24)
	&& (bitsPerPixel != 32)) {
		return BitmapErrorBpp;
	}
	if(planes != 1) { /*we did something wrong!*/
		return BitmapErrorWrongPlanes;
	}
	if(numberOfColors > 256) {
		return BitmapErrorInvalidColorsOutOfRange;
	}
	e->planes = planes; /*save everything in the struct pointer thing*/
	e->bitsPerPixel = bitsPerPixel;
	e->compression = compression;
	e->sizeOfImage = sizeOfImage;
	e->xPixelsPerMeter = xPixelsPerMeter;
	e->yPixelsPerMeter = yPixelsPerMeter;
	e->numberOfColors = numberOfColors;
	e->importantColors = importantColors;
	e->wide = wide;
	e->tall = tall;
	e->sizeOfFile = sizeOfFile;
	e->reserved = reserved;
	e->offset = offset;
	e->headerSize = headerSize;
	return BitmapSucess;
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
	unsigned char hold;
	unsigned char *data;
	if((signed long)b->tall <= 0 || (signed long)b->wide <= 0) {
		return NULL;
	}
	data = (unsigned char *)malloc((b->wide*b->tall)); /*allocate data for bitmap*/
	if(data == NULL) {
		return NULL;
	}
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
							if(!(i2&3)) {
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
					return NULL;
			}
			break;
		default:
			free(data);
			return NULL;
	}
	return (unsigned char * )data;
}

signed int  readImagePcxHeader(FILE * stream, pcxHeader * p) {
	unsigned char type,version,compression,bitsPerPixel,reserved,planes;
	unsigned char paletteType,horizontalScreenSize,verticalScreenSize;
	unsigned short xStart,yStart,xEnd,yEnd,horizontalResolution,verticalResolution,bytesPerLine;
	unsigned char i; /*iterator*/
	unsigned char *reserved2; /*54/58 bytes*/
	fread(&type,sizeof(unsigned char),1,stream);
	if(type != 10) { return -1; } /*pcx specification states that 10h is signature*/
	fread(&version,sizeof(unsigned char),1,stream);
	if(version > 5) { return -2; } /*pcx hasn't any version above 3.0 (0x05)*/
	fread(&compression,sizeof(unsigned char),1,stream);
	if(compression != 1) { return -3; } /*pcx is rle-only file*/
	fread(&bitsPerPixel,sizeof(unsigned char),1,stream);
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
			break;
		default:
			return -4; /*not valid!*/
	}
	fread(&xStart,sizeof(unsigned short),1,stream); /*useless stuff, but useful*/
	fread(&yStart,sizeof(unsigned short),1,stream); /*if the game has some sort of magic*/
	fread(&xEnd,sizeof(unsigned short),1,stream); /*stuff that makes pcx files to fly*/
	fread(&yEnd,sizeof(unsigned short),1,stream);
	fread(&horizontalResolution,sizeof(unsigned short),1,stream);
	fread(&verticalResolution,sizeof(unsigned short),1,stream);
	/*read the EGA palette for once*/
	for(i = 0; i < 16; i++) {
		p->egaPalette[i].r = fgetc(stream);
		p->egaPalette[i].g = fgetc(stream);
		p->egaPalette[i].b = fgetc(stream);
	}
	fread(&reserved,sizeof(unsigned char),1,stream);
	if(reserved != 0) { return -6; } /*reserved is always 0*/
	fread(&planes,sizeof(unsigned char),1,stream);
	fread(&bytesPerLine,sizeof(unsigned short),1,stream);
	fread(&paletteType,sizeof(unsigned short),1,stream);
	fread(&horizontalScreenSize,sizeof(unsigned short),1,stream);
	fread(&verticalScreenSize,sizeof(unsigned short),1,stream);
	reserved2 = (unsigned char *)malloc(54);
	if(reserved2 == NULL) { return -7; }
	fread((unsigned char *)reserved2,sizeof(unsigned char),54,stream);
	/*save the stuff in the struct if all went ok*/
	p->type = type;
	p->version = version;
	p->compression = compression;
	p->bitsPerPixel = bitsPerPixel;
	p->bytesPerLine = bytesPerLine;
	p->xStart = xStart;
	p->yStart = yStart;
	p->xEnd = xEnd;
	p->yEnd = yEnd;
	p->horizontalResolution = horizontalResolution;
	p->verticalResolution = verticalResolution;
	p->reserved = reserved;
	p->reserved2 = reserved2;
	p->planes = planes;
	return 0;
}

unsigned char *  readImagePcxData(FILE * stream, pcxHeader * p) {
	register unsigned char rLen,tmp,val;
	register unsigned long index,dataSize,total;
	static unsigned char * data;
	dataSize = (p->xEnd+1)*(p->yEnd+1);
	index = 0; total = 0;
	data = (unsigned char *)malloc(dataSize);
	if(data == NULL) { return 0; }
	while(index < dataSize) {
		tmp = fgetc(stream);
		if((tmp&0xC0) == 0xC0) { /*is it a 2 byte value*/
			rLen = tmp&0x3F; val = fgetc(stream);
		} else { /*1 byte value*/
			rLen = 1; val = tmp;
		}
		for(total += rLen; rLen&&index < dataSize; rLen--, index++) { /*decompress the data*/
			data[index] = val;
		}
	}
	return (unsigned char *)data;
}

/*TODO: Fix this function*/
paletteEntry *  readImagePcxVgaPalette(FILE * stream) {
	register signed short vgaPaletteChecker;
	register unsigned short i;
	paletteEntry * pal;
	vgaPaletteChecker = fgetc(stream); /*is last byte 0x0C or EOF?*/
	if(vgaPaletteChecker != 0x0C || vgaPaletteChecker == EOF) { return NULL; }
	pal = (paletteEntry *)malloc(256*sizeof(paletteEntry));
	if(pal == NULL) { return NULL; } /*allocation error*/
	/*read rgb components into the palette entry*/
	for(i = 0; i < 256; i++) {
		pal[i].r = fgetc(stream);
		pal[i].g = fgetc(stream);
		pal[i].b = fgetc(stream);
	}
	return (paletteEntry *)pal;
}
