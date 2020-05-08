/*
 * Leaf-Engine Base General Header for access with Leaf-Engine
 * Copyright (C) 2020 Jesus A. Diaz <jesusantonio30122016@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#ifndef __LEAF_LEAF_H__
#define __LEAF_LEAF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*These should be supported on all targets*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <float.h>
/*DOS specific functions*/
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
#include <conio.h>
#include <io.h>
#include <mem.h>
#if defined(__TURBOC__) && !defined(__BORLANDC__)
#include <dos.h> /*Borland compilers use this*/
#elif defined(__DJGPP__)
#include <sys/nearptr.h>
#endif /* __DJGPP__ */
#endif

#if defined(__linux) || defined(linux)
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "context.h"

/*Current engine version*/
#if !defined(LEAF_ENGINE)
#define LEAF_ENGINE 035L
#endif

/*
Codes for UI (Always check those, In DOS the exit code is produced
by Ctrl+C)
*/
#define _exit_code 1

/*
Video automatic-modes for leafGame (see leafGameCreate)
*/
#define _video_auto 1

/*A simple getchar cross-platform wrapper*/
#if defined(__linux) || defined(linux)
#define getch() getchar()
#elif defined(__APPLE2__)
#define getch() cgetc()
#endif

/*The Borland compilers uses inportb()
to avoid further confusions, inp() and outp() are used*/
#if defined(__TURBOC__) || defined(__BORLANDC__)
#define inp(x) inportb(x)
#define outp(x,y) outportb(x,y)
#endif

#if defined(__GBA__)
#define rgb16(r,g,b) ((r)+(g<<5)+(b<<10))
#endif

/*allows easy-detection and on-demand video switching without
using multiple versions*/
#define __cga	0x01
#define __vga	0x03
#define __ega	0x07
#define __mcga	0x0F

/*
LeafEvent is a special structure wich handles all events in a program
this should be the main structure and there should be generally one per
program.

The structure aims to provide a simple way to get/set data for input.
*/
typedef struct leafEvent {
	signed int key;
	/** Change of the mouse X */
	signed short cx;
	/** Change of the mouse Y */
	signed short cy;
	signed char eventStatus;
	/** UI code, can be used in UI-based/capable operating systems
	Always zero on DOS */
	signed char ui;
}leafEvent;

/* Mouse handler structure */
struct mouse {
	unsigned char buttonLeft;
	unsigned char buttonRight;
	unsigned char buttonMiddle;
	unsigned char buttons;
	signed short x; signed short y;
	signed short mx; signed short my;
};

/*
The main game context we are working on
*/
leafContext * leafCurrentCtx;

#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
/*Global variable for register I/O (DOS-only)*/
union REGS in,out;
#endif

extern unsigned char * videoBuffer;

/*
Time-based random number generator
*/
void seedRandom(void);
signed int generateRandom(void);

/*
Main, principal Leaf-Engine functions
*/
signed int leafContextCreate(leafContext * g);
signed int leafContextDestroy(leafContext * g);
signed int leafSetVideo(leafContext * g);

/*
Functions for manipulating the cursor and retrieving data from it.
Using the mouse structure to create a mouse handler.
*/
char initMouse(struct mouse * m);
void setMousePosition(unsigned short x, unsigned short y);
void showMouse(void);
void hideMouse(void);
void getMouseStatus(struct mouse * m);
void getMouseMovement(struct mouse * m);

/*
Functions to save data in (TODO) compressed format. Example: A savefile
or a array of characters with their respective information.
*/
signed char saveDataFile(FILE * s, void * data, size_t n);
signed char loadDataFile(FILE * s, void * data, size_t n);

/*
Simple functions to help debugging
*/
signed char openLogFile(FILE * s, const char * name);
signed char appendLogFile(FILE * s, const char * entry);
signed char closeLogFile(FILE * s);

/*
Sound functions
*/
signed char initSound(void);
void playSound(unsigned long freq);
void stopSound(void);

/*Legacy support (LeafEngine <= 0.3.4)*/
#if (LEAF_ENGINE <= 034L)
#define leafGameCreate(x) leafContextCreate(x)
#define leafGameEnd(x) leafContextDestroy(x)
#endif

/*Legacy support (LeafEngine <= 0.3.0)*/
#if defined(__USE_LEGACY_MODE__) || (LEAF_ENGINE < 030L) || !defined(LEAF_ENGINE)

/*TurboC 2.01 dosen't offers a default stdint.h
so Leaf-Engine <= 0.3.0 relied on this easy hack*/
#if defined(__TURBOC__) && !defined(__BORLANDC__)
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;
#endif
/*Newer Borland Turbo C++ +3.x  and DJGPP offers a default stdint.h to rely on*/
#if defined(__BORLANDC__) || defined(__DJGPP__)
#include <stdint.h>
#endif

/*DOS-dependant keystroke keymap (there was a reason this keymap was
removed, due to the fact that other systems have different keymappings)*/
#define KEY_F1 0x03b
#define KEY_F2 0x03c
#define KEY_F3 0x03d
#define KEY_F4 0x03e
#define KEY_F5 0x03f
#define KEY_F6 0x040
#define KEY_F7 0x041
#define KEY_F8 0x042
#define KEY_F9 0x043
#define KEY_F10 0x044
#define KEY_F11 0x0ff85
#define KEY_F12 0x0ff86
#define KEY_INS 0x052
#define KEY_DEL 0x053
#define KEY_HM 0x047
#define KEY_END 0x04f
#define KEY_PGUP 0x049
#define KEY_PGDWN 0x051
#define KEY_TAB 0x9
#define KEY_ENTER 0xd
#define KEY_ESC 0x1b
#define KEY_UP 0x048
#define KEY_DOWN 0x050
#define KEY_RIGHT 0x04d
#define KEY_LEFT 0x04b

/*Obsolete-error reporting functions*/
#ifdef LEAF_ERROR
#define setLeafError(id) putc(id,stdout)
#define leafError() getche()
#endif

/*fskip(s,n) was used instead of the correct fseek*/
#define fskip(s,n) fseek(s,SEEK_CUR,n)

/*Useless function, return SVGA as default*/
#define getVideoAdapter() 0

/*Yet another obsolete function. With double-buffer mouse redraw is
almost unessesary and useless*/
#define redrawOnMouse(m) 0

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
