#if defined(__TURBOC__) && !defined(__BORLANDC__)
#define __no_current_dir
#define __no_ctype_h
#endif

#if !defined(__no_current_dir)
#include "leaf.h"
#endif

#if defined(__no_current_dir)
#include "src/leaf.h"
#endif

void seedRandom(void) {
	srand(*clock);
	return;
}

signed int generateRandom(void) {
	return rand();
}

signed int leafGameCreate(leafGame * g) {
	g->videoConf = _video_auto; /*Default*/
	g->vwide = 320; g->vtall = 200;
	
	videoBuffer = (unsigned char *)malloc(64000);
	if(videoBuffer == NULL) { return -1; }
	return 0;
}

signed int leafGameEnd(leafGame * g) {
	if(videoBuffer != NULL) {
		free(videoBuffer);
	}
	return 0;
}

signed int leafSetVideo(leafGame * g) {
	unsigned char video;
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
}

unsigned int setVideo(unsigned char v) {
	in.h.al = v;
	in.h.ah = 0; /*set the video we want*/
	int86(0x10,&in,&out);
	return (signed int)0x03;
}

void plotPixel(register unsigned short x, register unsigned short y, register unsigned char c) {
	if(y >= 200 || x >= 320) { return; }
	videoBuffer[(y*320)+x] = c;
	return;
}

void plotLinearPixel(register unsigned short p,register unsigned char c) {
	videoBuffer[p] = c;
	return;
}

unsigned char fetchPixel(register unsigned short x,register unsigned short y) {
	return videoBuffer[(y*320)+x];
}

void plotLine(register signed short sx, register signed short sy, register signed short ex, register signed short ey, register unsigned char c) {
	signed short i,dx,dy,sdx,sdy,dxabs,dyabs,px,py,x,y;
	dx = ex-sx; dy = ey-sy;
	sdx = ((dx < 0) ? -1 : ((dx > 0) ? 1 : 0));
	sdy = ((dy < 0) ? -1 : ((dy > 0) ? 1 : 0));
	dxabs = abs(dx); dyabs = abs(dy);
	x = (dyabs>>1); y = (dxabs>>1);
	px = sx; py = sy;
	
	plotPixel(px,py,c);
	
	if(dxabs >= dyabs) {
		for(i = 0; i < dxabs; i++) {
			y += dyabs;
			if(y >= dxabs) {
				y -= dxabs; py += sdy;
			}
			px += sdx;
			plotPixel(px,py,c);
		}
	} else {
		for(i = 0; i < dyabs; i++) {
			x += dxabs;
			if(x >= dyabs) {
				x -= dyabs; px += sdx;
			}
			py += sdy;
			plotPixel(px,py,c);
		}
	}
	return;
}

void plotWireSquare(register signed short x1, register signed short y1, register signed short x2, register signed short y2, register unsigned char c) {
	plotLine(x1,y1,x2,y1,c);
	plotLine(x1,y1,x1,y2,c);
	plotLine(x1,y2,x2,y2,c);
	plotLine(x2,y2,x2,y1,c);
	return;
}

void plotWirePolygon(signed short * d, register unsigned short n, register unsigned char c) {
	unsigned short i;
	for(i = 0; i < n-1; i++) {
		plotLine(d[(i<<1)],d[((i<<1)+1)],d[((i<<1)+2)],d[((i<<1)+3)],c);
	}
	plotLine(d[(i<<1)],d[(i<<1)+1],d[0],d[1],c);
	return;
}

void setPalette(paletteEntry * p, register unsigned short n) {
	register unsigned short i;
	outportb(0x03c8,0); /*send to the vga registers that we are going to send palette data*/
	for(i = 0; i < n; i++) {
		outportb(0x03c9,(p[i].r>>2));
		outportb(0x03c9,(p[i].g>>2));
		outportb(0x03c9,(p[i].b>>2));
	}
	return;
}

void waitRetrace(void) {
	while((inportb(0x03DA)&8));
	while(!(inportb(0x03DA)&8));
	return;
}

void updateScreen(void) {
	waitRetrace(); /* Wait for VGA retrace */
	memcpy(video,videoBuffer,64000); /* Copy data to VGA */
	memset(videoBuffer,0,64000); /* Clear our buffer */
	return;
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
		pal = (paletteEntry *)malloc((sizeof(paletteEntry)*b->paletteEntries));
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

char initMouse(struct mouse * m) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	if((in.x.bx&2) != 0) { /*Two button mouse*/
        m->buttons = 2;
	}
	else if((in.x.bx&3) != 0) { /*Three button mouse*/
        m->buttons = 3;
	}
	else { /*Unknown buttons*/
        m->buttons = 0;
	}
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
}

void setMousePosition(unsigned short x, unsigned short y) {
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

void showMouse(void) {
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

void hideMouse(void) {
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

void getMouseStatus(struct mouse * m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

void getMouseMovement(struct mouse * m) {
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->mx = out.x.cx;
	m->my = out.x.dx;
	return;
}
