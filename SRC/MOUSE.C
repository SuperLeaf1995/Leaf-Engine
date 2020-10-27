#include <stdlib.h>
#if defined(__DJGPP__) || defined(__GO32__)
#include <dpmi.h>
#else
#include <dos.h>
#endif

#include "mouse.h"

#if defined(__DJGPP__) || defined(__GO32__)
#define regStruct __dpmi_regs
#else
#define regStruct union REGS
#endif

#if defined(__DJGPP__) || defined(__GO32__)
#define dointr(ino,r) __dpmi_int(ino,r);
#else
#define dointr(ino,r) int86(ino,r,r);
#endif

int Leaf_Mouse_Init(struct Leaf_Mouse * m) {
	regStruct r;
	
	/* Initialzie the DOS Mouse driver*/
	r.x.ax = 0x00;
	r.x.bx = 0x00;
	dointr(0x33,&r);
	
	/* Obtain buttons of the mouse*/
	if((r.x.bx&2) != 0) {
		m->buttons = 2;
	} else if((r.x.bx&3) != 0) {
		m->buttons = 3;
	} else {
		m->buttons = 5;
	}
	
	m->buttonArray = malloc(m->buttons*sizeof(char));
	if(m->buttonArray == NULL) {
		return 1;
	}
	
	/*If the mouse was initialized return 0, else return -1*/
	return ((r.x.ax != 0) ? 0 : 1);
}

int Leaf_Mouse_Show(int show) {
	regStruct r;
	
	r.x.ax = 0x01+show;
	dointr(0x33,&r);
	return 0;
}

int Leaf_Mouse_SetPos(struct Leaf_Mouse * m, int x, int y) {
	regStruct r;
	
	r.x.ax = 0x04;
	r.x.cx = x;
	r.x.dx = y;
	dointr(0x33,&r);
	
	m->x = x;
	m->y = y;
	return 0;
}

int Leaf_Mouse_GetStatus(struct Leaf_Mouse * m) {
	regStruct r;
	int i;
	
	r.x.ax = 0x03;
	dointr(0x33,&r);
	
	m->x = r.x.cx;
	m->y = r.x.dx;
	
	for(i = 0; i < m->buttons; i++) {
		m->buttonArray[i] = (r.x.bx&((i<<1)+1));
	}
	return 0;
}
