#ifndef MOUSE_H
#define MOUSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#include <dos.h>
#endif
#if defined(__linux) || defined(linux)
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#include "stddefc.h"

#define MOUSE_STATUS_HIDE 2
#define MOUSE_STATUS_SHOW 1

typedef struct mouse {
	uint8_t buttonLeft;
	uint8_t buttonRight;
	uint8_t buttonMiddle;
	uint8_t buttons;
	int16_t x;
	int16_t y;
}mouse;

void setMouseStatus(uint8_t status);
int8_t initMouse(struct mouse *m);
void setMousePosition(uint16_t x,uint16_t y);
void getMouseStatus(struct mouse *m);
void updateMouse(struct mouse *m);

#endif
