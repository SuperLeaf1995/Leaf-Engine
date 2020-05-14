#ifndef __LEAF_LEAF_H__
#define __LEAF_LEAF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <float.h>
#if defined(__linux) || defined(linux)
#include <unistd.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#include <AL/al.h>
#include <AL/alc.h>
#endif

/*
Codes for UI (Always check those, In DOS the exit code is produced
by Ctrl+C)
*/
#define __LEAF_UI_EXIT_CODE 1

/*allows easy-detection and on-demand video switching without
using multiple versions*/
#define __cga	0x01
#define __vga	0x03
#define __ega	0x07
#define __mcga	0x0F

typedef struct paletteEntry {
	/** Red component of the palette */
	unsigned char r;
	/** Green component of the palette */
	unsigned char g;
	/** Blue component of the palette */
	unsigned char b;
}paletteEntry;

typedef struct _Image {
	unsigned long wide;
	unsigned long tall;
	unsigned char bitsPerPixel;
	unsigned char * data;
	
	paletteEntry * palette;
}Image;

typedef struct leafContext {
#if defined(OPENAL)
	ALCdevice * alDev;
	ALCcontext * alCtx;
	ALuint alSoundSrc;
#endif
#if defined(__linux) || defined(linux)
	XEvent xevent; /*Event handler*/
	Display * xdisplay; /*Main X11 display for our X11 game*/
	Window xwindow; /*Our main window in X11*/
	Atom WM_DELETE_WINDOW; /*"x" atom in window (to close it)*/
	GC xgraphic; /*The X11 graphic context where we will draw on*/
#endif
#if defined(__GBA__) || defined(__linux) || defined(linux)
	/** Emulate a VGA palette for newer systems */
	paletteEntry * rgbPalette; /*Emulate a VGA palette*/
#endif
	/** Name of the game, only displays on UI systems */
	char * name;
	/** Video mode usage */
	unsigned char videoConf;
	/** Current video wide */
	unsigned short vwide;
	/** Current video tall */
	unsigned short vtall;
	/** Current video mode (Always VGA on UI systems) */
	unsigned char vvideo;
	/** Used for UI-break case, Ctrl+C in DOS makes this 1 */
	signed char ui;
	unsigned char * videoBuffer;
}leafContext;

struct mouse {
	unsigned char buttonLeft;
	unsigned char buttonRight;
	unsigned char buttonMiddle;
	unsigned char buttons;
	signed short x; signed short y;
	signed short mx; signed short my;
};

extern unsigned char * videoBuffer;
extern unsigned char * video;

extern signed char setVideo(unsigned char v);
extern void plotPixel(register unsigned short x, register unsigned short y, register unsigned char c);
extern void plotLinearPixel(register unsigned short pos,register unsigned char color);
extern unsigned char fetchPixel(register unsigned short x,register unsigned short y);
extern unsigned char fetchLinearPixel(register unsigned short p);
extern void setPalette(paletteEntry * p, register unsigned short n);
extern void waitRetrace(void);

extern void plotLine(register signed short sx, register signed short sy, register signed short ex, register signed short ey, register unsigned char c);
extern void plotWireSquare(register signed short x1, register signed short y1, register signed short x2, register signed short y2, register unsigned char c);
extern void plotWirePolygon(signed short * d, register unsigned short n, register unsigned char c);
extern void updateEvent(void);
extern void updateScreen(void);
extern void drawImage(Image * img, register unsigned short x, register unsigned short y);
extern void drawTiled(Image * img, unsigned short x, unsigned short y, unsigned short ix, unsigned short iy);

extern void seedRandom(void);
extern signed int generateRandom(void);

extern signed int leafContextCreate(leafContext * g);
extern signed int leafContextDestroy(leafContext * g);
extern signed int leafSetVideo(leafContext * g);

extern char initMouse(struct mouse * m);
extern void setMousePosition(unsigned short x, unsigned short y);
extern void showMouse(void);
extern void hideMouse(void);
extern void getMouseStatus(struct mouse * m);
extern void getMouseMovement(struct mouse * m);

extern signed char initSound(void);
extern void playSound(unsigned long freq);
extern void stopSound(void);

extern signed int imageBitmap(const char * filename, Image * img);

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

/*Yet another obsolete function. With double-buffer mouse, redraw is
almost unessesary and useless*/
#define redrawOnMouse(m) 0

#endif

#endif
