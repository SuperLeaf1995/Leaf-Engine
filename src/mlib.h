#ifndef MLIB_H
#define MLIB_H

#include <stdio.h>
#include <stdlib.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#include <sys/utsname.h>
#endif

#include <stdint.h>

void fskip(FILE *stream, uint64_t n);
signed int initLeaf(void);
signed int getEvent(void);
void endLeaf(void);

#endif
