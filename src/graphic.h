#ifndef __LEAF_GRAPHIC_H__
#define __LEAF_GRAPHIC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__linux) || defined(linux)
#include <unistd.h>
#include <sys/utsname.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#endif
#include "palette.h"
#include "context.h"
#include "image.h"

/*allows easy-detection and on-demand video switching without
using multiple versions*/
#define __cga	0x01
#define __vga	0x03
#define __ega	0x07
#define __mcga	0x0F

static unsigned short vtable[32][3] = {
	{0,0,0}, /* 0x00 */
	{0,0,0}, /* 0x01 */
	{0,0,0}, /* 0x02 */
	{0,0,0}, /* 0x03 */
	{320,200,__cga}, /* 0x04 */
	{0,0,0}, /* 0x05 */
	{640,400,__cga}, /* 0x06 */
	{0,0,0}, /* 0x07 */
	{0,0,0}, /* 0x08 */
	{0,0,0}, /* 0x09 */
	{0,0,0}, /* 0x0A */
	{0,0,0}, /* 0x0B */
	{0,0,0}, /* 0x0C */
	{320,200,__ega}, /* 0x0D */
	{640,200,__ega}, /* 0x0E */
	{640,350,__ega}, /* 0x0F */
	{640,400,__ega}, /* 0x10 */
	{640,480,__mcga}, /* 0x11 */
	{640,480,__vga}, /* 0x12 */
	{320,200,__vga}, /* 0x13 */
	{0,0,0} /* 0x14 */
};

#if defined(__DJGPP__)
unsigned char * video = (unsigned char * )0xA0000+__djgpp_conventional_base;
#elif defined(__TURBOC__) || defined(__BORLANDC__)
unsigned char * video = (unsigned char * )0xA0000000L;
#elif defined(__APPLE2__)
unsigned char * video = (unsigned char * )0x2000;
#elif defined(__GBA__)
unsigned short * video = (unsigned short *)0x6000000;
#endif

extern leafContext * leafCurrentCtx;

/*
General graphics manipulation functions.
Comaptible with almost all displays (VGA,EGA,CGA,etc).
*/
unsigned int setVideo(unsigned char v);
void plotPixel(register unsigned short x, register unsigned short y, register unsigned char c);
void plotLinearPixel(register unsigned short pos,register unsigned char color);
unsigned char fetchPixel(register unsigned short x,register unsigned short y);
void setPalette(paletteEntry * p, register unsigned short n);
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
static void waitRetrace(void);
#endif

/*
Graphics manipulation routines
This routines does not directly modify video, but they rely on the
principal graphics functions
*/
void plotLine(register signed short sx, register signed short sy, register signed short ex, register signed short ey, register unsigned char c);
void plotWireSquare(register signed short x1, register signed short y1, register signed short x2, register signed short y2, register unsigned char c);
void plotWirePolygon(signed short * d, register unsigned short n, register unsigned char c);
void updateScreen(void);
void drawImage(Image * img, register unsigned short x, register unsigned short y);
void drawSprite(unsigned char * data, register unsigned short x, register unsigned short y, register unsigned short sx, register unsigned short sy);
void drawTiledSprite(unsigned char * data, unsigned short x, register unsigned short y, register unsigned short sx, register unsigned short sy, register unsigned short ix, register unsigned short iy, register unsigned short tx, register unsigned short ty);

#endif
