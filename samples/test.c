//
// Leaf-Engine
// Bitmap demo
//

#include <stdio.h>
#include <stdlib.h>
#include <leaf/leaf.h>

Leaf_Context ctx;
Leaf_Image img;

int main(void) {
	srand(time(NULL));
	
	fprintf(stdout,"Test v1.0\n");

	if(Leaf_ContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		goto end;
	}
	
	if(Leaf_ImageBitmap("res/test.bmp",&img) != 0) {
		fprintf(stderr,"Error loading bitmap %i\n",Leaf_ImageBitmap("res/test.bmp",&img));
		goto end;
	}
	
	Leaf_SetPalette(img.palette,img.nPal);

	while(ctx.ui != __LEAF_UI_EXIT_CODE) {
		// Clear the screen
		Leaf_ClearScreen();
		
		Leaf_DrawImage(&img,0,0);
		
		// Update the screen
		Leaf_UpdateScreen();
	}

	end:
	Leaf_DestroyImage(&img);
	Leaf_ContextDestroy(&ctx);
	return 0;
}
