#ifndef PCSPEAK_H
#define PCSPEAK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#endif

#include "stddefc.h"

void soundPlay(uint32_t x);
void soundStop(void);
int8_t soundPlayRawSoundFile(FILE *stream);

#endif
