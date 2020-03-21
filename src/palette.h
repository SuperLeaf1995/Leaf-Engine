#ifndef PALETTE_H
#define PALETTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stddefc.h"

typedef struct paletteEntry {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}paletteEntry;

void  writePaletteFile(FILE *stream,paletteEntry *paletteInfo);
paletteEntry *  readPaletteFile(FILE *stream);

#endif
