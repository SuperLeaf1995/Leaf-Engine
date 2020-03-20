#include "src\lengine.h"

/* ENGINE.C
 * Base stuff, like functions used for the internal of everything*/

#define isSigned(x) (x < 0) ? -1 : (x == 0) ? 0 : -1

/* VIDEO.C
 * Header for video stuff and routines for plotting pixels and stuff*/

/*VGA functions*/
void vgaPlotPixel(register uint16_t x,register uint16_t y,register uint8_t color) {
	videoMemory[(y<<8)+(y<<6)+x] = color;
	return;
}
void vgaPlotLinearPixel(register uint16_t pos,register uint8_t color) {
	videoMemory[pos] = color;
	return;
}
uint8_t vgaFetchPixel(register uint16_t x,register uint16_t y) {
	return videoMemory[(y<<8)+(y<<6)+x];
}

/* VIDEO.C
 * Includes video-mode independent functions, like plotting a line and
 * getting video mode, setting video mode, etc*/

uint8_t setVideo(register uint8_t video) {
	static uint8_t oldVideo;
	in.h.al = 0x0f;
	int86(0x10,&in,&out); /*get old video*/
	oldVideo = out.h.al; /*save old video*/
	in.h.al = video; /*now set video that we want*/
	in.h.ah = 0;
	int86(0x10,&in,&out); /*issue interrupt*/
	return oldVideo; /*return the old video*/
}

uint8_t getVideo(void) {
	in.h.al = 0x0f;
	int86(0x10,&in,&out); /*issue interrupt*/
	return out.h.al; /*return video we got*/
}

void setPalette(paletteEntry *pal, register uint16_t n) {
	register uint16_t i;
	outportb(0x03c8,0);
	for(i = 0; i < n; i++) {
		outportb(0x03c9,(pal[i].red>>2));
		outportb(0x03c9,(pal[i].green>>2));
		outportb(0x03c9,(pal[i].blue>>2));
	}
	return;
}

void plotLine(register int16_t fx, register int16_t fy, register int16_t tx, register int16_t ty, register uint8_t color) {
	static int16_t dx,dy,xi,yi,d,x,y,i;
	if(abs(ty-fy) < abs(tx-fx)) { /*if ty-fy < tx-fx the line is more horizontal*/
		if(fx > tx) { /*starting X is higher than ending X*/
			dx=fx-tx;
			dy=fy-ty;
			yi=1;
			if(dy < 0) {
				yi = -1;
				dy = -dy;
			}
			d = 2*dy - dx;
			y = fy;
			for(i = fx; !(i > tx); i++) {
				plotPixel(i,y,color);
				if(d > 0) {
					y = y+yi;
					d = d -2*dx;
				}
				d = d +2*dy;
			}
		}
		else {
			dx=tx-fx;
			dy=ty-fy;
			yi=1;
			if(dy < 0) {
				yi = -1;
				dy = -dy;
			}
			d = 2*dy - dx;
			y = fy;
			for(i = fx; !(i > tx); i++) {
				plotPixel(i,y,color);
				if(d > 0) {
					y = y+yi;
					d = d -2*dx;
				}
				d = d +2*dy;
			}
		}
	}
	else {
		if(fy > ty) { /*starting Y is higher than ending Y*/
			dx = fx-tx;
			dy = fy-ty;
			xi = 1;
			if(dx < 0) {
				xi = -1;
				dx = -dx;
			}
			d = 2*dx-dy;
			x=fx;
			for(i = fy; !(i > ty); i++) {
				plotPixel(x,i,color);
				if(d > 0) {
					x = x+xi;
					d = d-2*dy;
				}
				d=d+2*dx;
			}
		}
		else {
			dx = tx-fx;
			dy = ty-fy;
			xi = 1;
			if(dx < 0) {
				xi = -1;
				dx = -dx;
			}
			d = 2*dx-dy;
			x=fx;
			for(i = fy; !(i > ty); i++) {
				plotPixel(x,i,color);
				if(d > 0) {
					x = x+xi;
					d = d-2*dy;
				}
				d=d+2*dx;
			}
		}
	}
	return;
}

