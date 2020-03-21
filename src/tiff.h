#ifndef TIFF_H
#define TIFF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stddefc.h"
#include "palette.h"

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

uint8_t readImageTiffHeader(FILE *stream, tiffHeader *t);

#endif
