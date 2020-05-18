#ifndef __LEAF_IMAGE_H__
#define __LEAF_IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palette.h"

typedef struct _Leaf_Image {
	unsigned long wide;
	unsigned long tall;
	unsigned char bitsPerPixel;
	unsigned char * data;
	
	paletteEntry * palette;
}Leaf_Image;

#endif
