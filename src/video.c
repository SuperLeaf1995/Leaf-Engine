#include "video.h"

#define isSigned(x) (x < 0) ? -1 : (x == 0) ? 0 : -1

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
uint8_t *videoMemory = (uint8_t *)0xA0000000L;
union REGS in,out;
#endif

/* VIDEO.C
 * Header for video stuff and routines for plotting pixels and stuff*/

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
/*VGA functions*/
void plotPixel(register uint16_t x,register uint16_t y,register uint8_t color) {
	videoMemory[(y<<8)+(y<<6)+x] = color;
	return;
}
void plotLinearPixel(register uint16_t pos,register uint8_t color) {
	videoMemory[pos] = color;
	return;
}
uint8_t fetchPixel(register uint16_t x,register uint16_t y) {
	return videoMemory[(y<<8)+(y<<6)+x];
}

/* VIDEO.C
 * Includes video-mode independent functions, like plotting a line and
 * getting video mode, setting video mode, etc*/

uint8_t setVideo(register uint8_t video) {
	static uint8_t oldVideo;
	in.h.al = 0x0f;
	int86(0x10,&in,&out); /*get old video*/
	oldVideo = out.h.al; /*save old video*/
	in.h.al = video; /*now set video that we want*/
	in.h.ah = 0;
	int86(0x10,&in,&out); /*issue interrupt*/
	return oldVideo; /*return the old video*/
}

uint8_t getVideo(void) {
	in.h.al = 0x0f;
	int86(0x10,&in,&out); /*issue interrupt*/
	return out.h.al; /*return video we got*/
}

void setPalette(paletteEntry *pal, register uint16_t n) {
	register uint16_t i;
	outportb(0x03c8,0);
	for(i = 0; i < n; i++) {
		outportb(0x03c9,(pal[i].red>>2));
		outportb(0x03c9,(pal[i].green>>2));
		outportb(0x03c9,(pal[i].blue>>2));
	}
	return;
}

void plotLine(register int16_t fx, register int16_t fy, register int16_t tx, register int16_t ty, register uint8_t color) {
	static int16_t dx,dy,xi,yi,d,x,y,i;
	if(abs(ty-fy) < abs(tx-fx)) { /*if ty-fy < tx-fx the line is more horizontal*/
		if(fx > tx) { /*starting X is higher than ending X*/
			dx=fx-tx;
			dy=fy-ty;
			yi=1;
			if(dy < 0) {
				yi = -1;
				dy = -dy;
			}
			d = 2*dy - dx;
			y = fy;
			for(i = fx; !(i > tx); i++) {
				plotPixel(i,y,color);
				if(d > 0) {
					y = y+yi;
					d = d -2*dx;
				}
				d = d +2*dy;
			}
		}
		else {
			dx=tx-fx;
			dy=ty-fy;
			yi=1;
			if(dy < 0) {
				yi = -1;
				dy = -dy;
			}
			d = 2*dy - dx;
			y = fy;
			for(i = fx; !(i > tx); i++) {
				plotPixel(i,y,color);
				if(d > 0) {
					y = y+yi;
					d = d -2*dx;
				}
				d = d +2*dy;
			}
		}
	}
	else {
		if(fy > ty) { /*starting Y is higher than ending Y*/
			dx = fx-tx;
			dy = fy-ty;
			xi = 1;
			if(dx < 0) {
				xi = -1;
				dx = -dx;
			}
			d = 2*dx-dy;
			x=fx;
			for(i = fy; !(i > ty); i++) {
				plotPixel(x,i,color);
				if(d > 0) {
					x = x+xi;
					d = d-2*dy;
				}
				d=d+2*dx;
			}
		}
		else {
			dx = tx-fx;
			dy = ty-fy;
			xi = 1;
			if(dx < 0) {
				xi = -1;
				dx = -dx;
			}
			d = 2*dx-dy;
			x=fx;
			for(i = fy; !(i > ty); i++) {
				plotPixel(x,i,color);
				if(d > 0) {
					x = x+xi;
					d = d-2*dy;
				}
				d=d+2*dx;
			}
		}
	}
	return;
}

void plotPoly(int32_t n, int32_t *v, uint8_t color) {
	int32_t i;
	for(i = 0; i < n-1; i++) {
		/*iterate trough all vertices*/
  		plotLine(v[(i<<1)+0],v[(i<<1)+1],v[(i<<1)+2],v[(i<<1)+3],color);
	}
	plotLine(v[0],v[1],v[(n<<1)-2],v[(n<<1)-1],color);
}
#endif
