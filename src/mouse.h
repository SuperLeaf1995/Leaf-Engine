#ifndef __LEAF_MOUSE_H__
#define __LEAF_MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Mouse handler structure */
struct mouse {
	unsigned char buttonLeft;
	unsigned char buttonRight;
	unsigned char buttonMiddle;
	unsigned char buttons;
	signed short x; signed short y;
	signed short mx; signed short my;
};

/*
Functions for manipulating the cursor and retrieving data from it.
Using the mouse structure to create a mouse handler.
*/
char initMouse(struct mouse * m);
void setMousePosition(unsigned short x, unsigned short y);
void showMouse(void);
void hideMouse(void);
void getMouseStatus(struct mouse * m);
void getMouseMovement(struct mouse * m);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
