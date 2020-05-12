#include <stdio.h>
#include <stdlib.h>
#include <leaf.h>

extern leafContext * leafCurrentCtx;
leafContext ctx;
Image timg;

int main(void) {
	ctx.vwide = 320;
	ctx.vtall = 200;
	leafContextCreate(&ctx);
	
	if(imageBitmap("res/test.bmp",&timg) != 0) {
		return -1;
	}

	setPalette(timg.palette,16);

	while(1) {
		drawImage(&timg,0,0);
		updateScreen();
	}

	leafContextDestroy(&ctx);
	return 0;
}
