#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stddefc.h"
#include "palette.h"

typedef struct _bmpHeader {
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

int8_t  readImageBitmapHeader(FILE *stream, bmpHeader *e);
paletteEntry *readImageBitmapPalette(FILE *stream, bmpHeader *b);
uint8_t *readImageBitmapData(FILE *stream, bmpHeader *b);

#endif
