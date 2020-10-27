#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include "video.h"

struct Leaf_Image {
	unsigned w;
	unsigned h;
	unsigned char bpp;
	unsigned char * data;
	
	unsigned nPaletteElem;
	struct Leaf_Video_Palette * palette;
};

struct Leaf_Image_PCX_Header {
	unsigned char identifier;
	unsigned char version;
	unsigned char encoding;
	unsigned char bitsPerPixel;
	unsigned short xStart;
	unsigned short yStart;
	unsigned short xEnd;
	unsigned short yEnd;
	unsigned short horzRes;
	unsigned short vertRes;
	unsigned char palette[48];
	unsigned char reserved;
	unsigned char nBitPlanes;
	unsigned short bytesPerLine;
	unsigned short paletteSize;
	unsigned short hScreenSize;
	unsigned short vScreenSize;
	unsigned char reserved2[54];
};

int Leaf_Image_ReadBitmap(const char * filename, struct Leaf_Image * img);

#endif
