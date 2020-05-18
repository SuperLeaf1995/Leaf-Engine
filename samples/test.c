#include <stdio.h>
#include <stdlib.h>
#include <leaf.h>

leafContext ctx;
Leaf_Image timg;

int main(void)
{
	fprintf(stdout,"Test v1.0\n");

	fprintf(stdout,"Creating context\n");
	if(Leaf_ContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		goto end;
	}
	
	fprintf(stdout,"Creating image\n");
	if(Leaf_imageBitmap("res/test.bmp",&timg) != 0) {
		fprintf(stderr,"Error on imageBitmap\n");
		goto end;
	}
	
	fprintf(stdout,"Setting palette\n");
	Leaf_setPalette(timg.palette,16);

	while(ctx.ui != __LEAF_UI_EXIT_CODE) {
		Leaf_drawImage(&timg,0,0);
		Leaf_updateScreen();
	}

	end:
	fprintf(stdout,"Destroying context\n");
	Leaf_ContextDestroy(&ctx);
	fprintf(stdout,"Destroying image\n");
	Leaf_destroyImage(&timg);
	fprintf(stdout,"End of program\n");
	return 0;
}
