#ifndef __VIDEO_H__
#define __VIDEO_H__

#include "leaf.h"

struct Leaf_Video {
	unsigned width;
	unsigned height;
	unsigned totalSize;
	unsigned bitDepth;
	unsigned colors;
};

struct Leaf_Video_Palette {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct Leaf_Video_Context {
	unsigned char * internalVideoBuffer;
	struct Leaf_Video * currentVideo;
};

int Leaf_Video_InitByMode(struct Leaf_Video_Context * ctx, unsigned w, unsigned h, unsigned char m);
int Leaf_Video_End(struct Leaf_Video_Context * ctx);
int Leaf_Video_SetPalette(struct Leaf_Video_Context * ctx, struct Leaf_Video_Palette * pal, unsigned char size, unsigned char offs);
int Leaf_Video_DrawPixel(struct Leaf_Video_Context * ctx, unsigned x, unsigned y, unsigned char c);
int Leaf_Video_DrawFillSquare(struct Leaf_Video_Context * ctx, signed x, signed y, signed size, unsigned char c);
int Leaf_Video_DrawLine(struct Leaf_Video_Context * ctx, signed sx, signed sy, signed ex, signed ey, unsigned char c);
int Leaf_Video_DrawPoly(struct Leaf_Video_Context * ctx, signed x, signed y, signed * data, unsigned size, unsigned char c);
int Leaf_Video_UpdateScreen(struct Leaf_Video_Context * ctx);
int Leaf_Video_ClearScreen(struct Leaf_Video_Context * ctx);

#endif
