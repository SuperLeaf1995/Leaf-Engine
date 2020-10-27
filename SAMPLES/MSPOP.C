/* Mouse bubble program */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <math.h>
#include <sys/stat.h>

#include "leaf.h"
#include "video.h"
#include "sound.h"
#include "mouse.h"
#include "image.h"

/* Underlying engine */
struct Leaf_Context * ctx;
struct Leaf_Video_Context * vctx;
struct Leaf_Sound_Context * sctx;
struct Leaf_Sound * sample;
struct Leaf_Mouse * mouse;
struct Leaf_Image bimg;
struct Leaf_Image bimg2;

FILE * fp;
unsigned * clock = (unsigned *)0x0000046C;
unsigned start; float t1;

struct bubble {
	int x;
	int y;
};

struct bubble stupidBubbles[20];
size_t cbubble = 0;

int main(void) {
	union REGS r;
	register int i, j, k;
	
	sctx = malloc(sizeof(struct Leaf_Sound_Context));
	if(sctx == NULL) {
		printf("Error allocating sound context\n");
		return 10;
	}
	
	sctx->base = 0x20;
	sctx->irq = 2;
	Leaf_Sound_DriverSB_InitChannel(sctx->base,1,SB_AUTO);
	Leaf_Sound_InitChannel(sctx,1);
	Leaf_Sound_Play(sctx,sample,1);
	if(Leaf_Sound_CreateSample(sample,8000,1)) {
		printf("Error creating sound sample\n");
		return 11;
	}
	
	ctx = malloc(sizeof(struct Leaf_Context));
	if(ctx == NULL) {
		printf("Error allocating context\n");
		return 8;
	}
	vctx = malloc(sizeof(struct Leaf_Video_Context));
	if(vctx == NULL) {
		printf("Error allocating video context\n");
		return 9;
	}
	mouse = malloc(sizeof(struct Leaf_Mouse));
	if(mouse == NULL) {
		printf("Error allocating mouse struct\n");
		return 7;
	}
	
	if(Leaf_Init(ctx)) {
		printf("Error initializing Leaf\n");
		return 1;
	}
	if(Leaf_Video_InitByMode(vctx,320,200,0x13)) {
		printf("Error initializing LeafVideo\n");
		return 2;
	}
	if(Leaf_Mouse_Init(mouse)) {
		printf("Error initializing LeafMouse\n");
		return 12;
	}
	
	if(Leaf_Image_ReadBitmap("DATA\\MOUSE.BMP",&bimg)) {
		printf("Error opening image of smile\n");
		return 13;
	}
	
	fp = fopen("DATA\\MS.RAW","rb");
	if(!fp) {
		printf("Error opening click sound\n");
		return 1;
	}
	
	Leaf_Video_SetPalette(vctx,bimg.palette,bimg.nPaletteElem,0);
	
	for(i = 0; i < 20; i++) {
		stupidBubbles[cbubble].x = 0;
		stupidBubbles[cbubble].y = 0;
	}
	
	while(!kbhit()) {
		int rx,ry;
		Leaf_Mouse_GetStatus(mouse);
		
		rx = (mouse->x+1)/2;
		ry = mouse->y+1;
		
		if(mouse->buttonArray[0]) {
			fread(sample->data,sizeof(char),sample->len,fp);
			Leaf_Sound_Play(sctx,sample,1);
			rewind(fp);
			
			stupidBubbles[cbubble].x = rx;
			stupidBubbles[cbubble].y = ry;
			cbubble++;
			if(cbubble > 20) {
				cbubble = 0;
			}
		}
		
		for(i = 0; i < 20; i++) {
			for(j = 0; j < bimg.h; j++) {
				register unsigned realy = (j*bimg.w);
				for(k = 0; k < bimg.w; k++) {
					register unsigned col = bimg.data[k+realy];
					if(col != 1) {
						Leaf_Video_DrawPixel(vctx,k+stupidBubbles[i].x,j+stupidBubbles[i].y,col);
					}
				}
			}
		}
		
		Leaf_Video_UpdateScreen(vctx);
		Leaf_Video_ClearScreen(vctx);
		
		Leaf_Mouse_Show(0);
	}
	
	r.h.al = 0x03;
	r.h.ah = 0;
	int86(0x10,&r,&r);
	
	Leaf_Sound_DestroySample(sample);
	Leaf_Video_End(vctx);
	Leaf_End(ctx);
	
	fclose(fp);
	free(sample);
	free(mouse);
	free(sctx);
	free(vctx);
	free(ctx);
	return 0;
}
