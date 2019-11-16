#ifndef LEAF_H_INCLUDED
#define LEAF_H_INCLUDED

#if defined(__STDC__) && !defined(__GNUC__)
#define _Cdecl
#elif !defined(__STDC__) && !defined(__GNUC__)
#define _Cdecl	cdecl
#elif defined(__GNUC__)
#define _Cdecl __attribute__((cdecl)) /*POSIX stuff dosent has _Cdecl
										like that, lets just warp the attribute*/
#endif

#if !defined(__LARGE__) && !defined(__HUGE__)
#error "Error: Video mode won't work on small sizes"
#endif

/*Include important headers*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS) || defined(__OS2__)
#include <conio.h>
#endif

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif

#if defined(linux) || defined(__unix__)
#include <unistd.h>
#endif

#if defined(__GNUC__)
#define getch() getchar() /*We will use getch() anywhere, so yeah*/
#elif defined(__APPLE2__) /*Apple 2 uses something!*/
#define getch() cgetc()
#endif

/*Keys*/
#define KEY_F1 0x03b
#define KEY_F2 0x03c
#define KEY_F3 0x03d
#define KEY_F4 0x03e
#define KEY_F5 0x03f
#define KEY_F6 0x040
#define KEY_F7 0x041
#define KEY_F8 0x042
#define KEY_F9 0x043
#define KEY_F10 0x044
#define KEY_F11 0x0ff85
#define KEY_F12 0x0ff86

#define KEY_INS 0x052
#define KEY_DEL 0x053
#define KEY_HM 0x047
#define KEY_END 0x04f
#define KEY_PGUP 0x049
#define KEY_PGDWN 0x051
#define KEY_TAB 0x9
#define KEY_ENTER 0xd
#define KEY_ESC 0x1b

#define KEY_UP 0x048
#define KEY_DOWN 0x050
#define KEY_RIGHT 0x04d
#define KEY_LEFT 0x04b

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
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
#elif defined(__GNUC__)
#include <stdint.h>
#endif

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)

#if defined(__LARGE__) || defined(__HUGE__)
static uint8_t far *vgaMemory = (uint8_t far *)0xA0000000L;
static uint8_t far *textMemory = (uint8_t far *)0xB8000000L;
static volatile uint16_t far *clock = (uint16_t far *)0x0000046C; /*Clock always changes*/
#endif

union REGS in,out;

struct mouse {
    buttonLeft:1;
    buttonRight:1;
    buttonMiddle:1;
    buttons:4;
    int16_t x;
    int16_t y;
};
#endif

struct image {
    uint32_t wide;
    uint32_t tall;
    uint32_t tileTall; /*Tall of each tile (useful for tiled images)*/
    uint8_t *data;
};

struct bitmapHeader {
	/*File*/
    uint8_t type[3];
    uint32_t sizeOfFile;
    uint32_t reserved;
    uint32_t offset;
    uint32_t headerSize;
    /*Info*/
    int32_t wide;
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

struct pcxHeader {
	uint8_t id;
	uint8_t version;
	uint8_t encoding;
	uint8_t bitsPerPixel;
	uint16_t xStart;
	uint16_t yStart;
	uint16_t xEnd;
	uint16_t yEnd;
	uint16_t horizontalResolution;
	uint16_t verticalResolution;
	uint8_t palette[48]; /*EGA palette*/
	uint8_t reserved;
	uint8_t numberOfBitPlanes;
	uint16_t bytesPerLine;
	uint16_t paletteType;
	uint16_t horizontalScreenSize;
	uint16_t verticalScreenSize;
	uint8_t zeroAfterHeader[54];
};

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#define plotPixel(x,y,color) vgaMemory[(y<<8)+(y<<6)+x] = color
#define plotLinearPixel(pos,color) vgaMemory[pos] = color
#define fetchPixel(x,y) vgaMemory[(y<<8)+(y<<6)+x]
#endif

#ifdef LEAF_ERROR
uint8_t globalLeafErrorHandler = 255;
void _Cdecl setLeafError(uint8_t id);
char * _Cdecl leafError(void);
#endif

uint16_t _Cdecl getEndianess(void);
void _Cdecl fskip(FILE *stream, uint64_t n);

/*LEAF.C*/
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
int16_t _Cdecl setVideo(register int16_t video);
int32_t _Cdecl getVideoAdapter(void);
#endif

void _Cdecl readBitmapHeader(FILE *stream, struct bitmapHeader *e);
uint8_t * _Cdecl readBitmapData(FILE *stream, struct bitmapHeader *b);
void _Cdecl writeBitmap(FILE *stream, struct bitmapHeader *bih, uint8_t *data);

void _Cdecl readPaintbrushHeader(FILE *stream, struct pcxHeader *p);
uint8_t * _Cdecl readPaintbrushData(FILE *stream, struct pcxHeader *p);

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
void _Cdecl displayImage(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall);
void _Cdecl displayBitmapImageWhileReading(FILE *stream, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall);
void _Cdecl displayImageTile(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall, uint32_t index);
void _Cdecl displayImageTileTransparent(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall, uint32_t index, uint8_t trans);

void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, uint8_t color);
char _Cdecl initMouse(struct mouse *m);
char _Cdecl initMouse(struct mouse *m);

void _Cdecl showMouse(void);
void _Cdecl hideMouse(void);

void _Cdecl getMouseStatus(struct mouse *m);
void _Cdecl redrawOnMouse(struct mouse *m);
#endif

#include "leaf.c" /*Primary functions!*/

#endif /*LEAF_H_INCLUDED*/
