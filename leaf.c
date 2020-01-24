/* LEAF ENGINE
 * Copyright (C) Jesus Antonio Diaz - 2020
 * Licensed under Apache License, see LICENSE.md
 */

#ifndef LEAF_ENGINE_H
#define LEAF_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#define MOUSE_STATUS_HIDE 2
#define MOUSE_STATUS_SHOW 1

#define INT8_MAX 0x7F
#define INT8_MIN -128
#define UINT8_MAX 0xFFU
#define INT16_MAX 0x7FFF
#define INT16_MIN ((int)0x8000)
#define UINT16_MAX 0xFFFFU
#define INT32_MAX 0x7FFF
#define INT32_MIN ((int)0x8000)
#define UINT32_MAX 0xFFFFU
#define INT64_MAX 0x7FFFFFFFL
#define INT64_MIN ((long)0x80000000L)
#define UINT64_MAX 0xFFFFFFFFUL

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;

static uint8_t far *vgaMemory = (uint8_t far *)0xA0000000L;
static uint8_t far *textMemory = (uint8_t far *)0xB8000000L;
static volatile uint16_t far *clock = (uint16_t far *)0x0000046C; /*Clock always changes*/
union REGS in,out;

#define plotPixel(x,y,color) vgaMemory[(y<<8)+(y<<6)+x] = color
#define plotLinearPixel(pos,color) vgaMemory[pos] = color
#define fetchPixel(x,y) vgaMemory[(y<<8)+(y<<6)+x]
#define isMouseOnPosition(x,y,m) (m.x == x) ? ((m.y == y) ? 1 : 0) : 0 /*0 if false, 1 if true*/
#define isSigned(x) (x < 0) ? -1 : (x == 0) ? 0 : -1

struct mouse {
	buttonLeft:1;
	buttonRight:1;
	buttonMiddle:1;
	buttons:4;
	int16_t x;
	int16_t y;
};

struct image {
	uint32_t wide;
	uint32_t tall;
	uint32_t tileTall; /*Tall of each tile (useful for tiled images)*/
	uint8_t *data;
};

struct bitmapHeader {
	uint8_t type[3]; /*File*/
	uint32_t sizeOfFile;
	uint32_t reserved;
	uint32_t offset;
	uint32_t headerSize;
	int32_t wide; /*Info*/
	int32_t tall;
	uint16_t planes;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t sizeOfImage;
	uint32_t xPixelsPerMeter;
	uint32_t yPixelsPerMeter;
	uint32_t numberOfColors;
	uint32_t importantColors;
	uint32_t mask[4]; /*Eh, some bitmaps use it*/
};

/* ====================================================================
 *
 * IMPORTANT SUBFUNCTIONS
 *
 * ====================================================================
 */

void _Cdecl fskip(FILE *stream, uint64_t n) {
	static uint64_t i;
	for(i = 0; i < (n+1); i++) {
		fgetc(stream); /*Skip characters*/
	}
	return;
}

/* ====================================================================
 *
 * VIDEO AND DISPLAY (GRAPHICAL) FUNCTIONS
 *
 * ====================================================================
 */
uint8_t _Cdecl setVideo(register uint8_t video) {
	static uint8_t oldVideo;
	in.h.al = 0x0f;
	int86(0x10,&in,&out);
	oldVideo = out.h.al;
	in.h.al = video;
	in.h.ah = 0;
	int86(0x10,&in,&out);
	return oldVideo;
}

void plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, uint8_t color) {
	static int16_t dx,dy;
	static int16_t xi;
	static int16_t yi;
	static int16_t d;
	static int16_t x;
	static int16_t y;
	static int16_t i;
	if(abs(ty-fy) < abs(tx-fx)) { /*If ty-fy < tx-fx the line is more horizontal*/
		if(fx > tx) { /*STarting X is higher than ending X*/
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
		if(fy > ty) { /*Starting Y is higher than ending Y*/
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
			/*plotLineHigh(x0, y0, x1, y1)*/
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

/* ====================================================================
 *
 * SOUND
 *
 * ====================================================================
 */

void playSound(uint32_t x) {
	uint32_t cot;
	uint8_t tmp;
	cot = (uint32_t)1193180/500;
	outportb(0x43,0xb6);
	outportb(0x42,(uint8_t)cot);
	outportb(0x42,(uint8_t)cot>>8);
	tmp = inportb(0x61);
	if(tmp != (tmp|3)) {
		outportb(0x61,tmp|3);
	}	
	return;
}

void stopSound(void) {
	uint8_t tmp = inportb(0x61)&0xfc;
	outportb(0x61,tmp);
	return;
}

/* ====================================================================
 *
 * IMAGE FILES AND READ (BMP,ICO,ETC) FUNCTIONS
 *
 * ====================================================================
 */
int8_t _Cdecl readBitmapHeader(FILE *stream, struct bitmapHeader *e) {
	static uint32_t headerSize;
	static uint32_t wide;
	static uint32_t tall;
	static uint16_t planes;
	static uint16_t bitsPerPixel;
	static uint32_t compression;
	static uint32_t sizeOfImage;
	static uint32_t xPixelsPerMeter;
	static uint32_t yPixelsPerMeter;
	static uint32_t numberOfColors;
	static uint32_t importantColors;
	static uint32_t sizeOfFile;
	static uint32_t reserved;
	static uint32_t offset;
	static uint32_t mask[4];
	/*Read file header*/
	fread(e->type,sizeof(uint16_t),1,stream);
	/*Confirmate that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) == 0 /*Normal windows bitmap*/
	|| strncmp((const char *)e->type,"BA",2) == 0
	|| strncmp((const char *)e->type,"IC",2) == 0 /*Icons (OS/2)*/
	|| strncmp((const char *)e->type,"PT",2) == 0 /*Pointers (OS/2)*/
	|| strncmp((const char *)e->type,"CI",2) == 0 /*Color icons (OS/2)*/
	|| strncmp((const char *)e->type,"CP",2) == 0) /*Color pointers (OS/2)*/ {
		/*Its valid, proceed*/
	}
	else {
		/*Damn, return error*/
		return -1;
	}
	fread(&sizeOfFile,sizeof(uint32_t),1,stream);
	fread(&reserved,sizeof(uint32_t),1,stream); /*reserved has an actual mean in OS/2*/
	fread(&offset,sizeof(uint32_t),1,stream);
	e->sizeOfFile = sizeOfFile;
	e->reserved = reserved;
	e->offset = offset;
	/*Now information*/
	fread(&headerSize,sizeof(uint32_t),1,stream);
	e->headerSize = headerSize;
	/*Check the header size*/
	if(headerSize == 40) { /*Windows 3.x*/
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
		e->mask[0] = 0;
		e->mask[1] = 0;
		e->mask[2] = 0;
		e->mask[3] = 0;
	}
	else if(headerSize == 12) { /*OS/2 1.x*/
		fread(&wide,sizeof(uint16_t),1,stream);
		fread(&tall,sizeof(uint16_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
	}
	else if(headerSize >= 56 && headerSize <= 64) { /*Windows (95,98,2000,XP,Vista,7,8,8.1,10) bitmap*/
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
		/*Inmediately place masks, on the struct*/
		e->mask[0] = mask[0];
		e->mask[1] = mask[1];
		e->mask[2] = mask[2];
		e->mask[3] = mask[3];
	}
	else {
		return -1;
	}
	e->planes = planes;
	e->bitsPerPixel = bitsPerPixel;
	e->compression = compression;
	e->sizeOfImage = sizeOfImage;
	e->xPixelsPerMeter = xPixelsPerMeter;
	e->yPixelsPerMeter = yPixelsPerMeter;
	e->numberOfColors = numberOfColors;
	e->importantColors = importantColors;
	/*Check if bit's are valid*/
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
			return -1;
	}
	if(wide < 0 || tall < 0 || planes != 1 || numberOfColors > 256) { /*We did something wrong!*/
		return -1;
	}
	e->wide = wide;
	e->tall = tall;
	if((uint64_t)wide > INT32_MAX/bitsPerPixel
	|| (uint64_t)wide > (INT32_MAX/abs(tall))/4) { /*Avoid overflows*/
		return -1;
	}
	return 0;
}

uint8_t * _Cdecl readBitmapData(FILE *stream, struct bitmapHeader *b) {
	static uint32_t i;
	static uint32_t i2;
	static uint16_t hold;
	uint8_t *data;
	fpos_t pos;
	if(b->tall == 0 || b->wide == 0) {
		return 0;
	}
	data = (uint8_t *)malloc(b->wide*b->tall);
	if(data == NULL) {
		return 0; /*Up to caller's, how to handle errors*/
	}
	fgetpos(stream,&pos);
	for(i = 1; i < b->tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < b->wide; i2++) {
			fread(&hold,sizeof(uint8_t),1,stream);
			data[(i2+((b->tall-i)*b->wide))] = hold;
		}
		if(b->wide%4 != 0) { /*Padding exists*/
			fskip(stream,b->wide%4);
		}
	}
	return (uint8_t *)data;
}

void _Cdecl writeBitmap(FILE *stream, struct bitmapHeader *bih, uint8_t *data) {
	static uint32_t sizeOfFile,reserved,offset; /*We do the same as in our read routines*/
	static uint32_t headerSize;					/*we just switch the order of stuff and*/
	static uint32_t wide;						/*change read with write.*/
	static uint32_t tall;
	static uint16_t planes;
	static uint16_t bitsPerPixel;
	static uint32_t compression;
	static uint32_t sizeOfImage;
	static uint32_t xPixelsPerMeter;
	static uint32_t yPixelsPerMeter;
	static uint32_t numberOfColors;
	static uint32_t importantColors;
	static uint32_t i;
	static uint32_t i2;
	static uint16_t hold;
	sizeOfFile = bih->sizeOfFile;
	reserved = bih->reserved;
	offset = bih->offset;
	headerSize = bih->headerSize;
	wide = bih->wide;
	tall = bih->tall;
	planes = bih->planes;
	bitsPerPixel = bih->bitsPerPixel;
	compression = bih->compression;
	sizeOfImage = bih->sizeOfImage;
	xPixelsPerMeter = bih->xPixelsPerMeter;
	yPixelsPerMeter = bih->yPixelsPerMeter;
	numberOfColors = bih->numberOfColors;
	importantColors = bih->importantColors;
	fwrite(bih->type,sizeof(uint16_t),1,stream);
	fwrite(&sizeOfFile,sizeof(uint32_t),1,stream);
	fwrite(&reserved,sizeof(uint32_t),1,stream);
	fwrite(&offset,sizeof(uint32_t),1,stream);
	fwrite(&headerSize,sizeof(uint32_t),1,stream);
	fwrite(&wide,sizeof(int32_t),1,stream);
	fwrite(&tall,sizeof(int32_t),1,stream);
	fwrite(&planes,sizeof(uint16_t),1,stream);
	fwrite(&bitsPerPixel,sizeof(uint32_t),1,stream);
	fwrite(&compression,sizeof(uint32_t),1,stream);
	fwrite(&sizeOfImage,sizeof(uint32_t),1,stream);
	fwrite(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
	fwrite(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
	fwrite(&numberOfColors,sizeof(uint32_t),1,stream);
	fwrite(&importantColors,sizeof(uint32_t),1,stream);
	/*Palette*/
	for(i = 0; i < 1022; i++) {
		fwrite(&i,sizeof(uint8_t),1,stream);
	}
	/*Bitmap data*/
	for(i = 1; i < tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) { /*We just replotting our shit back again!*/
			hold = data[(i2+((tall-i)*wide))];
			fwrite(&hold,sizeof(uint8_t),1,stream);
		}
	}
	return;
}

/* ====================================================================
 * 
 * MOUSE FUNCTIONS
 * 
 * ====================================================================
 */

void _Cdecl setMouseStatus(uint8_t status) {
	in.x.ax = status;
	int86(0x33,&in,&out);
	return;
}

int8_t _Cdecl initMouse(struct mouse *m) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	/*set the correct button stuff*/
	if((in.x.bx&2) != 0) {
		m->buttons = 2;
	} else if((in.x.bx&2) != 0) {
		m->buttons = 3;
	} else {
		m->buttons = 0;
	}
	if(out.x.ax != 0) {
		setMouseStatus(MOUSE_STATUS_SHOW); /*automaticaly show mouse if the mouse was initialized*/
	}
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 1, else return 0*/
}

void _Cdecl setMousePosition(uint16_t x,uint16_t y) {
	in.x.ax = 4;
	in.x.cx = x; /*set the coordinates and stuff*/
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl getMouseStatus(struct mouse *m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/*Extra functions for compatibility with old Leaf Engine programs/games*/
void _Cdecl showMouse(void) {
	in.x.ax = 1;
	int86(0x33,&in,&out);
	return;
}
void _Cdecl hideMouse(void) {
	in.x.ax = 2;
	int86(0x33,&in,&out);
	return;
}

#endif

