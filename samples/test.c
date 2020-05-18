#include <stdio.h>
#include <stdlib.h>
#include <leaf.h>

leafContext ctx;
Leaf_Image timg;

int main(void)
{
	fprintf(stdout,"Test v1.0\n");

	if(Leaf_ContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		goto end;
	}

	if(Leaf_imageBitmap("res/test.bmp",&timg) != 0) {
		fprintf(stderr,"Error on imageBitmap\n");
		goto end;
	}

	Leaf_setPalette(timg.palette,timg.nPal);

	while(ctx.ui != __LEAF_UI_EXIT_CODE) {
		Leaf_drawImage(&timg,64,64);
		Leaf_updateScreen();
	}

	end:
	Leaf_ContextDestroy(&ctx);
	Leaf_destroyImage(&timg);
	return 0;
}
