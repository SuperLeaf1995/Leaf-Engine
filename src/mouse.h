/*
 * Leaf-Engine MOUSE Header
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

#ifndef __LEAF_MOUSE_H__
#define __LEAF_MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Mouse handler structure */
typedef struct _Mouse
{
	unsigned char buttonLeft;
	unsigned char buttonRight;
	unsigned char buttonMiddle;
	unsigned char buttons;
	signed short x; signed short y;
	signed short mx; signed short my;
}Mouse;

/*
Functions for manipulating the cursor and retrieving data from it.
Using the mouse structure to create a mouse handler.
*/
char Leaf_initMouse(Mouse * m);
void Leaf_setMousePosition(unsigned short x, unsigned short y);
void Leaf_showMouse(void);
void Leaf_hideMouse(void);
void Leaf_getMouseStatus(Mouse * m);
void Leaf_getMouseMovement(Mouse * m);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
