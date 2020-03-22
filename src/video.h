#ifndef BMP_H
#define BMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include "stddefc.h"
#include "palette.h"

void plotPixel(register uint16_t x,register uint16_t y,register uint8_t color);
void plotLinearPixel(register uint16_t pos,register uint8_t color);
uint8_t fetchPixel(register uint16_t x,register uint16_t y);
uint8_t setVideo(register uint8_t video);
uint8_t getVideo(void);
void setPalette(paletteEntry *pal, register uint16_t n);
void plotLine(register int16_t fx, register int16_t fy, register int16_t tx, register int16_t ty, register uint8_t color);
void plotPoly(int32_t n, int32_t *v, uint8_t color);

#endif
