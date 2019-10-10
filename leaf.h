#ifndef LEAF_H_INCLUDED
#define LEAF_H_INCLUDED

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#include <dos.h>

#include "key.h" /*Keyboard keys*/

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef unsigned long long uint64_t;
typedef signed long long int64_t;

/*Some bitmap strucutres, yes, alrigth, lets keep this small and compact*/
struct bitmapFileHeader {
    uint8_t type[3];
    uint32_t sizeOfFile;
    uint32_t reserved;
    uint32_t offset;
};

struct bitmapInfoHeader {
    uint32_t headerSize;
    int32_t wide;
    int32_t tall;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t sizeOfImage;
    uint32_t xPixelsPerMeter;
    uint32_t yPixelsPerMeter;
    uint32_t numberOfColors;
    uint32_t importantColors;
};

struct image {
    uint32_t wide;
    uint32_t tall;
    uint32_t tileTall; /*Tall of each tile (useful for tiled images)*/
    uint8_t *data;
};

uint8_t far *vgaMemory = (uint8_t far *)0xA0000000L;
uint8_t far *textMemory = (uint8_t far *)0xB8000000L;
uint16_t *clock = (uint16_t far *)0x0000046C;

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
void _Cdecl plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, uint8_t color);

char _Cdecl initMouse(struct mouse *m);
void _Cdecl setMousePosition(uint16_t x,uint16_t y);
void _Cdecl showMouse(void);
void _Cdecl hideMouse(void);
void _Cdecl getMouse(struct mouse *m);

void _Cdecl fskip(FILE *stream, uint64_t n);
int32_t _Cdecl switchEndianness32(int32_t end); /*Compatible with almost anything, even an Apple II+*/

#include "leaf.c" /*Primary functions. cross-platform (using macros obviously), do not change order!*/
#include "image.c" /*Bitmap and (PCX?) decoder?*/

#endif /*LEAF_H_INCLUDED*/
