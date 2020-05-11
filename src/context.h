#ifndef __LEAF_CONTEXT_H__
#define __LEAF_CONTEXT_H__

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

#include "palette.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
Codes for UI (Always check those, In DOS the exit code is produced
by Ctrl+C)
*/
#define _exit_code 1

/*
LeafGame is another special structure wich is the main structure for
main operations. This structure should only be in programs wich require
leafEvent and any of the functions that depends of it.

Provides a simple way for porting DOS, Linux and BSD applications without
the hassle of setting everything separately
*/
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
	unsigned char * video; /*Used to emulate a monitor*/
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
}leafContext;

signed int leafContextCreate(leafContext * g);
signed int leafContextDestroy(leafContext * g);
signed int leafSetVideo(leafContext * g);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
