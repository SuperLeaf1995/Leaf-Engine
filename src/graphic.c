#include "graphic.h"

#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
#include "dos\graphic.c"
#elif defined(__APPLE2__) /*In APPLE2 there is only one mode used and it's HIGHRES*/
#include "appleii\graphic.c"
#elif defined(__GBA__)
#include "gba\graphic.c"
#endif

void plotPixel(register unsigned short x, register unsigned short y, register unsigned char c) {
	if(y >= leafCurrentCtx->vtall || x >= leafCurrentCtx->vwide) { return; }
	videoBuffer[(y*leafCurrentCtx->vwide)+x] = c;
	return;
}

void plotLinearPixel(register unsigned short p,register unsigned char c) {
	videoBuffer[p] = c;
	return;
}

unsigned char fetchPixel(register unsigned short x,register unsigned short y) {
	return videoBuffer[(y*leafCurrentCtx->vwide)+x];
}

unsigned char fetchLinearPixel(register unsigned short p) {
	return videoBuffer[p];
}

void plotLine(register signed short sx, register signed short sy, register signed short ex, register signed short ey, register unsigned char c) {
	signed short i,dx,dy,sdx,sdy,dxabs,dyabs,px,py,x,y;
	dx = ex-sx; dy = ey-sy;
	sdx = ((dx < 0) ? -1 : ((dx > 0) ? 1 : 0));
	sdy = ((dy < 0) ? -1 : ((dy > 0) ? 1 : 0));
	dxabs = abs(dx); dyabs = abs(dy);
	x = (dyabs>>1); y = (dxabs>>1);
	px = sx; py = sy;
	
	plotPixel(px,py,c);
	
	if(dxabs >= dyabs) {
		for(i = 0; i < dxabs; i++) {
			y += dyabs;
			if(y >= dxabs) {
				y -= dxabs; py += sdy;
			}
			px += sdx;
			plotPixel(px,py,c);
		}
	} else {
		for(i = 0; i < dyabs; i++) {
			x += dxabs;
			if(x >= dyabs) {
				x -= dyabs; px += sdx;
			}
			py += sdy;
			plotPixel(px,py,c);
		}
	}
	return;
}

void plotWireSquare(register signed short x1, register signed short y1, register signed short x2, register signed short y2, register unsigned char c) {
	plotLine(x1,y1,x2,y1,c); plotLine(x1,y1,x1,y2,c);
	plotLine(x1,y2,x2,y2,c); plotLine(x2,y2,x2,y1,c);
	return;
}

void plotWirePolygon(signed short * d, register unsigned short n, register unsigned char c) {
	unsigned short i;
	for(i = 0; i < n-1; i++) {
		plotLine(d[(i<<1)],d[((i<<1)+1)],d[((i<<1)+2)],d[((i<<1)+3)],c);
	}
	plotLine(d[(i<<1)],d[(i<<1)+1],d[0],d[1],c);
	return;
}

void drawSprite(unsigned char * data, unsigned short x, register unsigned short y, register unsigned short sx, register unsigned short sy) {
	unsigned short i;
	unsigned short i2;
	for(i = 0; i < sx; i++) {
		for(i2 = 0; i2 < sy; i2++) {
			plotPixel(x+i,y+i2,data[(i2*sx)+i]);
		}
	}
	return;
}

void drawTiledSprite(unsigned char * data, unsigned short x, register unsigned short y, register unsigned short sx, register unsigned short sy, register unsigned short ix, register unsigned short iy, register unsigned short tx, register unsigned short ty) {
	unsigned short i;
	unsigned short i2;
	unsigned short i3;
	unsigned short i4;
	for(i = 0; i < tx; i++) {
		for(i2 = 0; i2 < ty; i2++) {
			i4 = iy; i3 = ix;
			plotPixel((x+i),(y+i2),data[(((i4*tx)+i2)*sy)+((i3)*ty)+i]);
		}
	}
	return;
}
