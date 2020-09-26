/*
 * Leaf-Engine DOS graphic
 * Copyright (C) 2020 Jesus A. Diaz <jesusantonio30122016@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 */

#if defined(__TURBOC__) || defined(__BORLANDC__)
#include <dos.h>
#endif
#if defined(__DJGPP__)
#include <sys/nearptr.h>
#endif /* __DJGPP__ */
#include "graphic.h"

static union REGS in,out;

/*The Borland compilers uses inportb()
to avoid further confusions, inp() and outp() are used*/
#if defined(__TURBOC__) || defined(__BORLANDC__)
#define inp(x) inportb(x)
#define outp(x,y) outportb(x,y)
#endif

signed char Leaf_SetVideo(unsigned char v)
{
	in.h.al = v;
	in.h.ah = 0; /*set the video we want*/
	int86(0x10,&in,&out);

	Leaf_CurrentContext->vwide = vtable[v][0];
	Leaf_CurrentContext->vtall = vtable[v][1];
	Leaf_CurrentContext->vvideo = vtable[v][2];

	if(Leaf_CurrentContext->vvideo == __ega || Leaf_CurrentContext->vvideo == __vga) {
		video = (unsigned char *)0xA0000000L;
	}
	else if(Leaf_CurrentContext->vvideo == __cga) {
		video = (unsigned char *)0xB8000000L;
	}

	Leaf_CurrentContext->videoBuffer = (unsigned char *)realloc(Leaf_CurrentContext->videoBuffer,(Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall));
	if(Leaf_CurrentContext->videoBuffer == NULL) {
		return -1;
	}

	return 0;
}

void Leaf_SetPalette(Leaf_PaletteEntry * p,  unsigned short n)
{
	 unsigned short i;
	if(Leaf_CurrentContext->vvideo == __vga) {
		outp(0x03C8,(unsigned char)0); /*send to the vga s that we are going to send palette data*/
		for(i = 0; i < n; i++) {
			outp(0x03C9,(unsigned char)(p[i].r>>2));
			outp(0x03C9,(unsigned char)(p[i].g>>2));
			outp(0x03C9,(unsigned char)(p[i].b>>2));
		}
	}
	else if(Leaf_CurrentContext->vvideo == __ega) {

	}
	else if(Leaf_CurrentContext->vvideo == __cga) {

	}
	return;
}

static void waitRetrace(void) {
	while((inp(0x03DA)&8));
	while(!(inp(0x03DA)&8));
	return;
}

void Leaf_UpdateEvent(void) {
	return;
}

void Leaf_UpdateScreen(void) {
	unsigned long i;
	size_t x; size_t y; size_t offs;
	unsigned char * cgaSelect[2] =
	{
		(unsigned char *)0xB8000000L,
		(unsigned char *)0xBA000000L
	};
	unsigned char bitMask;

	waitRetrace(); /* Wait for VGA retrace */
	if(Leaf_CurrentContext->vvideo == __vga)
	{
		/*in VGA simply copy it to the plain VGA memory*/
		memcpy(video,Leaf_CurrentContext->videoBuffer,(size_t)Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); /* Copy data to VGA */
	}
	else if(Leaf_CurrentContext->vvideo == __ega)
	{
		/*TODO: Add working EGA code*/
		in.h.ah = 0x0C;
		for(i = 0; i < (Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); i++)
		{
			in.x.dx = (i/Leaf_CurrentContext->vwide);
			in.x.cx = (i%Leaf_CurrentContext->vwide);
			in.h.al = Leaf_CurrentContext->videoBuffer[i];
			int86(0x10,&in,&out);
		}
	}
	else if(Leaf_CurrentContext->vvideo == __cga)
	{
#if defined(__386__) || defined(_M_I386) || defined(__i386__) || defined(__32BIT__) /*A 32-bit compiler*/
		/*Use adaptive framebuffer (all-screen)*/
		for(i = 0; i < (Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); i++)
		{
			x = (i%320); y = (i/320);
			bitMask = 0x80>>(x&7);
			offs = ((y>>1)*((Leaf_CurrentContext->vwide)>>3)+(x>>3));
			if(Leaf_CurrentContext->videoBuffer[i])
			{
				*(cgaSelect[y&1]+offs) |= bitMask;
			}
			else
			{
				*(cgaSelect[y&1]+offs) &= ~bitMask;
			}
		}
#else /*A 16-bit compiler*/
		/*Wide the pixels*/
		for(i = 0; i < (Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); i++)
		{
			x = (i%320); y = (i/320);
			bitMask = 0x80>>(x&7);
			offs = ((y>>1)*((Leaf_CurrentContext->vwide<<1)>>3)+(x>>3));
			if(Leaf_CurrentContext->videoBuffer[i])
			{
				*(cgaSelect[y&1]+offs) |= bitMask;
			}
			else
			{
				*(cgaSelect[y&1]+offs) &= ~bitMask;
			}
		}
#endif
	}
    return;
}

void Leaf_ClearScreen(void) {
    memset(Leaf_CurrentContext->videoBuffer,0,(size_t)Leaf_CurrentContext->vwide*Leaf_CurrentContext->vtall); /* Clear our buffer */
    return;
}
