#ifndef __EVENT_H__
#define __EVENT_H__

struct Leaf_Event {
	unsigned char type;
	
	union Keyboard {
		int key_down;
		int key_up;
	};
	
	union Mouse {
		int x;
		int y;
		
		char left_button;
		char right_button;
		char middle_button;
	};
};

#endif
