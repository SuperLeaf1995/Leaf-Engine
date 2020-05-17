/*
 * Leaf-Engine DOS mouse
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

#include <dos.h>
#include "mouse.h"

static union REGS in,out;

/**
@brief Initializes mouse driver/cursor

@param m Mouse structure
*/
char initMouse(Mouse * m)
{
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	
	/*Two button mouse*/
	if((in.x.bx&2) != 0)
	{
        m->buttons = 2;
	}
	/*Three button mouse*/
	else if((in.x.bx&3) != 0)
	{
        m->buttons = 3;
	}
	/*Unknown buttons*/
	else
	{
        m->buttons = 0;
	}
	/*If the mouse was initialized return 0, else return -1*/
	return ((out.x.ax != 0) ? 0 : -1); 
}

/**
@brief Sets position of the mouse to given parameters

@param x X Coordinates
@param y Y Coordinates
*/
void setMousePosition(register unsigned short x, register unsigned short y)
{
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

/**
@brief Shows mouse
*/
void showMouse(void)
{
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

/**
@brief Hides mouse
*/
void hideMouse(void)
{
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

/**
@brief Gets status of buttons and current position

@param m Mouse structure
*/
void getMouseStatus(Mouse * m)
{
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	
	/*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonLeft = (out.x.bx & 1);
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/**
@brief Gets mouse movement

@param m Mouse structure
*/
void getMouseMovement(Mouse * m)
{
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->mx = out.x.cx;
	m->my = out.x.dx;
	return;
}
