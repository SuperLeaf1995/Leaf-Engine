#include "graphic.h"

unsigned int setVideo(unsigned char v) {
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
	return (unsigned int)v;
}

void setPalette(paletteEntry * p, register unsigned short n) {
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

	}
	return;
}

static void waitRetrace(void) {
	while((inp(0x03DA)&8));
	while(!(inp(0x03DA)&8));
	return;
}

void updateScreen(void) {
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
	memset(videoBuffer,0,(size_t)leafCurrentCtx->vwide*leafCurrentCtx->vtall); /* Clear our buffer */
	return;
}
