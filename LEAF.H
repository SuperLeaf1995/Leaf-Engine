/* LEAF.H
 * Main leaf file, should be included as "leaf.h" or "leaf/leaf.h" in program*/

#ifndef LEAF_ENGINE_H
#define LEAF_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#define MOUSE_STATUS_HIDE 2
#define MOUSE_STATUS_SHOW 1

#define KEY_ESC 0x1b
#define KEY_UP 72
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_DOWN 80

#define INT8_MAX 0x7F
#define INT8_MIN -128
#define UINT8_MAX 0xFFU
#define INT16_MAX 0x7FFF
#define INT16_MIN ((int)0x8000)
#define UINT16_MAX 0xFFFFU
#define INT32_MAX 0x7FFF
#define INT32_MIN ((int)0x8000)
#define UINT32_MAX 0xFFFFU
#define INT64_MAX 0x7FFFFFFFL
#define INT64_MIN ((long)0x80000000L)
#define UINT64_MAX 0xFFFFFFFFUL

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;

static uint8_t far *textMemory = (uint8_t far *)0xB8000000L;
static volatile uint16_t far *clock = (uint16_t far *)0x0000046C; /*Clock always changes*/
union REGS in,out;

#define isSigned(x) (x < 0) ? -1 : (x == 0) ? 0 : -1

typedef struct paletteEntry {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}paletteEntry;

typedef struct mouse {
	buttonLeft:1;
	buttonRight:1;
	buttonMiddle:1;
	buttons:4;
	int16_t x;
	int16_t y;
}mouse;

/*files that dosen't requieres of other files*/
#include "leaf/vga.h" /*vga defines and stuff*/
#include "leaf/file.c" /*file functions*/

/*extra stuff needed*/
#include "leaf/mouse.c" /*mouse functions*/
#include "leaf/video.c" /*video functions*/
#include "leaf/pcspeak.c" /*square sound functions*/

/*stuff for file formats*/
#include "leaf/file/bmp.c"
#include "leaf/file/pal.c"
#include "leaf/file/pcx.c"
#include "leaf/file/tiff.c"

#endif
