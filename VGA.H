/* LEAF ENGINE
 * Copyright (C) Jesus Antonio Diaz - 2020
 * Licensed under Apache License, see LICENSE.md
 */

/* VGA.H
 * Header for VGA memory and routines for plotting pixels and stuff*/

static uint8_t far *videoMemory = (uint8_t far *)0xA0000000L;

#define plotPixel(x,y,color) videoMemory[(y<<8)+(y<<6)+x] = color
#define plotLinearPixel(pos,color) videoMemory[pos] = color
#define fetchPixel(x,y) videoMemory[(y<<8)+(y<<6)+x]