void plotPoly(int32_t n, int32_t *v, uint8_t color) {
	int32_t i;
	for(i = 0; i < n-1; i++) {
		/*iterate trough all vertices*/
  		plotLine(v[(i<<1)+0],v[(i<<1)+1],v[(i<<1)+2],v[(i<<1)+3],color);
	}
	plotLine(v[0],v[1],v[(n<<1)-2],v[(n<<1)-1],color);
}

/* PCSPEAK.C
 * C file for PC Speaker functions
 */

void soundPlay(uint32_t x) {
	uint32_t cot; /*countdown value*/
	uint8_t tmp; /*temporal value stuff*/
	cot = (uint32_t)(1193180/x);
	outportb(0x43,0xb6); /*say to the speaker we are going to send data*/
	outportb(0x42,(uint8_t)cot); /*output low byte of countdown*/
	outportb(0x42,(uint8_t)cot>>8); /*and then the high byte*/
	tmp = inportb(0x61); /*set pit2 timer*/
	if(tmp != (tmp|3)) {
		outportb(0x61,tmp|3);
	}
	return;
}

void soundStop(void) {
	uint8_t tmp = inportb(0x61)&0xfc; /*shutdown speaker command*/
	outportb(0x61,tmp);
	return;
}

int8_t soundPlayRawSoundFile(FILE *stream) {
	int16_t i = fgetc(stream);
	if(i == EOF) {
		fseek(stream,SEEK_SET,0); /*rewind file*/
	}
	soundPlay(i*30); /*TODO: know why multiplying by 30 makes a nice noise*/
	return 0;
}

/* FILE.C
 * Miscellaneous file functions for handling files better*/

void fskip(FILE *stream, uint64_t n) {
	static uint64_t i;
	for(i = 0; i < (n+1); i++) {
		fgetc(stream); /*Skip characters*/
	}
	return;
}

/* MOUSE.C
 * Contains mouse functions (MOUSE.COM driver stuff, useful for clicking
 * actions and stuff that involves mouse interaction*/

#define isMouseOnPosition(x,y,m) (m.x == x) ? ((m.y == y) ? 1 : 0) : 0 /*0 if false, 1 if true*/

#if !defined (__FAST__)
void setMouseStatus(uint8_t status) {
	in.x.ax = status; /*See MOUSE_STATUS_HIDE and MOUSE_STATUS_SHOW for more information*/
	int86(0x33,&in,&out); /*issue an interrupt*/
	return;
}
#endif

int8_t initMouse(struct mouse *m) {
	in.x.ax = 0x00; /*ax 0 and bx 0 = mouse.com initializes mouse*/
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	/*set the correct button stuff*/
	if((in.x.bx&2) != 0) {
		m->buttons = 2;
	} else if((in.x.bx&2) != 0) {
		m->buttons = 3; /*posibly some weird mouse*/
	} else {
		m->buttons = 0; /*unknown!*/
	}
	if(out.x.ax != 0) { /*if mouse was initialized...*/
		setMouseStatus(MOUSE_STATUS_SHOW); /*...automaticaly show mouse*/
	}
	return (out.x.ax != 0) ? 0 : -1; /*if the mouse was initialized return 0, else return -1*/
}

#if !defined (__FAST__)
void setMousePosition(uint16_t x,uint16_t y) {
	in.x.ax = 4; /*ax: 4, set mouse coordinates*/
	in.x.cx = x; /*set the coordinates and stuff*/
	in.x.dx = y;
	int86(0x33,&in,&out); /*issue the interrupt*/
	return;
}
#endif

