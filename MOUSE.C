/* LEAF ENGINE
 * Copyright (C) Jesus Antonio Diaz - 2020
 * Licensed under Apache License, see LICENSE.md
 */

/* MOUSE.C
 * Contains mouse functions (MOUSE.COM driver stuff, useful for clicking
 * actions and stuff that involves mouse interaction*/

#define isMouseOnPosition(x,y,m) (m.x == x) ? ((m.y == y) ? 1 : 0) : 0 /*0 if false, 1 if true*/

void _Cdecl setMouseStatus(uint8_t status) {
	in.x.ax = status; /*See MOUSE_STATUS_HIDE and MOUSE_STATUS_SHOW for more information*/
	int86(0x33,&in,&out); /*issue an interrupt*/
	return;
}

int8_t _Cdecl initMouse(struct mouse *m) {
	in.x.ax = 0x00; /*ax 0 and bx 0 = mouse.com initializes mouse*/
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	/*set the correct button stuff*/
	if((in.x.bx&2) != 0) {
		m->buttons = 2;
	} else if((in.x.bx&2) != 0) {
		m->buttons = 3; /*posibly some weird mouse*/
	} else {
		m->buttons = 0; /*unknown!*/
	}
	if(out.x.ax != 0) { /*if mouse was initialized...*/
		setMouseStatus(MOUSE_STATUS_SHOW); /*...automaticaly show mouse*/
	}
	return (out.x.ax != 0) ? 0 : -1; /*if the mouse was initialized return 0, else return -1*/
}

void _Cdecl setMousePosition(uint16_t x,uint16_t y) {
	in.x.ax = 4; /*ax: 4, set mouse coordinates*/
	in.x.cx = x; /*set the coordinates and stuff*/
	in.x.dx = y;
	int86(0x33,&in,&out); /*issue the interrupt*/
	return;
}

void _Cdecl getMouseStatus(struct mouse *m) {
	in.x.ax = 0x03; /*command 0x03, get mouse info*/
	int86(0x33,&in,&out); /*issue the interrupt*/
	m->x = out.x.cx; /*x coords*/
	m->y = out.x.dx; /*y coords*/
	m->buttonLeft = (out.x.bx & 1); /*while it is not equal 0, its*/
	m->buttonRight = (out.x.bx & 2); /*HOLD/PRESSED, else its RELEASED*/
	m->buttonMiddle = (out.x.bx & 4);
	return;
}
