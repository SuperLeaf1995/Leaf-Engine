#include <stdlib.h>
#include <dos.h>

#include "leaf.h"
#include "video.h"

/* Initializes video mode by W and H */
int Leaf_Video_InitByMode(struct Leaf_Video_Context * ctx, unsigned w, unsigned h, unsigned char m) {
	union REGS r;
	
	r.h.ah = 0x00; r.h.al = m;
	int86(0x10,&r,&r);
	
	ctx->currentVideo = NULL;
	ctx->internalVideoBuffer = NULL;
	
	ctx->currentVideo = malloc(sizeof(struct Leaf_Video));
	if(ctx->currentVideo == NULL) {
		return 1;
	}
	ctx->currentVideo->width = w;
	ctx->currentVideo->height = h;
	ctx->currentVideo->totalSize = ctx->currentVideo->width*ctx->currentVideo->height;
	
	ctx->internalVideoBuffer = malloc(ctx->currentVideo->totalSize);
	if(ctx->internalVideoBuffer == NULL) {
		return 2;
	}
	return 0;
}

/* Deletes and ends everything related to video */
int Leaf_Video_End(struct Leaf_Video_Context * ctx) {
	free(ctx->currentVideo);
	free(ctx->internalVideoBuffer);
	return 0;
}

/* Set the video palette of the VGA */
int Leaf_Video_SetPalette(struct Leaf_Video_Context * ctx, struct Leaf_Video_Palette * pal, unsigned char size, unsigned char offs) {
	unsigned i;
	
	/* Wait for correct timing (no flicker) */
	while(inportb(0x03da)&0x08) {
		
	}
	while(!(inportb(0x03da)&0x08)) {
		
	}
	
	outportb(0x03c8,offs);
	for(i = offs; i < (unsigned char)offs+size; i++) {
		outportb(0x3c9,pal[i].r>>2);
		outportb(0x3c9,pal[i].g>>2);
		outportb(0x3c9,pal[i].b>>2);
	}
	return 0;
}

/* Plots a pixel on screen */
int Leaf_Video_DrawPixel(struct Leaf_Video_Context * ctx, unsigned x, unsigned y, unsigned char c) {
	ctx->internalVideoBuffer[(ctx->currentVideo->width*y)+x] = c;
	return 0;
}

/* Function to draw a filled square on screen */
int Leaf_Video_DrawFillSquare(struct Leaf_Video_Context * ctx, signed x, signed y, signed size, unsigned char c) {
	signed i,j;
	
	for(i = x; i < x+size; i++) {
		for(j = y; j < y+size; j++) {
			Leaf_Video_DrawPixel(ctx,i,j,c);
		}
	}
	
	return 0;
}

/* Function to draw a line on the screen */
int Leaf_Video_DrawLine(struct Leaf_Video_Context * ctx, signed sx, signed sy, signed ex, signed ey, unsigned char c) {
	signed i,dx,dy,sdx,sdy,dxabs,dyabs,px,py,x,y;
	
	/* Are lines far from screen? (junkly drawn?) */
	if(sx < 0 && sy < 0 && ex < 0 && ey < 0) {
		return 0;
	}
	
	/* Limit lines to screen ;) */
	sx = (sx < 0) ? 0 : sx;
	sy = (sy < 0) ? 0 : sy;
	ex = (ex < 0) ? 0 : ex;
	ey = (ey < 0) ? 0 : ey;
	
	/* Now do actual linely stuff */
	dx = ex-sx; dy = ey-sy;
	sdx = ((dx < 0) ? -1 : ((dx > 0) ? 1 : 0));
	sdy = ((dy < 0) ? -1 : ((dy > 0) ? 1 : 0));
	dxabs = abs(dx); dyabs = abs(dy);
	x = (dyabs>>1); y = (dxabs>>1);
	px = sx; py = sy;
	
	Leaf_Video_DrawPixel(ctx,px,py,c);
	
	if(dxabs >= dyabs) {
		for(i = 0; i < dxabs; i++) {
			y += dyabs;
			if(y >= dxabs) {
				y -= dxabs; py += sdy;
			}
			px += sdx;
			Leaf_Video_DrawPixel(ctx,px,py,c);
		}
	} else {
		for(i = 0; i < dyabs; i++) {
			x += dxabs;
			if(x >= dyabs) {
				x -= dyabs; px += sdx;
			}
			py += sdy;
			Leaf_Video_DrawPixel(ctx,px,py,c);
		}
	}
	return 0;
}

/* Poly draw */
int Leaf_Video_DrawPoly(struct Leaf_Video_Context * ctx, signed x, signed y, signed * data, unsigned size, unsigned char c) {
	unsigned i;
	
	for(i = 0; i < ((size<<1)-1); i+=2) {
		Leaf_Video_DrawLine(ctx,data[i]+x,data[i+1]+y,data[i+2]+x,data[i+3]+y,c);
	}
	Leaf_Video_DrawLine(ctx,data[0]+x,data[1]+y,data[i-2]+x,data[i-1]+y,c);
	return 0;
}

/* Updates the screen (draws buffer) */
int Leaf_Video_UpdateScreen(struct Leaf_Video_Context * ctx) {
	void * VGAMem = (void *)0xA0000000L;
	int i;
	
	memcpy(VGAMem,ctx->internalVideoBuffer,ctx->currentVideo->totalSize);
	return 0;
}

/* Clears the video buffer */
int Leaf_Video_ClearScreen(struct Leaf_Video_Context * ctx) {
	int i;
	
	memset(ctx->internalVideoBuffer,0,ctx->currentVideo->totalSize);
	return 0;
}
