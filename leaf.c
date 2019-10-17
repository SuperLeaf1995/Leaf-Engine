/*
@Action: Sets video to *video*
@Parameters: video=16-bit long value of the desired video mode
@Output: Old video mode
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT+WIP)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*T), EGA (*T), CGA (*T)
*/
int16_t _Cdecl setVideo(register int16_t video) { /*Sets the video using int 10h*/
	static int16_t oldVideo;
	in.h.ah = 0x0F;
    int86(0x10,&in,&out);
    oldVideo = out.h.al;
    if(oldVideo == 0x29) { /*Realtek RTVGA BIOS v3.C10 crashes when switching to mode 0x21 or 0x27*/
		if(video == 0x21 || video == 0x27) {
			in.h.ah = 0x00;
			in.h.al = 0x13;
			int86(0x10,&in,&out);
		}
	}
    in.h.ah = 0x00;
    in.h.al = video;
    int86(0x10,&in,&out);
    return oldVideo;
}

/*
@Action: Gets video adapter
@Parameters: void
@Output: Video adapter [see below]
-0 Unknown (probably SVGA)
-1 AHEAD adapters
-2 PARADISE adapters
-3 OAK TECH adapters
-5 ATI 18800
-6 ATI 18800-1
-7 ATI 18800-2
-8 ATI 18800-4
-9 ATI 18800-5
-10 ATI 68800AX
-11 EGA Wonder
-12 VGA Wonder
-13 EGA Wonder8000+
-14 Genoa 6200/6300
-15 Genoa 6400/6600
-16 Genoa 6100
-17 Genoa 5100/5200
-18 Genoa 5300/5400
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT+WIP)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*T), EGA (*T), CGA (*T)
*/
int32_t _Cdecl getVideoAdapter(void) {
	static uint8_t *ptr;
	static uint8_t *txt;
	txt = (uint8_t *)malloc(sizeof(uint8_t)*6);
	if(txt == NULL) {
		return -1;
	}
	ptr = (uint8_t *)0xC0000025; /*Check for AHEAD adapters*/
	strcpy(txt,"AHEAD\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		return 1;
	}
	txt = (uint8_t *)realloc(txt,sizeof(uint8_t)*5);
	ptr = (uint8_t *)0xC000007D; /*Check for PARADISE adapters*/
	strcpy(txt,"VGA=\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		return 2;
	}
	txt = (uint8_t *)realloc(txt,sizeof(uint8_t)*8);
	ptr = (uint8_t *)0xC0000008; /*Check for OAK TECH adapters*/
	strcpy(txt,"OAK VGA\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		return 3;
	}
	txt = (uint8_t *)realloc(txt,sizeof(uint8_t)*10);
	ptr = (uint8_t *)0xC0000031; /*Check for ATI adapters*/
	strcpy(txt,"761295520\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		ptr = (uint8_t *)0xC0000043; /*We also have to put the stupid revision*/
		switch(*ptr) {
			case 0x31:
				free(txt);
				return 5; /*18800*/
			case 0x32:
				free(txt);
				return 6; /*18800-1*/
			case 0x33:
				free(txt);
				return 7; /*18800-2*/
			case 0x34:
				free(txt);
				return 8; /*18800-4*/
			case 0x35:
				free(txt);
				return 9; /*18800-5*/
			case 0x62:
				free(txt);
				return 10; /*68800AX*/
		}
		ptr = (uint8_t *)0xC0000040;
		switch(ptr[0]) {
			case '2':
				switch(ptr[1]) {
					case '2':
						return 11; /*EGA Wonder*/
				}
				break;
			case '3':
				switch(ptr[1]) {
					case '1':
						return 12; /*VGA Wonder*/
					case '2':
						return 13; /*EGA Wonder8000+*/
				}
				break;
		}
	}
	free(txt);
	ptr = (uint8_t *)0xC0000037; /*Genoa Graphics Adapter*/
	ptr = (uint8_t *)0xC0000000+(*ptr); /*Fuck, what a mess!*/
	if(ptr[0] == 0x77 && ptr[2] == 0x99 && ptr[3] == 0x66) {
		switch(ptr[1]) {
			case 0x00:
				return 14; /*Genoa 6200/6300*/
			case 0x11:
				return 15; /*Genoa 6400/6600*/
			case 0x22:
				return 16; /*Genoa 6100*/
			case 0x33:
				return 17; /*Genoa 5100/5200*/
			case 0x55:
				return 18; /*Genoa 5300/5400*/
		}
	}
	return 0;
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
@Action: "redraws" black area left by initialized mouse once a thing has been draw
@Parameters: structure mouse=structure of the mouse
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*NDFI)
@Compatible video modes: VGA (*T), SVGA (*T), Hercules (*UT), EGA (*UT), CGA (*UT)
*/
void _Cdecl redrawOnMouse(struct mouse *m) {
    static uint16_t i,i2;
    for(i = 0; i < 16; i++) {
        for(i2 = 0; i2 < 16; i2++) {
            plotPixel(i+(m->x/2),i2+(m->y/2),fetchPixel(i+(m->x/2),i2+(m->y/2)));
        }
    }
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
