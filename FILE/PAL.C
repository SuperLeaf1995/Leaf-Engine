/* LEAF ENGINE
 * Copyright (C) Jesus Antonio Diaz - 2020
 * Licensed under Apache License, see LICENSE.md
 */

/* PALETTE.C
 * Reads and writes palette files (*.pal), can be used to transport palette
 * files betwen different VGA games*/

void _Cdecl writePaletteFile(FILE *stream,paletteEntry *paletteInfo) {
	static uint16_t i;
	for(i = 0; i < 256; i++) { /*loop for 256 colors (should equal to a file of 768 bytes!)*/
		fwrite((uint8_t *)paletteInfo[i].red,sizeof(uint8_t),1,stream);
		fwrite((uint8_t *)paletteInfo[i].green,sizeof(uint8_t),1,stream);
		fwrite((uint8_t *)paletteInfo[i].blue,sizeof(uint8_t),1,stream);
	}
}

paletteEntry * _Cdecl readPaletteFile(FILE *stream) {
	paletteEntry *pal; /*define a struct pointer to allocate it and save stuff on it*/
	static uint16_t i;
	pal = malloc(sizeof(paletteEntry)*256); /*allocate the stuff*/
	if(pal == NULL) { return 0; } /*not engough memory*/
	for(i = 0; i < 256; i++) { /*loop and put stuff*/
		fread((uint8_t *)&pal[i].red,sizeof(uint8_t),1,stream);
		fread((uint8_t *)&pal[i].green,sizeof(uint8_t),1,stream);
		fread((uint8_t *)&pal[i].blue,sizeof(uint8_t),1,stream);
	}
	return pal;
}
