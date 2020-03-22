/* LEAF.H
 * Main leaf file*/

#ifndef LEAF_H
#define LEAF_H

#include <stdio.h>
#include <stdlib.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux)
#include <stdint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#endif

typedef struct paletteEntry {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}paletteEntry;

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

typedef struct mouse {
	uint8_t buttonLeft;
	uint8_t buttonRight;
	uint8_t buttonMiddle;
	uint8_t buttons;
	int16_t x;
	int16_t y;
}mouse;

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

extern void plotPixel(register uint16_t x,register uint16_t y,register uint8_t color);
extern void plotLinearPixel(register uint16_t pos,register uint8_t color);
extern uint8_t fetchPixel(register uint16_t x,register uint16_t y);
extern uint8_t setVideo(register uint8_t video);
extern uint8_t getVideo(void);
extern void setPalette(paletteEntry *pal, register uint16_t n);
extern void plotLine(register int16_t fx, register int16_t fy, register int16_t tx, register int16_t ty, register uint8_t color);
extern void plotPoly(int32_t n, int32_t *v, uint8_t color);
extern int8_t  readImageBitmapHeader(FILE *stream, bmpHeader *e);
extern paletteEntry *readImageBitmapPalette(FILE *stream, bmpHeader *b);
extern uint8_t *readImageBitmapData(FILE *stream, bmpHeader *b);
extern int8_t  readImagePcxHeader(FILE *stream, pcxHeader *p);
extern uint8_t *  readImagePcxData(FILE *stream, pcxHeader *p);
extern paletteEntry *  readImagePcxVgaPalette(FILE *stream);
extern void setMouseStatus(uint8_t status);
extern int8_t initMouse(struct mouse *m);
extern void setMousePosition(uint16_t x,uint16_t y);
extern void getMouseStatus(struct mouse *m);
extern void updateMouse(struct mouse *m);
extern uint8_t readImageTiffHeader(FILE *stream, tiffHeader *t);
extern void  writePaletteFile(FILE *stream,paletteEntry *paletteInfo);
extern paletteEntry *  readPaletteFile(FILE *stream);
extern void soundPlay(uint32_t x);
extern void soundStop(void);
extern int8_t soundPlayRawSoundFile(FILE *stream);

#endif
