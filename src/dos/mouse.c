#include <dos.h>
#include "mouse.h"

static union REGS in,out;

/**
@brief Initializes mouse driver/cursor

@param m Mouse structure
*/
char initMouse(struct mouse * m) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	if((in.x.bx&2) != 0) { /*Two button mouse*/
        m->buttons = 2;
	}
	else if((in.x.bx&3) != 0) { /*Three button mouse*/
        m->buttons = 3;
	}
	else { /*Unknown buttons*/
        m->buttons = 0;
	}
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
}

/**
@brief Sets position of the mouse to given parameters

@param x X Coordinates
@param y Y Coordinates
*/
void setMousePosition(register unsigned short x, register unsigned short y) {
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

/**
@brief Shows mouse
*/
void showMouse(void) {
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

/**
@brief Hides mouse
*/
void hideMouse(void) {
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

/**
@brief Gets status of buttons and current position

@param m Mouse structure
*/
void getMouseStatus(struct mouse * m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/**
@brief Gets mouse movement

@param m Mouse structure
*/
void getMouseMovement(struct mouse * m) {
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->mx = out.x.cx;
	m->my = out.x.dx;
	return;
}
