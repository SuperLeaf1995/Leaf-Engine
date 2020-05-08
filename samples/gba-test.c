#include <stdio.h>
#include <stdlib.h>
#include "leaf.c"

leafContext game;

paletteEntry p[255] = {
	{0,0,0}
};

void start(void) {
	size_t i;
	
	leafContextCreate(&game);
	setVideo(0x13);
	
	setPalette((paletteEntry *)&p,4);

	while(1) {
		for(i = 0; i < 32; i++) {
			plotPixel(i,2,0);
		}
		updateScreen();
	}
	
	leafContextDestroy(&game);
	return;
}
