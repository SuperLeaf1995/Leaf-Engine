#include "graphic.h"

unsigned int setVideo(unsigned char v) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.h.al = v;
	in.h.ah = 0; /*set the video we want*/
	int86(0x10,&in,&out);
	
	leafCurrentCtx->vwide = vtable[v][0];
	leafCurrentCtx->vtall = vtable[v][1];
	leafCurrentCtx->vvideo = vtable[v][2];
	if(leafCurrentCtx->vvideo == __ega || leafCurrentCtx->vvideo == __vga) {
		video = (unsigned char *)0xA0000000L;
	} else if(leafCurrentCtx->vvideo == __cga) {
		video = (unsigned char *)0xB8000000L;
	}
#elif defined(__APPLE2__) /*In APPLE2 there is only one mode used and it's HIGHRES*/
	poke(0xC050,0); /*Enter graphics mode*/
	poke(0xC052,0); /*Enter fullscreen*/
	poke(0xC057,0); /*Enter highres mode*/
	poke(0xC054,0); /*Select PAGE1*/
#elif defined(__GBA__)
	*(unsigned long*)0x4000000 = 0x403; /*Set mode 3*/
#endif
	return (unsigned int)v;
}

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

#if defined(__GBA__)
#define abs(x) ((unsigned)x)
#endif

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

void setPalette(paletteEntry * p, register unsigned short n) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	register unsigned short i;
	if(leafCurrentCtx->vvideo == __vga) {
		outp(0x03C8,(unsigned char)0); /*send to the vga registers that we are going to send palette data*/
		for(i = 0; i < n; i++) {
			outp(0x03C9,(unsigned char)(p[i].r>>2));
			outp(0x03C9,(unsigned char)(p[i].g>>2));
			outp(0x03C9,(unsigned char)(p[i].b>>2));
		}
	} else if(leafCurrentCtx->vvideo == __ega) {

	} else if(leafCurrentCtx->vvideo == __cga) {

	} /*__MSDOS__*/
#elif defined(__GBA__) || defined(__linux) || defined(linux)
	register unsigned short i;
	for(i = 0; i < n; i++) {
		leafCurrentCtx->rgbPalette[i] = p[i];
	}
#endif
	return;
}

void waitRetrace(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	while((inp(0x03DA)&8));
	while(!(inp(0x03DA)&8));
#endif
	return;
}

void updateScreen(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	unsigned long i;
	size_t x; size_t y; size_t offs;
	unsigned char * cgaSelect[2] = {
		(unsigned char *)0xB8000000L,
		(unsigned char *)0xBA000000L
	};
	unsigned char bitMask; 
	waitRetrace(); /* Wait for VGA retrace */
	if(leafCurrentCtx->vvideo == __vga) {
		/*in VGA simply copy it to the plain VGA memory*/
		memcpy(video,videoBuffer,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Copy data to VGA */
	} else if(leafCurrentCtx->vvideo == __ega) {
		/*TODO: Add working EGA code*/
		in.h.ah = 0x0C;
		for(i = 0; i < (leafCurrentCtx->vwide*leafCurrentCtx->vtall); i++) {
			in.x.dx = (i/leafCurrentCtx->vwide);
			in.x.cx = (i%leafCurrentCtx->vwide);
			in.h.al = videoBuffer[i];
			int86(0x10,&in,&out);
		}
	} else if(leafCurrentCtx->vvideo == __cga) {
#if defined(__386__) || defined(_M_I386) || defined(__i386__) || defined(__32BIT__) /*A 32-bit compiler*/
		/*Use adaptive framebuffer (all-screen)*/
		for(i = 0; i < (leafCurrentCtx->vwide*leafCurrentCtx->vtall); i++) {
			x = (i%320); y = (i/320);
			bitMask = 0x80>>(x%8);
			offs = ((y>>1)*((leafCurrentCtx->vwide)>>3)+(x>>3));
			if(videoBuffer[i]) { *(cgaSelect[y&1]+offs) |= bitMask; }
			else { *(cgaSelect[y&1]+offs) &= ~bitMask; }
		}
#else /*A 16-bit compiler*/
		/*Wide the pixels*/
		for(i = 0; i < (leafCurrentCtx->vwide*leafCurrentCtx->vtall); i++) {
			x = (i%320); y = (i/320);
			bitMask = 0x80>>(x%8);
			offs = ((y>>1)*((leafCurrentCtx->vwide<<1)>>3)+(x>>3));
			if(videoBuffer[i]) { *(cgaSelect[y&1]+offs) |= bitMask; }
			else { *(cgaSelect[y&1]+offs) &= ~bitMask; }
		}
#endif
	}
#elif defined(__GBA__)
	size_t i;
	for(i = 0; i < (160*240); i++) {
		/*Why does this work?*/
		*(unsigned short *)(0x6000000+(i*sizeof(unsigned short))) = rgb16(
		leafCurrentCtx->rgbPalette[videoBuffer[i]].r,
		leafCurrentCtx->rgbPalette[videoBuffer[i]].g,
		leafCurrentCtx->rgbPalette[videoBuffer[i]].b
		);
	}
#endif
	memset(videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
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
