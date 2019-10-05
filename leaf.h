#ifndef LEAF_H_INCLUDED
#define LEAF_H_INCLUDED

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;

#ifdef __APPLE2__
uint8_t *enterGraphicalMode = (uint8_t *)0xC050;
uint8_t *enterFullScreen = (uint8_t *)0xC052;
uint8_t *enterHighResolutionMode = (uint8_t *)0xC057;
uint8_t *graphicsPage1 = (uint8_t *)0xC054;
uint8_t *graphicsPage2 = (uint8_t *)0xC055;
uint8_t *graphicsAddressPage1 = (uint8_t *)0x2000;
uint8_t *graphicsAddressPage2 = (uint8_t *)0x4000;
uint8_t *speakerAddress = (uint8_t *)0xC030;

/*Some functions in some Apple compilers have diferent names, fuck off*/
#define getch() cgetc()

/*#include "leaf_apple2.c"*/
#endif

#if defined (__MSDOS__) || defined (_MSDOS) || defined (MSDOS) || defined(__DOS__) || defined(FREEDOS) /*Compatibility with Apple II*/
#include <dos.h>

uint8_t far *vgaMemory = (uint8_t far *)0xA0000000L;
uint8_t far *textMemory = (uint8_t far *)0xB8000000L;
uint16_t *clock = (uint16_t *)0x0000046C;

union REGS in,out;

struct mouse {
    buttonLeft:1;
    buttonRight:1;
    buttonMiddle:1;
    buttons:4;
    int16_t x;
    int16_t y;
};

#define plotPixel(x,y,color) vgaMemory[x+(y<<8)+(y<<6)] = color
#define fetchPixel(x,y) vgaMemory[x+(y<<8)+(y<<6)]

int16_t _Cdecl setVideo(int16_t video);
void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, int8_t color);

char _Cdecl initMouse(void);
void _Cdecl setMousePosition(int16_t x,int16_t y);
void _Cdecl showMouse(void);
void _Cdecl hideMouse(void);
void _Cdecl getMouse(struct mouse *m);

#include "leaf_dos.c"
#endif

#include "key.h" /*Keyboard keys*/

#endif /*LEAF_H_INCLUDED*/
