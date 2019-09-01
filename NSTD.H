#ifndef NSTD_H
#define NSTD_H

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
extern union REGS in,out;
extern byte far *vgaMemory = (byte far *)0xA0000000L;

/*Mouse*/
extern char _Cdecl initmouse(void);
extern void _Cdecl setMousePosition(word x, word y);
extern void _Cdecl showMouse(void);
extern void _Cdecl hideMouse(void);
extern void _Cdecl getMouse(struct mouse *m);
extern byte _Cdecl mouseBoxGet(struct mouse *b, word fx, word fy, word tx, word ty);
/*Video*/
extern void _Cdecl initGraph(void);
extern void _Cdecl plotPixel(register word x, register word y, byte color);
extern void _Cdecl plotLinearPixel(register word coord, unsigned byte color);
extern word _Cdecl getVideo_mode(void);
extern void _Cdecl endGraph(void);
/*GET*/
extern void _Cdecl getBIOSEquipementList(struct computerBIOSEquipementList *comp);
#endif
