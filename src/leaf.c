/*
 * Leaf-Engine Base code
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "leaf.h"

void seedRandom(void) {
#if defined(__GNUC__)
	srand(time(NULL));
#elif defined(__TURBOC__) && !defined(__BORLANDC__)
	srand(*biosClock);
#endif
	return;
}

signed int generateRandom(void) {
	return rand();
}

signed int leafContextCreate(leafContext * g) {
#if defined(__DJGPP__)
	if(__djgpp_nearptr_enable() == 0) {
		fprintf(stderr,"Cannot enable nearptr\n");
		exit(-1);
	}
#endif /* __DJGPP__ */
	leafCurrentCtx = g;
	g->vwide = 320; g->vtall = 200;
	videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(videoBuffer == NULL) {
		return -1;
	}
	g->name = NULL;
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	return 0;
}

signed int leafContextDestroy(leafContext * g) {
	if(videoBuffer != NULL) {
		free(videoBuffer);
	}
	if(g->name != NULL) {
		free(g->name);
	}
	leafCurrentCtx = NULL; /*Destroy Context*/
#if defined(__DJGPP__)
	__djgpp_nearptr_disable();
#endif
	return 0;
}

unsigned int setVideo(unsigned char v) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.h.al = v;
	in.h.ah = 0; /*set the video we want*/
	int86(0x10,&in,&out);
	
	leafCurrentCtx->vwide = vtable[v][0];
	leafCurrentCtx->vtall = vtable[v][1];
	leafCurrentCtx->vvideo = vtable[v][2];
	if(leafCurrentCtx->vvideo == __ega || leafCurrentCtx->vvideo == __vga) {
		video = (unsigned char *)0xA0000000L;
	} else if(leafCurrentCtx->vvideo == __cga) {
		video = (unsigned char *)0xB8000000L;
	}
#elif defined(__APPLE2__) /*In APPLE2 there is only one mode used and it's HIGHRES*/
	poke(0xC050,0); /*Enter graphics mode*/
	poke(0xC052,0); /*Enter fullscreen*/
	poke(0xC057,0); /*Enter highres mode*/
	poke(0xC054,0); /*Select PAGE1*/
#elif defined(__linux__)
#endif
	return (unsigned int)v;
}

void plotPixel(register unsigned short x, register unsigned short y, register unsigned char c) {
	if(y >= leafCurrentCtx->vtall || x >= leafCurrentCtx->vwide) { return; }
	videoBuffer[(y*leafCurrentCtx->vwide)+x] = c;
	return;
}

void plotLinearPixel(register unsigned short p,register unsigned char c) {
	videoBuffer[p] = c;
	return;
}

unsigned char fetchPixel(register unsigned short x,register unsigned short y) {
	return videoBuffer[(y*leafCurrentCtx->vwide)+x];
}

unsigned char fetchLinearPixel(register unsigned short p) {
	return videoBuffer[p];
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
	plotLine(x1,y1,x2,y1,c); plotLine(x1,y1,x1,y2,c);
	plotLine(x1,y2,x2,y2,c); plotLine(x2,y2,x2,y1,c);
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
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	register unsigned short i;
	if(leafCurrentCtx->vvideo == __vga) {
		outp(0x03C8,(unsigned char)0); /*send to the vga registers that we are going to send palette data*/
		for(i = 0; i < n; i++) {
			outp(0x03C9,(unsigned char)(p[i].r>>2));
			outp(0x03C9,(unsigned char)(p[i].g>>2));
			outp(0x03C9,(unsigned char)(p[i].b>>2));
		}
	} else if(leafCurrentCtx->vvideo == __ega) {

	} else if(leafCurrentCtx->vvideo == __cga) {

	}
#endif /*__MSDOS__*/
	return;
}

void waitRetrace(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	while((inp(0x03DA)&8));
	while(!(inp(0x03DA)&8));
#endif
	return;
}

