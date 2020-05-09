#ifndef __LEAF_FBMP_H__
#define __LEAF_FBMP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "palette.h"
#include "image.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Device-Dependant bitmap file for Windows 1.x*/
typedef struct _DDBheader {
	unsigned short wide;
	unsigned short tall;
	unsigned short bitmapSize;
	unsigned char planes;
	unsigned char bitsPerPixel;
}DDBheader;

/** Windows Bitmap File header, included in all Windows bitmaps */
typedef struct _WinBmpFileHeader {
	unsigned long fileSize;
	unsigned long reserved;
	unsigned long offset;
	unsigned long headerSize;
}WinBmpFileHeader;

/** Windows 2.x bitmap file header */
typedef struct _Win2xBmpFileHeader {
	signed short wide;
	signed short tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
}Win2xBmpFileHeader;

/** Windows 3.x+ (inc. 95/98) bitmap file header */
typedef struct _Win3xBmpFileHeader {
	signed long wide;
	signed long tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
}Win3xBmpFileHeader;

/** Windows 3.x bitmap info header */
typedef struct _Win3xBmpHeader {
	unsigned long compression;
	unsigned long bitmapSize;
	signed long xResolution;
	signed long yResolution;
	unsigned long usedColors;
	unsigned long importantColors;
}Win3xBmpHeader;

/** Windows NT bitmap info header */
typedef struct _WinNTBmpHeader {
	unsigned long compression;
	unsigned long bitmapSize;
	signed long xResolution;
	signed long yResolution;
	unsigned long usedColors;
	unsigned long importantColors;
}WinNTBmpHeader;

/** Windows NT bitmap masks */
typedef struct _WinNTBmpMasks {
	unsigned long r;
	unsigned long g;
	unsigned long b;
}WinNTBmpMasks;

/** Windows 95/98 bitmap info header */
typedef struct _Win95BmpHeader {
	unsigned long compression;
	unsigned long bitmapSize;
	signed long xResolution;
	signed long yResolution;
	unsigned long usedColors;
	unsigned long importantColors;
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
}Win95BmpHeader;

signed int imageBitmap(const char * filename, Image * img);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