void getMouseStatus(struct mouse *m) {
	in.x.ax = 0x03; /*command 0x03, get mouse info*/
	int86(0x33,&in,&out); /*issue the interrupt*/
	m->x = out.x.cx; /*x coords*/
	m->y = out.x.dx; /*y coords*/
	m->buttonLeft = (out.x.bx & 1); /*while it is not equal 0, its*/
	m->buttonRight = (out.x.bx & 2); /*HOLD/PRESSED, else its RELEASED*/
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

void updateMouse(struct mouse *m) {
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->x += (int16_t)out.x.cx; /*add the movement!*/
	m->y += (int16_t)out.x.dx;
	return;
}

/* BITMAP.C
 * Used to read and write Microsoft Bitmaps (Read/Write) and OS/2 bitmaps (Read)
 * */

int8_t  readImageBitmapHeader(FILE *stream, bmpHeader *e) {
	static uint32_t headerSize,wide,tall,planes,bitsPerPixel,compression,sizeOfImage;
	static uint32_t xPixelsPerMeter,yPixelsPerMeter,numberOfColors;
	static uint32_t importantColors,sizeOfFile,reserved,offset,mask[4];
	if(!stream) { /*check if file is open*/
		return -1; /*nope, bail out!*/
	}
	fread(e->type,sizeof(uint16_t),1,stream); /*read file header*/
	/*check that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) != 0 && strncmp((const char *)e->type,"BA",2) != 0
	&& strncmp((const char *)e->type,"IC",2) != 0 && strncmp((const char *)e->type,"PT",2) != 0
	&& strncmp((const char *)e->type,"CI",2) != 0 && strncmp((const char *)e->type,"CP",2) != 0) {
		return -1; /*invalid bitmap!*/
	}
	fread(&sizeOfFile,sizeof(uint32_t),1,stream);
	fread(&reserved,sizeof(uint32_t),1,stream); /*reserved has an actual mean in OS/2*/
	fread(&offset,sizeof(uint32_t),1,stream);
	fread(&headerSize,sizeof(uint32_t),1,stream);
	e->sizeOfFile = sizeOfFile; /*save everything in the struct pointer thing*/
	e->reserved = reserved;
	e->offset = offset;
	e->headerSize = headerSize;
	/*check the header size*/
	if(headerSize == 40) { /*Windows 3.x bitmap*/
		fread(&wide,sizeof(int32_t),1,stream);
		fread(&tall,sizeof(int32_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
		fread(&compression,sizeof(uint32_t),1,stream);
		fread(&sizeOfImage,sizeof(uint32_t),1,stream);
		fread(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&numberOfColors,sizeof(uint32_t),1,stream);
		fread(&importantColors,sizeof(uint32_t),1,stream);
	} else if(headerSize == 12) { /*OS/2 1.x bitmap*/
		fread(&wide,sizeof(uint16_t),1,stream);
		fread(&tall,sizeof(uint16_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
	} else if(headerSize >= 56 && headerSize <= 64) { /*Windows 95 bitmap*/
		fread(&wide,sizeof(int32_t),1,stream);
		fread(&tall,sizeof(int32_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
		fread(&compression,sizeof(uint32_t),1,stream);
		fread(&sizeOfImage,sizeof(uint32_t),1,stream);
		fread(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&numberOfColors,sizeof(uint32_t),1,stream);
		fread(&importantColors,sizeof(uint32_t),1,stream);
		fread(&mask[0],sizeof(uint32_t),1,stream);
		fread(&mask[1],sizeof(uint32_t),1,stream);
		fread(&mask[2],sizeof(uint32_t),1,stream);
		fread(&mask[3],sizeof(uint32_t),1,stream);
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

paletteEntry *readImageBitmapPalette(FILE *stream, bmpHeader *b) {
	uint16_t i;
	paletteEntry * pal;
	if(b->bitsPerPixel == 8 || b->bitsPerPixel == 4 || b->bitsPerPixel == 2
	|| b->bitsPerPixel == 1) { /*check if it really has palette...*/
		b->paletteEntries = (1<<(b->bitsPerPixel)); /*number of palette entries to read*/
		pal = (paletteEntry *)malloc(sizeof(paletteEntry)*b->paletteEntries);
		if(pal == NULL) { return NULL; }
		for(i = 0; i < b->paletteEntries; i++) { /*read the palette thing*/
			pal[i].blue = fgetc(stream); /*bitmap has reverse RGB order for each entry*/
			pal[i].green = fgetc(stream);
			pal[i].red = fgetc(stream);
			if(b->headerSize >= 40) { /*later versions require 4 byte palette elements*/
				fgetc(stream); /*padding*/
			}
		}
		return (paletteEntry *)pal; /*return the allocated palette*/
	} else {
		return NULL; /*16 and 24 bit bitmaps dosent has palette*/
	}
}

uint8_t *readImageBitmapData(FILE *stream, bmpHeader *b) {
	static uint32_t i,i2;
	static uint16_t hold;
	uint8_t *data;
	if(b->tall == 0 || b->wide == 0) {
		return 0;
	}
	data = (uint8_t *)malloc((b->wide*b->tall)); /*allocate data for bitmap*/
	if(data == NULL) { return 0; }
	switch(b->compression) {
		case 0: /*no compression*/
			switch(b->bitsPerPixel) {
				case 8: /*256 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							fread(&hold,sizeof(uint8_t),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = hold;
						}
						if(((((b->wide*8)+7)>>3)&3)) { /*skip padding (dword)*/
							fskip(stream,3-((((b->wide*8)+7)>>3)&3));
						}
					}
					break;
				case 4: /*16 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							fread(&hold,sizeof(uint8_t),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = (hold>>4)&0x0F;
							i2++;
							data[(i2+((b->tall-i)*b->wide))] = hold&0x0F;
						}
						if(((((b->wide*4)+7)>>3)&3)) {
							fskip(stream,3-((((b->wide*4)+7)>>3)&3));
						}
					}
					break;
				case 2: /*4 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							if((i2&3) == 0) {
								fread(&hold,sizeof(uint8_t),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = (hold>>6)&3;
							hold = hold<<2;
						}
						if(((((b->wide*2)+7)>>3)&3)) {
							fskip(stream,3-((((b->wide*2)+7)>>3)&3));
						}
					}
					break;
				case 1: /*2 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							if(!(i2&7)) {
								fread(&hold,sizeof(uint8_t),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = ((hold>>7)&1);
							hold <<= 1;
						}
						if(((((b->wide*1)+7)>>3)&3)) {
							fskip(stream,3-((((b->wide*1)+7)>>3)&3));
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
	return (uint8_t *)data;
}

/* PALETTE.C
 * Reads and writes palette files (*.pal), can be used to transport palette
 * files betwen different VGA games*/

void  writePaletteFile(FILE *stream,paletteEntry *paletteInfo) {
	static uint16_t i;
	for(i = 0; i < 256; i++) { /*loop for 256 colors (should equal to a file of 768 bytes!)*/
		fwrite((uint8_t *)&paletteInfo[i].red,sizeof(uint8_t),1,stream);
		fwrite((uint8_t *)&paletteInfo[i].green,sizeof(uint8_t),1,stream);
		fwrite((uint8_t *)&paletteInfo[i].blue,sizeof(uint8_t),1,stream);
	}
	return;
}

paletteEntry *  readPaletteFile(FILE *stream) {
	paletteEntry *pal; /*define a struct pointer to allocate it and save stuff on it*/
	static uint16_t i;
	pal = malloc(sizeof(paletteEntry)*256); /*allocate the stuff*/
	if(pal == NULL) { return 0; } /*not engough memory*/
	for(i = 0; i < 256; i++) { /*loop and put stuff*/
		fread((uint8_t *)&pal[i].red,sizeof(uint8_t),1,stream);
		fread((uint8_t *)&pal[i].green,sizeof(uint8_t),1,stream);
		fread((uint8_t *)&pal[i].blue,sizeof(uint8_t),1,stream);
	}
	return pal;
}

/*PCX.C
 *ZSoft paintbrush file format parser and reader*/

int8_t  readImagePcxHeader(FILE *stream, pcxHeader *p) {
	static uint8_t type,version,compression,bitsPerPixel,reserved,planes;
	static uint8_t paletteType,horizontalScreenSize,verticalScreenSize;
	static uint16_t xStart,yStart,xEnd,yEnd,horizontalResolution,verticalResolution,bytesPerLine;
	static uint8_t i; /*iterator*/
	uint8_t *reserved2; /*54/58 bytes*/
	fread(&type,sizeof(uint8_t),1,stream);
	if(type != 10) { return -1; } /*pcx specification states that 10h is signature*/
	fread(&version,sizeof(uint8_t),1,stream);
	if(version > 5) { return -2; } /*pcx hasn't any version above 3.0 (0x05)*/
	fread(&compression,sizeof(uint8_t),1,stream);
	if(compression != 1) { return -3; } /*pcx is rle-only file*/
	fread(&bitsPerPixel,sizeof(uint8_t),1,stream);
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
			break;
		default:
			return -4; /*not valid!*/
	}
	fread(&xStart,sizeof(uint16_t),1,stream); /*useless stuff, but useful*/
	fread(&yStart,sizeof(uint16_t),1,stream); /*if the game has some sort of magic*/
	fread(&xEnd,sizeof(uint16_t),1,stream); /*stuff that makes pcx files to fly*/
	fread(&yEnd,sizeof(uint16_t),1,stream);
	fread(&horizontalResolution,sizeof(uint16_t),1,stream);
	fread(&verticalResolution,sizeof(uint16_t),1,stream);
	/*read the EGA palette for once*/
	for(i = 0; i < 16; i++) {
		p->egaPalette[i].red = fgetc(stream);
		p->egaPalette[i].green = fgetc(stream);
		p->egaPalette[i].blue = fgetc(stream);
	}
	fread(&reserved,sizeof(uint8_t),1,stream);
	if(reserved != 0) { return -6; } /*reserved is always 0*/
	fread(&planes,sizeof(uint8_t),1,stream);
	fread(&bytesPerLine,sizeof(uint16_t),1,stream);
	fread(&paletteType,sizeof(uint16_t),1,stream);
	fread(&horizontalScreenSize,sizeof(uint16_t),1,stream);
	fread(&verticalScreenSize,sizeof(uint16_t),1,stream);
	reserved2 = (uint8_t *)malloc(54);
	if(reserved2 == NULL) { return -7; }
	fread((uint8_t *)reserved2,sizeof(uint8_t),54,stream);
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

uint8_t *  readImagePcxData(FILE *stream, pcxHeader *p) {
	register uint8_t rLen,tmp,val;
	register uint32_t index,dataSize,total;
	static uint8_t * data;
	dataSize = (p->xEnd+1)*(p->yEnd+1);
	index = 0; total = 0;
	data = (uint8_t *)malloc(dataSize);
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
	return (uint8_t *)data;
}

/*TODO: Fix this function*/
paletteEntry *  readImagePcxVgaPalette(FILE *stream) {
	register int16_t vgaPaletteChecker;
	register uint16_t i;
	paletteEntry * pal;
	vgaPaletteChecker = fgetc(stream); /*is last byte 0x0C or EOF?*/
	if(vgaPaletteChecker != 0x0C || vgaPaletteChecker == EOF) { return NULL; }
	pal = (paletteEntry *)malloc(256*sizeof(paletteEntry));
	if(pal == NULL) { return NULL; } /*allocation error*/
	/*read rgb components into the palette entry*/
	for(i = 0; i < 256; i++) {
		pal[i].red = fgetc(stream);
		pal[i].green = fgetc(stream);
		pal[i].blue = fgetc(stream);
	}
	return (paletteEntry *)pal;
}

/*TIFF.C
 *File to read TIFF files*/

uint8_t readImageTiffHeader(FILE *stream, tiffHeader *t) {
	static uint16_t id,version;
	static uint32_t ifdOff;
	fread(&id,sizeof(uint16_t),1,stream);
	if(id != 0x4949 /*intel*/
	&& id != 0x4D4D /*motorola*/) {
		return -1; /*invalid tiff*/
	}
	fread(&version,sizeof(uint16_t),1,stream);
	fread(&ifdOff,sizeof(uint32_t),1,stream);
	t->version = version;
	t->id = id;
	t->ifdOff = ifdOff;
	return 0;
}
