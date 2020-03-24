#ifndef LEAF_H
#define LEAF_H

#include <stdio.h>
#include <stdlib.h>
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <unistd.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#include <time.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#error "You can't use Windows to compile Leaf-Engine, use Linux or BSD, or DOSBox. Or add the Windows GUI functions yourself"
#endif

#if defined(__TURBOC__) && !defined(__BORLANDC__)
#define __no_current_dir
#define __no_ctype_h
#include <limits.h>
#endif

#ifndef _exit_code
#define _exit_code 1
#endif

#ifndef _video_auto
#define _video_auto 1
#endif

#if defined(__no_ctype_h)
#include <ctype.h>
#endif

typedef struct paletteEntry {
	unsigned char r;
	unsigned char g;
	unsigned char b;
}paletteEntry;

typedef struct leafEvent {
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	XEvent xevent;
#endif
	signed int key; /*Key scancode (0-255) or SPECIAL_KEY
					(used by Leaf-Engine for arrow keys and stuff)*/
	signed short cx; /*Change of X movement of mouse*/
	signed short cy; /*Change of Y movement of mouse*/
	signed char eventStatus; /*Bit 1, if set, means keypress
							Bit 2, if set means mouse-left button press
							Bit 3, if set means mouse-right button press
							Bit 4, if set means mouse-middle button press*/
	signed char ui; /*UI and window events, always 0 in DOS and monotask OSes*/
}leafEvent;

typedef struct leafGame {
#if defined(__linux) || defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
	Display * xdisplay; /*Main X11 display for our X11 game*/
	Window xwindow; /*Our main window in X11*/
	Atom WM_DELETE_WINDOW; /*"x" atom in window (to close it)*/
	GC xgraphic; /*The X11 graphic context where we will draw on*/
	paletteEntry * xpalette; /*Emulated palette*/
	unsigned char * video; /*Used to emulate a monitor*/
#endif
	const char * name; /*name of the game*/
	unsigned char videoConf; /*used for video mode usage*/
	unsigned short vwide; /*wide of heritage screen*/
	unsigned short vtall; /*height of the screen*/
}leafGame;

signed int generateRandom(void);
signed int leafGameCreate(leafGame * g);
signed int leafEventUpdate(leafGame * g, leafEvent * e);
signed int leafGameDestroy(leafGame * g);
signed int leafSetVideo(leafGame * g);
void plotPixel(leafGame * g, register unsigned char x, register unsigned char y, register unsigned char c);
void plotLinearPixel(leafGame * g, register unsigned short pos,register unsigned char color);
unsigned char fetchPixel(leafGame * g, register unsigned short x,register unsigned short y);
signed int setPalette(leafGame * g, paletteEntry * p, register unsigned short n);

typedef struct _bmpHeader {
	unsigned char type[3];
	unsigned long sizeOfFile;
	unsigned long reserved;
	unsigned long offset;
	unsigned long headerSize;
	signed long wide;
	signed long tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned long compression;
	unsigned long sizeOfImage;
	unsigned long xPixelsPerMeter;
	unsigned long yPixelsPerMeter;
	unsigned long numberOfColors;
	unsigned long importantColors;
	unsigned long mask[4];
	unsigned long paletteEntries;
	paletteEntry *palette;
}bmpHeader;

signed int readImageBitmapHeader(FILE * stream, bmpHeader * e);
paletteEntry * readImageBitmapPalette(FILE * stream, bmpHeader * b);
unsigned char * readImageBitmapData(FILE * stream, bmpHeader * b);

#endif
