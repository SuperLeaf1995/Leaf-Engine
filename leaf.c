/*
@Action: Sets video to *video*
@Parameters: video=16-bit long value of the desired video mode
@Output: Old video mode
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT+WIP)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*T), EGA (*T), CGA (*T)
*/
int16_t _Cdecl setVideo(int16_t video) { /*Sets the video using int 10h*/
    in.h.ah = 0x00;
    in.h.al = video;
    int86(0x10,&in,&out);
    in.h.ah = 0x0F;
    int86(0x10,&in,&out);
    return out.h.al;
}

/*
@Action: Plots a line
@Parameters: fx=from x. fy=from y. tx=to x. ty=to y. color=color (byte)
@Output: void
@Compatible platforms: All that plotPixel() can handle
@Compatible video modes: All that plotPixel() can handle
*/
void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, uint8_t color) {
	static int16_t dx,dy,px,py,i;
	static float sl;
	if(tx >= fx) {
		dx = tx-fx;
	}
	else {
		dx = fx-tx;
	}
	if(ty >= fy) {
		dy = ty-fy;
	}
	else {
		dy = fy-ty;
	}
	if(abs(dy) >= abs(dx)) {
		sl=(float)dy/(float)dx;
		for(i = 0; i != dx; i+= 1) {
			px = i+fx;
			py = sl*i+fy;
			plotPixel(px,py,color);
		}
	}
	else {
		sl=(float)dx/(float)dy;
		for(i = 0; i != dy; i+= 1) {
			py = sl*i+fx;
			px = i+fy;
			plotPixel(px,py,color);
		}
	}
	return;
}

/*
@Action: Initializes mouse
@Parameters: structure mouse=structure of the mouse, there should be one per program (recommended)
because this structure will hold all mouses. And i don't think anyone uses more than 2
bloody mouses.
@Output: void, but returns overwritten structure *m
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*NDFI)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*UT), EGA (*UT), CGA (*UT)
*/
char _Cdecl initMouse(struct mouse *m) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	if((in.x.bx&2) != 0) { /*Two button mouse*/
        m->buttons = 2;
	}
	else if((in.x.bx&3) != 0) { /*Three button mouse*/
        m->buttons = 3;
	}
	else { /*Unknown buttons*/
        m->buttons = 0;
	}
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
}

/*
@Action: Sets position of mouse
@Parameters: x=x position. y=y position
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*NDFI)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*UT), EGA (*UT), CGA (*UT)
*/
void _Cdecl setMousePosition(uint16_t x,uint16_t y) {
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

/*
@Action: Shows mouse
@Parameters: void
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*NDFI)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*UT), EGA (*UT), CGA (*UT)
*/
void _Cdecl showMouse(void) {
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

/*
@Action: Hide mouse
@Parameters: void
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*NDFI)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*UT), EGA (*UT), CGA (*UT)
*/
void _Cdecl hideMouse(void) {
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

/*
@Action: Get mouse status
@Parameters: structure mouse=structure of the mouse to write to
@Output: void, but returns overwritten structure *m
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*NDFI)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*UT), EGA (*UT), CGA (*UT)
*/
void _Cdecl getMouseStatus(struct mouse *m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/*
@Action: Skips *n* number of chars
@Parameters: stream=file stream. n=number of chars to skip
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl fskip(FILE *stream, uint64_t n) {
    static uint64_t i;
    for(i = 0; i < (n+1); i++) {
        fgetc(stream); /*Skip characters*/
    }
    return;
}
