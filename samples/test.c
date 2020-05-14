#include <stdio.h>
#include <stdlib.h>
#include <leaf.h>

extern leafContext * leafCurrentCtx;
leafContext ctx;
Image timg;

int main(void) {
	ctx.vwide = 320;
	ctx.vtall = 200;
	if(leafContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		return -1;
	}
	
	if(imageBitmap("res/test.bmp",&timg) != 0) {
		fprintf(stderr,"Error on imageBitmap\n");
		return -2;
	}

	setPalette(timg.palette,16);

	for(;;) {
		drawImage(&timg,0,0);
		
		updateScreen();
	}

	leafContextDestroy(&ctx);
	fprintf(stdout,"End of program\n");
	return 0;
}
