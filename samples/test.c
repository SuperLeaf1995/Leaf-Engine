#include <stdio.h>
#include <stdlib.h>
#include <leaf.h>

leafContext ctx;
Image timg;

void destroyImage(Image * img);

void destroyImage(Image * img) {
	if(img->palette != NULL) {
		free(img->palette);
	}
	if(img->data != NULL) {
		free(img->palette);
	}
	return;
}

int main(void)
{
	ctx.vwide = 320;
	ctx.vtall = 200;
	
	fprintf(stdout,"Test v1.0\n");
	
	if(leafContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		goto end;
	}
	
	if(imageBitmap("res/test.bmp",&timg) != 0) {
		fprintf(stderr,"Error on imageBitmap\n");
		goto end;
	}

	setPalette(timg.palette,16);

	while(ctx.ui != __LEAF_UI_EXIT_CODE) {
		drawImage(&timg,0,0);
		updateScreen();
	}

	end:
	leafContextDestroy(&ctx);
	destroyImage(&timg);
	fprintf(stdout,"End of program\n");
	return 0;
}