void updateScreen(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	unsigned long i;
	size_t x; size_t y; size_t offs;
	unsigned char * cgaSelect[2] = {
		(unsigned char *)0xB8000000L,
		(unsigned char *)0xBA000000L
	};
	unsigned char bitMask; 
	waitRetrace(); /* Wait for VGA retrace */
	if(leafCurrentCtx->vvideo == __vga) {
		/*in VGA simply copy it to the plain VGA memory*/
		memcpy(video,videoBuffer,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Copy data to VGA */
	} else if(leafCurrentCtx->vvideo == __ega) {
		/*TODO: Add working EGA code*/
		in.h.ah = 0x0C;
		for(i = 0; i < (leafCurrentCtx->vwide*leafCurrentCtx->vtall); i++) {
			in.x.dx = (i/leafCurrentCtx->vwide);
			in.x.cx = (i%leafCurrentCtx->vwide);
			in.h.al = videoBuffer[i];
			int86(0x10,&in,&out);
		}
	} else if(leafCurrentCtx->vvideo == __cga) {
		for(i = 0; i < (leafCurrentCtx->vwide*leafCurrentCtx->vtall); i++) {
			x = (i%320); y = (i/320);
			bitMask = 0x80>>(x%8);
			offs = ((y>>1)*((leafCurrentCtx->vwide<<1)>>3)+(x>>3));
			if(videoBuffer[i]) { *(cgaSelect[y%2]+offs) |= bitMask; }
			else { *(cgaSelect[y%2]+offs) &= ~bitMask; }
		}
	}
	memset(videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
#endif
	return;
}

void drawSprite(unsigned char * data, unsigned short x, register unsigned short y, register unsigned short sx, register unsigned short sy) {
	unsigned short i;
	unsigned short i2;
	for(i = 0; i < sx; i++) {
		for(i2 = 0; i2 < sy; i2++) {
			plotPixel(x+i,y+i2,data[(i2*sx)+i]);
		}
	}
	return;
}

void drawTiledSprite(unsigned char * data, unsigned short x, register unsigned short y, register unsigned short sx, register unsigned short sy, register unsigned short ix, register unsigned short iy, register unsigned short tx, register unsigned short ty) {
	unsigned short i;
	unsigned short i2;
	unsigned short i3;
	unsigned short i4;
	for(i = 0; i < tx; i++) {
		for(i2 = 0; i2 < ty; i2++) {
			i4 = iy; i3 = ix;
			plotPixel((x+i),(y+i2),data[(((i4*tx)+i2)*sy)+((i3)*ty)+i]);
		}
	}
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
		return -1; /*nope, bail out!*/
	}
	fread(e->type,sizeof(unsigned short),1,stream); /*read file header*/
	/*check that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) != 0
	&& strncmp((const char *)e->type,"BA",2) != 0
	&& strncmp((const char *)e->type,"IC",2) != 0
	&& strncmp((const char *)e->type,"PT",2) != 0
	&& strncmp((const char *)e->type,"CI",2) != 0
	&& strncmp((const char *)e->type,"CP",2) != 0) {
		return -2; /*invalid bitmap!*/
	}
	if(fread(&sizeOfFile,sizeof(unsigned long),1,stream) != 1) {
		return -3;
	}
	if(fread(&reserved,sizeof(unsigned long),1,stream) != 1) { /*reserved has an actual mean in OS/2*/
		return -4;
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
		return -5;
	}
	/*check if bit's are valid*/
	if((bitsPerPixel != 1)
	&& (bitsPerPixel != 2)
	&& (bitsPerPixel != 4)
	&& (bitsPerPixel != 8)
	&& (bitsPerPixel != 16)
	&& (bitsPerPixel != 24)
	&& (bitsPerPixel != 32)) {
		return -6;
	}
	if(planes != 1) { /*we did something wrong!*/
		return -7;
	}
	if(numberOfColors > 256) {
		return -8;
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
	return 0;
}

paletteEntry * readImageBitmapPalette(FILE *stream, bmpHeader * b) {
	unsigned short i;
	paletteEntry * pal;
	if(b->bitsPerPixel == 8 || b->bitsPerPixel == 4 || b->bitsPerPixel == 2
	|| b->bitsPerPixel == 1) { /*check if it really has palette...*/
		b->paletteEntries = (1<<(b->bitsPerPixel)); /*number of palette entries to read*/
		pal = (paletteEntry *)malloc((sizeof(paletteEntry)*b->paletteEntries));
		if(pal == NULL) { return NULL; }
		for(i = 0; i < b->paletteEntries-3; i++) { /*read the palette thing*/
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

/**
@brief Reads header of the PCX

@param stream File stream of the PCX file
@param p PCXHeader used to store metadata
*/
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

/**
@brief Reads data of the PCX

@param stream File stream of the PCX file
@param p Used for getting correct data
*/
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
/**
@brief Reads the VGA palette of the PCX

@param stream File stream of the PCX file
*/
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

/**
@brief Initializes mouse driver/cursor

@param m Mouse structure
*/
char initMouse(struct mouse * m) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
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
#endif
}

/**
@brief Sets position of the mouse to given parameters

@param x X Coordinates
@param y Y Coordinates
*/
void setMousePosition(register unsigned short x, register unsigned short y) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
#endif
	return;
}

/**
@brief Shows mouse
*/
void showMouse(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
#endif
	return;
}

/**
@brief Hides mouse
*/
void hideMouse(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
#endif
	return;
}

/**
@brief Gets status of buttons and current position

@param m Mouse structure
*/
void getMouseStatus(struct mouse * m) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
#endif
	return;
}

/**
@brief Gets mouse movement

@param m Mouse structure
*/
void getMouseMovement(struct mouse * m) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->mx = out.x.cx;
	m->my = out.x.dx;
#endif
	return;
}

