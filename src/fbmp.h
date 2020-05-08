#ifndef __LEAF_FBMP_H__
#define __LEAF_FBMP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palette.h"

/* BitmapHeader structure */
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

#endif
