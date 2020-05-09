#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <leaf.h>

leafContext ctx;

int main(void) {
	leafContextCreate(&ctx);
	setVideo(0x13);

	while(!kbhit()) {
		gotoxy(1,1);
		printf("hello world");
	}

	setVideo(0x03);
	leafContextDestroy(&ctx);
	return 0;
}