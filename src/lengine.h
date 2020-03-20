/* LENGINE.H
 * Main leaf file*/

#ifndef LEAF_ENGINE_H
#define LEAF_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#if defined(__GNUC__)
#include <stddef.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;
#endif

#if defined(__FAST__) && !defined (__GNUC__)
#include "a2c.h"
#endif

#define MOUSE_STATUS_HIDE 2
#define MOUSE_STATUS_SHOW 1

#define KEY_ESC 0x1b
#define KEY_UP 72
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_DOWN 80

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

/*static uint8_t *textMemory = (uint8_t *)0xB8000000L;*/
uint8_t *videoMemory = (uint8_t *)0xA0000000L;
static volatile uint16_t *clock = (uint16_t *)0x0000046C; /*Clock always changes*/
union REGS in,out;

typedef struct paletteEntry {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}paletteEntry;

typedef struct mouse {
	uint8_t buttonLeft;
	uint8_t buttonRight;
	uint8_t buttonMiddle;
	uint8_t buttons;
	int16_t x;
	int16_t y;
}mouse;

typedef struct tiffHeader {
	uint16_t id;
	uint16_t version; /*version and stuff*/
	uint32_t ifdOff; /*first image offset*/
}tiffHeader;

typedef struct tiffTag {
	uint16_t tagId;
	uint16_t dataType;
	uint32_t dataCount;
	uint32_t dataOff;
}tiffTag;

typedef struct tiffIfd {
	uint16_t directoryEntries; /*number of directory entries*/
	tiffTag *tags; /*12-byte wide tags array*/
	uint32_t nextIfdOff; /*next ifd*/
}tiffIfd;

typedef struct pcxHeader {
	uint8_t type;
	uint8_t version;
	uint8_t compression;
	uint8_t bitsPerPixel;
	uint16_t xStart;
	uint16_t yStart;
	uint16_t xEnd;
	uint16_t yEnd;
	uint16_t horizontalResolution;
	uint16_t verticalResolution;
	uint8_t reserved;
	uint8_t planes;
	uint16_t bytesPerLine;
	uint16_t paletteType;
	uint16_t horizontalScreenSize;
	uint16_t verticalScreenSize;
	uint8_t *reserved2;
	
	paletteEntry *egaPalette;
	paletteEntry *vgaPalette;
}pcxHeader;

typedef struct bmpHeader {
	uint8_t type[3]; /*file header*/
	uint32_t sizeOfFile;
	uint32_t reserved;
	uint32_t offset;
	uint32_t headerSize;
	int32_t wide; /*bitmap header*/
	int32_t tall;
	uint16_t planes;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t sizeOfImage;
	uint32_t xPixelsPerMeter;
	uint32_t yPixelsPerMeter;
	uint32_t numberOfColors;
	uint32_t importantColors;
	uint32_t mask[4];
	
	uint16_t paletteEntries;
	paletteEntry *palette;
}bmpHeader;

#endif
