#ifndef __MOUSE_H__
#define __MOUSE_H__

struct Leaf_Mouse {
	int buttons;
	char * buttonArray;
	
	unsigned x;
	unsigned y;
};

int Leaf_Mouse_Init(struct Leaf_Mouse * m);
int Leaf_Mouse_Show(int show);
int Leaf_Mouse_SetPos(struct Leaf_Mouse * m, int x, int y);
int Leaf_Mouse_GetStatus(struct Leaf_Mouse * m);

#endif
