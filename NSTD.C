#ifndef NSTD_C
#define NSTD_C

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

struct bitmapHeader {
	word identifier;
	dword sizeOfFile;
	dword reserved;
	dword offsetFromFileData;
	dword headerSize;
	dword wide;
	dword tall;
	word planes;
	word bitsPerPixel;
	dword compression;
	dword sizeOfImage;
	dword xPixelsPerMeter;
	dword yPixelsPerMeter;
	dword numberOfColours;
	dword numberOfRelevantColours;
};

/*Global REGS. DO NOT CHANGE!*/
union REGS in,out;
byte far *vgaMemory = (byte far *)0xA0000000L;
byte oldVideoMode;
float pi = 3.1415;

/*Mouse*/
/*extern char _Cdecl initmouse(void);
extern void _Cdecl setMousePosition(word x, word y);
extern void _Cdecl showMouse(void);
extern void _Cdecl hideMouse(void);
extern void _Cdecl getMouse(struct mouse *m);
extern byte _Cdecl mouseBoxGet(struct mouse *b, word fx, word fy, word tx, word ty);*/
/*Video*/
/*extern void _Cdecl initGraph(void);
extern void _Cdecl plotPixel(register word x, register word y, byte color);
extern void _Cdecl plotLinearPixel(register word coord, unsigned byte color);
extern word _Cdecl getVideo_mode(void);
extern void _Cdecl endGraph(void);*/
/*GET*/
/*extern void _Cdecl getBIOSEquipementList(struct computerBIOSEquipementList *comp);*/

/*==========================================*/
/*Display routines							*/
/*==========================================*/

/*Initialize mode*/
void _Cdecl initGraph(void)
{
	/*Lets use our mind to change to VGA 256 mode*/
	in.h.ah = 0x00;
	in.h.al = 0x13;
	int86(0x10 ,&in, &out);
	oldVideoMode = out.h.al;
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
	in.h.al = oldVideoMode;
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
	dx = (tx >= fx) ? tx-fx : fx-tx;
	dy = (ty >= fy) ? ty-fy : fy-ty;
	if(abs(dy) >= abs(dx)) {
		sl=(float)dy/(float)dx;
		for(i = 0; i != dx; i+= 1) {
			px = i+fx;
			py = sl*i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	else if(abs(dy) <= abs(dx)) {
		sl=(float)dx/(float)dy;
		for(i = 0; i != dy; i+= 1) {
			py = sl*i+fx;
			px = i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	return;
}
/*==========================================*/
/*Mouse routines							*/
/*==========================================*/

/*Initialize mouse*/
char _Cdecl initMouse(void) {
	in.x.ax = 0;
	in.x.bx = 0;
	int86(0x33, &in, &out);
	return (out.x.ax != 0) ? 0 : -1;
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

/*==========================================*/
/*Miscellaneous stuff						*/
/*==========================================*/

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

/*==========================================*/
/*Experimental								*/
/*==========================================*/

qword _Cdecl readBitmapFile(FILE *stream, struct bitmapHeader *bh, byte *ptr, byte *palette) {
	word d;
	if(!stream) { return -1; } /*Error file isnt open*/
	if(bh == NULL) { return -2; } /*Error null pointer*/
	/*Time to read header*/
	fread((struct bitmapHeader *)bh->identifier,sizeof(word),1,stream);
	fread((struct bitmapHeader *)bh->sizeOfFile,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->reserved,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->offsetFromFileData,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->headerSize,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->wide,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->tall,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->planes,sizeof(word),1,stream);
	fread((struct bitmapHeader *)bh->bitsPerPixel,sizeof(word),1,stream);
	fread((struct bitmapHeader *)bh->compression,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->sizeOfImage,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->xPixelsPerMeter,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->yPixelsPerMeter,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->numberOfColours,sizeof(dword),1,stream);
	fread((struct bitmapHeader *)bh->numberOfRelevantColours,sizeof(dword),1,stream);
	/*1024-bytes wide palette information*/
	fread((byte *)palette,sizeof(byte),1024,stream);
	/*Now read the image data*/
	for(d = bh->wide*bh->tall; d > 0; d--) {
		fread((byte *)ptr[d],sizeof(byte),1,stream);
	}
	return (qword)((bh->wide<<32)+bh->tall); /*useful for many operations*/
}

byte _Cdecl writeBitmapFile(FILE *stream, struct bitmapHeader *bh, byte *ptr, byte *palette) {
	word d;
	if(!stream) { return -1; } /*Error file isnt open*/
	if(bh == NULL) { return -2; } /*Error null pointer*/
	/*Time to read header*/
	fwrite((struct bitmapHeader *)bh->identifier,sizeof(word),1,stream);
	fwrite((struct bitmapHeader *)bh->sizeOfFile,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->reserved,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->offsetFromFileData,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->headerSize,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->wide,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->tall,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->planes,sizeof(word),1,stream);
	fwrite((struct bitmapHeader *)bh->bitsPerPixel,sizeof(word),1,stream);
	fwrite((struct bitmapHeader *)bh->compression,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->sizeOfImage,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->xPixelsPerMeter,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->yPixelsPerMeter,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->numberOfColours,sizeof(dword),1,stream);
	fwrite((struct bitmapHeader *)bh->numberOfRelevantColours,sizeof(dword),1,stream);
	/*1024-bytes wide palette information*/
	fwrite((byte *)palette,sizeof(byte),1024,stream);
	/*Now plot the image data again, rEvERsAl orDeR!!*/
	for(d = bh->wide*bh->tall; d > 0; d--) {
		fwrite((byte *)ptr[d],sizeof(byte),1,stream);
	}
	return 0;
}

#endif
