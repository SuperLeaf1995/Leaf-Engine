/* LEAF ENGINE
 * Copyright (C) Jesus Antonio Diaz - 2020
 * Licensed under Apache License, see LICENSE.md
 */

/* VIDEO.C
 * Includes video-mode independent functions, like plotting a line and
 * getting video mode, setting video mode, etc*/

uint8_t _Cdecl setVideo(register uint8_t video) {
	static uint8_t oldVideo;
	in.h.al = 0x0f;
	int86(0x10,&in,&out); /*get old video*/
	oldVideo = out.h.al; /*save old video*/
	in.h.al = video; /*now set video that we want*/
	in.h.ah = 0;
	int86(0x10,&in,&out); /*issue interrupt*/
	return oldVideo; /*return the old video*/
}

uint8_t _Cdecl getVideo(void) {
	in.h.al = 0x0f;
	int86(0x10,&in,&out); /*issue interrupt*/
	return out.h.al; /*return video we got*/
}

void plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, uint8_t color) {
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
