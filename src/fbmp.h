/*
 * Leaf-Engine FBMP Header
 * Copyright (C) 2020 Jesus A. Diaz <jesusantonio30122016@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#ifndef __LEAF_FBMP_H__
#define __LEAF_FBMP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palette.h"
#include "image.h"

/** Device-Dependant bitmap file for Windows 1.x*/
typedef struct _DDBheader
{
	unsigned short wide;
	unsigned short tall;
	unsigned short bitmapSize;
	unsigned char planes;
	unsigned char bitsPerPixel;
}DDBheader;

/** Windows Bitmap File header, included in all Windows bitmaps */
typedef struct _BmpFileHeader
{
	unsigned long fileSize;
	unsigned long reserved;
	unsigned long offset;
	unsigned long headerSize;
}BmpFileHeader;

/** Windows 2.x bitmap file header */
typedef struct _WinOldBmpFileHeader
{
	signed short wide;
	signed short tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
}WinOldBmpFileHeader;

/** Windows 3.x+ and newer, bitmap file header */
typedef struct _WinNewBmpFileHeader
{
	signed long wide;
	signed long tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
}WinNewBmpFileHeader;

/** Windows bitmap info header */
typedef struct _WinBmpHeader {
	unsigned long compression;
	unsigned long bitmapSize;
	signed long xResolution;
	signed long yResolution;
	unsigned long usedColors;
	unsigned long importantColors;
}WinBmpHeader;

/** Windows NT bitmap masks */
typedef struct _WinNTBmpMasks {
	unsigned long r;
	unsigned long g;
	unsigned long b;
}WinNTBmpMasks;

/** Windows 95/98 bitmap info header */
typedef struct _Win95BmpHeaderExtension {
	unsigned long rMask;
	unsigned long gMask;
	unsigned long bMask;
	unsigned long aMask;
	unsigned long colorSpace;
	signed long rX;
	signed long rY;
	signed long rZ;
	signed long gX;
	signed long gY;
	signed long gZ;
	signed long bX;
	signed long bY;
	signed long bZ;
	unsigned long gRed;
	unsigned long gGreen;
	unsigned long gBlue;
}Win95BmpHeaderExtension;

signed int Leaf_imageBitmap(const char * filename, Leaf_Image * img);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
