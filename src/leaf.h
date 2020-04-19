#ifndef LEAF_H
#define LEAF_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <conio.h>
#include <io.h>
#include <mem.h>
#endif

#if defined(__TURBOC__) && !defined(__BORLANDC__)
#include <dos.h>
#endif

#define _exit_code 1
#define _video_auto 1
#define BitmapSucess 0
#define BitmapErrorSignature -1
#define BitmapErrorBpp -3
#define BitmapErrorInvalidHeader -2
#define BitmapErrorFile -4
#define BitmapErrorInvalidColorsOutOfRange -5
#define BitmapErrorWrongPlanes -6
#define BitmapErrorFileReadSizeOfFile -7
#define BitmapErrorFileReadReserved -8
#define VideoUsesLinux -1
#define VideoErrorOnSet -2

typedef struct paletteEntry {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}paletteEntry;

typedef struct leafEvent {
	signed int key; /*Key scancode (0-255) or SPECIAL_KEY
					(used by Leaf-Engine for arrow keys and stuff)*/
	signed short cx; /*Change of X movement of mouse*/
	signed short cy; /*Change of Y movement of mouse*/
	signed char eventStatus; /*Bit 1, if set, means keypress
							Bit 2, if set means mouse-left button press
							Bit 3, if set means mouse-right button press
							Bit 4, if set means mouse-middle button press*/
	signed char ui; /*UI and window events, always 0 in DOS and monotask OSes*/
}leafEvent;

typedef struct leafGame {
	const char * name; /*name of the game*/
	unsigned char videoConf; /*used for video mode usage*/
	unsigned short vwide; /*wide of heritage screen*/
	unsigned short vtall; /*height of the screen*/
}leafGame;

unsigned short * clock = (unsigned short *)0x0000046CL;
unsigned char * video = (unsigned char * )0xA0000000L;

unsigned char * videoBuffer;

union REGS in,out;

void seedRandom(void);
signed int generateRandom(void);

signed int leafGameCreate(leafGame * g);
signed int leafGameEnd(leafGame * g);
signed int leafSetVideo(leafGame * g);

unsigned int setVideo(unsigned char v);
void plotPixel(register unsigned short x, register unsigned short y, register unsigned char c);
void plotLinearPixel(register unsigned short pos,register unsigned char color);
unsigned char fetchPixel(register unsigned short x,register unsigned short y);
void plotLine(register signed short sx, register signed short sy, register signed short ex, register signed short ey, register unsigned char c);
void plotWireSquare(register signed short x1, register signed short y1, register signed short x2, register signed short y2, register unsigned char c);
void setPalette(paletteEntry * p, register unsigned short n);
void plotWirePolygon(signed short * d, register unsigned short n, register unsigned char c);
void waitRetrace(void);
void updateScreen(void);

typedef struct _bmpHeader {
	unsigned char type[3];
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
	unsigned long paletteEntries;
	
	paletteEntry * palette;
}bmpHeader;

signed int readImageBitmapHeader(FILE * stream, bmpHeader * e);
paletteEntry * readImageBitmapPalette(FILE * stream, bmpHeader * b);
unsigned char * readImageBitmapData(FILE * stream, bmpHeader * b);

typedef struct pcxHeader {
	unsigned char type;
	unsigned char version;
	unsigned char compression;
	unsigned char bitsPerPixel;
	unsigned short xStart;
	unsigned short yStart;
	unsigned short xEnd;
	unsigned short yEnd;
	unsigned short horizontalResolution;
	unsigned short verticalResolution;
	unsigned char reserved;
	unsigned char planes;
	unsigned short bytesPerLine;
	unsigned short paletteType;
	unsigned short horizontalScreenSize;
	unsigned short verticalScreenSize;
	unsigned char * reserved2;
	
	paletteEntry * egaPalette;
	paletteEntry * vgaPalette;
}pcxHeader;

signed int readImagePcxHeader(FILE * stream, pcxHeader * p);
unsigned char * readImagePcxData(FILE * stream, pcxHeader * p);
paletteEntry * readImagePcxVgaPalette(FILE * stream);

struct mouse {
	buttonLeft:1;
	buttonRight:1;
	buttonMiddle:1;
	buttons:4;
	signed short x; signed short y;
	signed short mx; signed short my;
};

char initMouse(struct mouse * m);
void setMousePosition(unsigned short x, unsigned short y);
void showMouse(void);
void hideMouse(void);
void getMouseStatus(struct mouse * m);
void getMouseMovement(struct mouse * m);

#endif
