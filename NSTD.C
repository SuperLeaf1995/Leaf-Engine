/*Cdecl intensifies*/
#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

/*Include important files*/
#include <stdio.h>
#include <dos.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef unsigned long long qword;

/*The structures and stuff*/
struct mouse {
	word x;
	word y;
	byte bLeft;
	byte bRight;
	byte bMiddle;
};

struct computerBIOSEquipementList {
	diskDrive : 1;
	mathCoProcessor : 1;
	word systemMemory;
	drives : 2;
	noDMA : 1;
	serialPorts : 3;
	gameAdapter : 1;
	serialInstalled : 1;
	serialPrinter : 1;
	numberOfPrinters : 2;
};

/*Global REGS. DO NOT CHANGE!*/
union REGS in,out;
byte far *vgaMemory = (byte far *)0xA0000000L;

void _Cdecl getBIOSEquipementList(struct computerBIOSEquipementList *comp) {
	register word a;
	register word c;
	a = biosequip();
	comp->diskDrive = ((a & 0x1) >= 0x1) ? 1 : 0;
	comp->mathCoProcessor = ((a & 0x2) >= 0x2) ? 1 : 0;
	/*How many memory we have*/
	comp->systemMemory = ((a & 3) >= 3) ? (((a & 7) >= 7) ? 64000 : 32000) : (((a & 7) >= 7) ? 48000 : 16000);
	comp->drives = ((a & 0x40-1) >= 0x40-1) ? 1 : 0;
	comp->drives += ((a & 0x80-1) >= 0x80-1) ? 1 : 0;
	comp->noDMA = ((a & 0x100-1) >= 0x100-1) ? 1 : 0;
	comp->serialPorts = ((a & 0x200-1) >= 0x200-1) ? 0x1 : 0x0;
	comp->serialPorts += ((a & 0x400-1) >= 0x400-1) ? 0x2 : 0;
	comp->serialPorts += ((a & 0x800-1) >= 0x800-1) ? 0x4 : 0;
	comp->gameAdapter = ((a & 0x1000-1) >= 0x1000-1) ? 1 : 0;
	/*Sorry PS/2 users*/
	comp->serialInstalled = ((a & 0x2000-1) >= 0x2000-1) ? 1 : 0;
	comp->serialPrinter = ((a & 0x4000-1) >= 0x4000-1) ? 1 : 0;
	comp->serialPrinter += ((a & 0x8000-1) >= 0x8000-1) ? 2 : 0;
	return;
}

/*Display routines*/

/*Initialize mode*/
void _Cdecl initGraph(void)
{
	/*Lets use our mind to change to VGA 256 mode*/
	in.h.ah = 0x00;
	in.h.al = 0x13;
	int86(0x10 ,&in, &out);
	return;
}

/*Plot a pixel with X and Y auto calculation*/
void _Cdecl plotPixel(register word x, register word y, byte color)
{
	vgaMemory[((y<<8)+(y<<6)+x)] = color;
	return;
}

/*Plot a pixel without taking X, Y in account*/
void _Cdecl plotLinearPixel(register word coord, register byte color) {
	vgaMemory[coord] = color;
	return;
}

/*Plots a line, be careful*/
void _Cdecl plotLine(word fx, word fy, word tx, word ty, byte color) {
	int dx,dy,px,py,x,y,i;
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

/*DO NOT EVER IN YOUR LIFE USE THIS*/
void _Cdecl plotBasicPoly(byte p, word *x, byte color) {
	word i;
	for(i = 0; i < p*3; i += 3) {
		plotLine(x[i],x[i+1],x[i+2],x[i+3],color);
	}
	return;
}

/*Gets current video mode*/
word _Cdecl getVideoMode(void) {
	in.x.ax = 0x0F;
	int86(0x10 ,&in, &out);
	return out.h.al;
}

/*End graphics*/
void _Cdecl endGraph(void)
{
	in.h.ah = 0x00;
	in.h.al = 0x03;
	int86(0x10 ,&in, &out);
	return;
}

/*Initialize mouse*/
char _Cdecl initMouse(void) {
	in.x.ax = 0;
	in.x.bx = 0;
	int86(0x33, &in, &out);
	if(out.x.ax != 0) {
		return 0;
	}
	else {
		/*Write to STDERR*/
		fprintf(stderr,"Mouse could not initialize");
		return -1;
	}
}

/*Set mouse position*/
void _Cdecl setMousePosition(word x, word y) {
	in.x.ax = 4;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33, &in, &out);
	return;
}

/*Display mouse*/
void _Cdecl showMouse(void) {
	in.x.ax = 1;
	int86(0x33, &in, &out);
	return;
}

/*Hide the mouse*/
void _Cdecl hideMouse(void) {
	in.x.ax = 2;
	int86(0x33, &in, &out);
	return;
}

/*Get Mouse status*/
void _Cdecl getMouse(struct mouse *m) {
	in.x.ax = 3;
	int86(0x33, &in, &out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->bLeft = (out.x.bx & 1);
	m->bRight = (out.x.bx & 2);
	m->bMiddle = (out.x.bx & 4);
	return;
}

/*Get if in BOX area*/
byte _Cdecl mouseBoxGet(struct mouse *b, word fx, word fy, word tx, word ty) {
	getMouse(b);
	if( b->x >= fx && b->x <= tx &&
		b->y >= fy && b->y <= ty) {
		return 1;	
	}
	else {
		return 0;
	}
}
