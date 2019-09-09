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

struct objectBasicProperties {
	word x;
	word y;
	word z;
	word id;
};

/*Global REGS. DO NOT CHANGE!*/
union REGS in,out;
byte far *vgaMemory = (byte far *)0xA0000000L;
byte far *textMemory = (byte far *)0xB8000000L;

/*Mouse*/
char _Cdecl initMouse(void);
void _Cdecl setMousePosition(word x, word y);
void _Cdecl showMouse(void);
void _Cdecl hideMouse(void);
void _Cdecl getMouse(struct mouse *m);
byte _Cdecl mouseBoxGet(struct mouse *b, word fx, word fy, word tx, word ty);
/*Video*/
void _Cdecl initGraph(void);
void _Cdecl plotPixel(register word x, register word y, byte color);
void _Cdecl plotLinearPixel(register word coord, unsigned byte color);
word _Cdecl getVideo_mode(void);
void _Cdecl endGraph(void);
_Cdecl printCustomCharset(byte *ptr, char * text, word x, word y);
void _Cdecl plotTile16(byte *ptr, register byte index, register word x, register word y);
void _Cdecl plotTile(byte *ptr, register byte index, register word x, register word y, size_t w, size_t t);
/*GET*/
void _Cdecl getBIOSEquipementList(struct computerBIOSEquipementList *comp);
/*Physics*/
void _Cdecl objectPhysicsBasicJump(struct objectBasicProperties *obj,word force,int leftKey,int rightKey,signed char or)
#endif
