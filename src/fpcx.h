#ifndef __LEAF_FPCX_H__
#define __LEAF_FPCX_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palette.h"

/* PcxHeader structure */
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

/*
PCX read functions
*/
signed int readImagePcxHeader(FILE * stream, pcxHeader * p);
unsigned char * readImagePcxData(FILE * stream, pcxHeader * p);
paletteEntry * readImagePcxVgaPalette(FILE * stream);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
