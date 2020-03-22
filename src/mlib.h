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
#include <sys/utsname.h>
#endif

void fskip(FILE *stream, uint64_t n);
void initLeaf(void);
void endLeaf(void);

#endif
