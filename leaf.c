/*===Basic but needed routines (change endianess, etc)====*/
int32_t switchEndianness32(int32_t end) {
    return (end<<24)|((end<<8)&0x00ff0000)|((end>>8)&0x0000ff00)|((end>>24)&0x000000ff);
}

/*=====================Video Routines=====================*/

int16_t _Cdecl setVideo(int16_t video) { /*Sets the video using int 10h*/
    in.h.ah = 0x00;
    in.h.al = video;
    int86(0x10,&in,&out);
    return out.h.al;
}

void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, int8_t color) {
	int16_t dx,dy,px,py,x,y,i;
	float sl;
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
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	else {
		sl=(float)dx/(float)dy;
		for(i = 0; i != dy; i+= 1) {
			py = sl*i+fx;
			px = i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	return;
}

/*=====================Mouse Routines=====================*/

char _Cdecl initMouse(void) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
}

void _Cdecl setMousePosition(int16_t x,int16_t y) {
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl showMouse(void) {
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl hideMouse(void) {
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

void _Cdecl getMouse(struct mouse *m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1);
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/*=================Decode/Encode Routines==================*/
/*
*0-2	Identifier	Defaults to BM
*2-6	Size of file	Size of file, can range from 0 to 65,550
*6-10	Reserved	?
*10-14	Offset	Offset from the file data
*14-18	Header size	The size of the Header, set to 40 [1]
*18-22	Wide	Wide of the image
*22-26	Tall	Tall of the image
*26-28	Planes	By default set to 1
*28-30	Bits per pixel	If set 1 becomes monochrome and each bit is a pixel, if set 4 can have up to 16 colours and each byte are two pixels, 8 to 256 colours and each byte is a pixel, and 28 has up to 2^28
and has three bytes per pixel, representing Red, Green and Blue respectively

*30-34	Compression	If set to 0 RLE is enabled. Else it's raw data
*34-38	Size of image	Size in bytes of the image
*38-42	X px/m	X pixels per meter
*42-46	Y px/m	Y pixels per meter
*46-50	Number of colors	Number of used colors, max. Depends on bits per pixel
*50-54	Important colors	Number of relevant colors, i.e the ones that are used most
*/

