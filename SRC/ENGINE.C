/* ENGINE.C
 * Base stuff, like functions used for the internal of everything*/

#define isSigned(x) (x < 0) ? -1 : (x == 0) ? 0 : -1

/* VIDEO.C
 * Header for video stuff and routines for plotting pixels and stuff*/

/*VGA functions*/
void vgaPlotPixel(register uint16_t x,register uint16_t y,register uint8_t color) {
	videoMemory[(y<<8)+(y<<6)+x] = color;
	return;
}
void vgaPlotLinearPixel(register uint16_t pos,register uint8_t color) {
	videoMemory[pos] = color;
	return;
}
uint8_t vgaFetchPixel(register uint16_t x,register uint16_t y) {
	return videoMemory[(y<<8)+(y<<6)+x];
}

/*default stuff*/
void (*plotPixel)(register uint16_t,register uint16_t,uint8_t) = vgaPlotPixel;
uint8_t (*fetchPixel)(register uint16_t,register uint16_t) = vgaFetchPixel;
void (*plotLinearPixel)(register uint16_t,register uint8_t) = vgaPlotLinearPixel;

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

/* PCSPEAK.C
 * C file for PC Speaker functions
 */

void soundPlay(uint32_t x) {
	uint32_t cot; /*countdown value*/
	uint8_t tmp; /*temporal value stuff*/
	cot = (uint32_t)(1193180/x);
	outportb(0x43,0xb6); /*say to the speaker we are going to send data*/
	outportb(0x42,(uint8_t)cot); /*output low byte of countdown*/
	outportb(0x42,(uint8_t)cot>>8); /*and then the high byte*/
	tmp = inportb(0x61); /*set pit2 timer*/
	if(tmp != (tmp|3)) {
		outportb(0x61,tmp|3);
	}
	return;
}

void soundStop(void) {
	uint8_t tmp = inportb(0x61)&0xfc; /*shutdown speaker command*/
	outportb(0x61,tmp);
	return;
}

int8_t soundPlayRawSoundFile(FILE *stream) {
	int16_t i = fgetc(stream);
	if(i == EOF) {
		fseek(stream,SEEK_SET,0); /*rewind file*/
	}
	soundPlay(i*30); /*TODO: know why multiplying by 30 makes a nice noise*/
	return 0;
}

/* FILE.C
 * Miscellaneous file functions for handling files better*/

void fskip(FILE *stream, uint64_t n) {
	static uint64_t i;
	for(i = 0; i < (n+1); i++) {
		fgetc(stream); /*Skip characters*/
	}
	return;
}

/* MOUSE.C
 * Contains mouse functions (MOUSE.COM driver stuff, useful for clicking
 * actions and stuff that involves mouse interaction*/

#define isMouseOnPosition(x,y,m) (m.x == x) ? ((m.y == y) ? 1 : 0) : 0 /*0 if false, 1 if true*/

void setMouseStatus(uint8_t status) {
	in.x.ax = status; /*See MOUSE_STATUS_HIDE and MOUSE_STATUS_SHOW for more information*/
	int86(0x33,&in,&out); /*issue an interrupt*/
	return;
}

int8_t initMouse(struct mouse *m) {
	in.x.ax = 0x00; /*ax 0 and bx 0 = mouse.com initializes mouse*/
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	/*set the correct button stuff*/
	if((in.x.bx&2) != 0) {
		m->buttons = 2;
	} else if((in.x.bx&2) != 0) {
		m->buttons = 3; /*posibly some weird mouse*/
	} else {
		m->buttons = 0; /*unknown!*/
	}
	if(out.x.ax != 0) { /*if mouse was initialized...*/
		setMouseStatus(MOUSE_STATUS_SHOW); /*...automaticaly show mouse*/
	}
	return (out.x.ax != 0) ? 0 : -1; /*if the mouse was initialized return 0, else return -1*/
}

void setMousePosition(uint16_t x,uint16_t y) {
	in.x.ax = 4; /*ax: 4, set mouse coordinates*/
	in.x.cx = x; /*set the coordinates and stuff*/
	in.x.dx = y;
	int86(0x33,&in,&out); /*issue the interrupt*/
	return;
}

void getMouseStatus(struct mouse *m) {
	in.x.ax = 0x03; /*command 0x03, get mouse info*/
	int86(0x33,&in,&out); /*issue the interrupt*/
	m->x = out.x.cx; /*x coords*/
	m->y = out.x.dx; /*y coords*/
	m->buttonLeft = (out.x.bx & 1); /*while it is not equal 0, its*/
	m->buttonRight = (out.x.bx & 2); /*HOLD/PRESSED, else its RELEASED*/
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

void updateMouse(struct mouse *m) {
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->x += (int16_t)out.x.cx; /*add the movement!*/
	m->y += (int16_t)out.x.dx;
	return;
}