/**
@brief Saves data to file

@param fileName Name of the file
@param data Pointer to the place of data to put on file
@param n Size of data to write
*/
signed char saveLeafDataFile(const char * fileName, void * data, size_t n) {
	FILE * s;
	s = fopen(fileName,"wb");
	if(!s) {
		return -1;
	}
	fwrite((unsigned char *)data,sizeof(unsigned char),n,s);
	fclose(s);
	return 0;
}

/**
@brief Loads data from file

@param fileName Name of the file
@param data Pointer to the place to put data on
@param n Size of data to read
*/
signed char loadLeafDataFile(const char * fileName, void * data, size_t n) {
	FILE * s;
	s = fopen(fileName,"rb");
	if(!s) {
		return -1;
	}
	fread((unsigned char *)data,sizeof(unsigned char),n,s);
	fclose(s);
	return 0;
}

/**
@brief Opens log file

@param stream Stream/File to open
@param name Filename of the file to open
*/
signed char openLogFile(FILE * stream, const char * name) {
	stream = fopen(name,"a+t");
	if(!stream) {
		return -1;
	}
	return 0;
}

/**
@brief Appends a log entry into the log file

@param stream Stream/File to close
@param entry Contents of the logging entry
*/
signed char appendLogFile(FILE * stream, const char * entry) {
	if(!stream) {
		return -1;
	}
	fprintf(stream,"%s\n",entry);
	return 0;
}

/**
@brief Closes log file

@param stream Stream/File to close
*/
signed char closeLogFile(FILE * stream) {
	if(stream) {
		fclose(stream);
	}
	return 0;
}

/**
@brief Initializes everything needed for sound-support
*/
signed char initSound(void) {
#if defined(OPENAL)
	/*Open default AL device*/
	leafCurrentCtx->alDev = alcOpenDevice(NULL); /*Choose default device (NULL) for playback*/
	if(!leafCurrentCtx->alDev) {
		return -1;
	}
	/*Create a OpenAL context*/
	leafCurrentCtx->alCtx = alcCreateContext(leafCurrentCtx->alDev,NULL);
	if(!alcMakeContextCurrent(leafCurrentCtx->alCtx)) {
		return -2;
	}
	/*Create a main audio object*/
	alGenSources((ALuint)1,&leafCurrentCtx->alSoundSrc);
	alSourcef(leafCurrentCtx->alSoundSrc,AL_PITCH,1);
	alSourcef(leafCurrentCtx->alSoundSrc,AL_GAIN,1);
	alSource3f(leafCurrentCtx->alSoundSrc,AL_POSITION,0,0,0);
	alSource3f(leafCurrentCtx->alSoundSrc,AL_VELOCITY,0,0,0);
	alSourcei(leafCurrentCtx->alSoundSrc,AL_LOOPING,AL_FALSE);
#endif
	return 0;
}

/**
@brief Plays a sound with the given freq

@param freq Is the frequency for the sound
*/
void playSound(unsigned long freq) {
#if defined(OPENAL)
	
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	register unsigned long cot; /*countdown value*/
	register unsigned char tmp; /*temporal value stuff*/
	cot = (unsigned long)(1193180/freq);
	outp(0x43,0xb6); /*say to the speaker we are going to send data*/
	outp(0x42,(unsigned char)cot); /*output low byte of countdown*/
	outp(0x42,(unsigned char)cot>>8); /*and then the high byte*/
	tmp = inp(0x61); /*set PIT2 timer*/
	if(tmp != (tmp|3)) {
		outp(0x61,tmp|3);
	}
#endif
	return;
}

/**
@brief Stops all playing sounds
*/
void stopSound(void) {
#if defined(OPENAL)

#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	register unsigned char tmp = inp(0x61)&0xfc; /*shutdown speaker command*/
	outp(0x61,tmp);
#endif
	return;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
