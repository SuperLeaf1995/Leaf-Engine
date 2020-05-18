#include <stdio.h>
#include <stdlib.h>
#include <leaf.h>

leafContext ctx;
Leaf_Image timg;

void Leaf_destroyImage(Leaf_Image * img);

void Leaf_destroyImage(Leaf_Image * img) {
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
	fprintf(stdout,"Test v1.0\n");

	fprintf(stdout,"Creating context\n");
	if(Leaf_ContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		goto end;
	}
	fprintf(stdout,"Buffer: %p\n",(void *)ctx.videoBuffer);
	
	fprintf(stdout,"Creating image\n");
	if(Leaf_imageBitmap("res/test.bmp",&timg) != 0) {
		fprintf(stderr,"Error on imageBitmap\n");
		goto end;
	}
	
	fprintf(stdout,"Buffer: %p\n",(void *)ctx.videoBuffer);
	fprintf(stdout,"Setting palette\n");
	Leaf_setPalette(timg.palette,16);

	fprintf(stdout,"Tall: %u, Wide: %u\n",ctx.vwide,ctx.vtall);
	fprintf(stdout,"Buffer: %p\n",(void *)ctx.videoBuffer);
	fprintf(stdout,"Data: %p, Palette: %p\n",(void *)timg.data,(void *)timg.palette);

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
