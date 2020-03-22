/* LEAF.H
 * Main leaf file*/

#ifndef LEAF_H
#define LEAF_H

#include <stdio.h>
#include <stdlib.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#endif

#include "stddefc.h"

#include "video.h"
#include "mouse.h"
#include "bmp.h"
#include "pcspeak.h"
#include "tiff.h"
#include "pcx.h"
#include "mlib.h"

#endif
