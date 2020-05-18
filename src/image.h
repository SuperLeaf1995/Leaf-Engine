#ifndef __LEAF_IMAGE_H__
#define __LEAF_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
	size_t nPal;
}Leaf_Image;

void Leaf_destroyImage(Leaf_Image * img);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
