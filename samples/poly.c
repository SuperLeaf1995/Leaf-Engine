//
// Leaf-Engine
// Polygon demo
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <leaf/leaf.h>

Leaf_Context ctx;
Leaf_PaletteEntry defaultPal[16] = {
	{0x00,0x00,0x00},
	{0xFF,0xFF,0xFF},
	{0xFF,0x00,0x00},
	{0x00,0xFF,0x00},
	{0x00,0x00,0xFF}
};

signed short poly[16] = {
	64,64,
	32,32,
	64,32,
	16,96
};

int main(void) {
	size_t oscillator = 0;
	size_t oscillate = 1;
	size_t i;
	
	srand(time(NULL));
	
	fprintf(stdout,"Test v1.0\n");

	if(Leaf_ContextCreate(&ctx) != 0) {
		fprintf(stderr,"Error on context\n");
		goto end;
	}
	
	Leaf_SetPalette((Leaf_PaletteEntry *)&defaultPal,4);

	while(ctx.ui != __LEAF_UI_EXIT_CODE) {
		if(oscillate) { oscillator++; }
		else { oscillator--; }
		if(oscillator >= 100) { oscillate = 0; }
		else if(oscillator == 0) { oscillate = 1; }
		
		// Clear the screen
		Leaf_ClearScreen();
		
		for(i = 2; i < (4<<1); i++) {
			if(oscillate) { poly[i]++; }
			else { poly[i]--; }
		}
		
		// Plot a nice polygon
		Leaf_DrawWirePolygon((signed short *)&poly,4,1);
		
		// Update the screen
		Leaf_UpdateScreen();
	}

	end:
	Leaf_ContextDestroy(&ctx);
	return 0;
}
