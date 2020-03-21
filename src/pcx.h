#ifndef PCX_H
#define PCX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stddefc.h"
#include "palette.h"

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

int8_t  readImagePcxHeader(FILE *stream, pcxHeader *p);
uint8_t *  readImagePcxData(FILE *stream, pcxHeader *p);
paletteEntry *  readImagePcxVgaPalette(FILE *stream);

#endif
