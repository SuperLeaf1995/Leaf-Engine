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
#include <dos.h>
#endif
#if defined(__DJGPP__)
#include <sys/nearptr.h>
#endif /* __DJGPP__ */

#if defined(__linux) || defined(linux)
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "context.h"

#if defined(__DJGPP__)
volatile unsigned short * biosClock = (volatile unsigned short *)0x046C+__djgpp_conventional_base;
#elif defined(__TURBOC__) || defined(__BORLANDC__)
volatile unsigned short * biosClock = (volatile unsigned short *)0x0000046CL;
#endif

/*Current engine version*/
#if !defined(LEAF_ENGINE)
#define LEAF_ENGINE 035L
#endif

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

#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
/*Global variable for register I/O (DOS-only)*/
static union REGS in,out;
#endif

/*
The main game context we are working on
*/
leafContext * leafCurrentCtx;

extern unsigned char * videoBuffer;

/*
Time-based random number generator
*/
void seedRandom(void);
signed int generateRandom(void);

/*
Sound functions
*/
signed char initSound(void);
void playSound(unsigned long freq);
void stopSound(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
