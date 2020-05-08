#ifndef __LEAF_CONTEXT_H__
#define __LEAF_CONTEXT_H__

#if defined(__linux) || defined(linux)
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "palette.h"

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
}leafContext;

#endif