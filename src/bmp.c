/* BITMAP.C
 * Used to read and write Microsoft Bitmaps (Read/Write) and OS/2 bitmaps (Read)
 * */

#include "bmp.h"

int8_t  readImageBitmapHeader(FILE *stream, bmpHeader *e) {
	static uint32_t headerSize,wide,tall,planes,bitsPerPixel,compression,sizeOfImage;
	static uint32_t xPixelsPerMeter,yPixelsPerMeter,numberOfColors;
	static uint32_t importantColors,sizeOfFile,reserved,offset,mask[4];
	if(!stream) { /*check if file is open*/
		return -1; /*nope, bail out!*/
	}
	fread(e->type,sizeof(uint16_t),1,stream); /*read file header*/
	/*check that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) != 0 && strncmp((const char *)e->type,"BA",2) != 0
	&& strncmp((const char *)e->type,"IC",2) != 0 && strncmp((const char *)e->type,"PT",2) != 0
	&& strncmp((const char *)e->type,"CI",2) != 0 && strncmp((const char *)e->type,"CP",2) != 0) {
		return -1; /*invalid bitmap!*/
	}
	fread(&sizeOfFile,sizeof(uint32_t),1,stream);
	fread(&reserved,sizeof(uint32_t),1,stream); /*reserved has an actual mean in OS/2*/
	fread(&offset,sizeof(uint32_t),1,stream);
	fread(&headerSize,sizeof(uint32_t),1,stream);
	e->sizeOfFile = sizeOfFile; /*save everything in the struct pointer thing*/
	e->reserved = reserved;
	e->offset = offset;
	e->headerSize = headerSize;
	/*check the header size*/
	if(headerSize == 40) { /*Windows 3.x bitmap*/
		fread(&wide,sizeof(int32_t),1,stream);
		fread(&tall,sizeof(int32_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
		fread(&compression,sizeof(uint32_t),1,stream);
		fread(&sizeOfImage,sizeof(uint32_t),1,stream);
		fread(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&numberOfColors,sizeof(uint32_t),1,stream);
		fread(&importantColors,sizeof(uint32_t),1,stream);
	} else if(headerSize == 12) { /*OS/2 1.x bitmap*/
		fread(&wide,sizeof(uint16_t),1,stream);
		fread(&tall,sizeof(uint16_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
	} else if(headerSize >= 56 && headerSize <= 64) { /*Windows 95 bitmap*/
		fread(&wide,sizeof(int32_t),1,stream);
		fread(&tall,sizeof(int32_t),1,stream);
		fread(&planes,sizeof(uint16_t),1,stream);
		fread(&bitsPerPixel,sizeof(uint16_t),1,stream);
		fread(&compression,sizeof(uint32_t),1,stream);
		fread(&sizeOfImage,sizeof(uint32_t),1,stream);
		fread(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
		fread(&numberOfColors,sizeof(uint32_t),1,stream);
		fread(&importantColors,sizeof(uint32_t),1,stream);
		fread(&mask[0],sizeof(uint32_t),1,stream);
		fread(&mask[1],sizeof(uint32_t),1,stream);
		fread(&mask[2],sizeof(uint32_t),1,stream);
		fread(&mask[3],sizeof(uint32_t),1,stream);
		/*inmediately place masks, on the struct*/
		e->mask[0] = mask[0];
		e->mask[1] = mask[1];
		e->mask[2] = mask[2];
		e->mask[3] = mask[3];
	} else {
		return -2;
	}
	e->planes = planes;
	e->bitsPerPixel = bitsPerPixel;
	e->compression = compression;
	e->sizeOfImage = sizeOfImage;
	e->xPixelsPerMeter = xPixelsPerMeter;
	e->yPixelsPerMeter = yPixelsPerMeter;
	e->numberOfColors = numberOfColors;
	e->importantColors = importantColors;
	/*check if bit's are valid*/
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 24:
		case 32:
			break;
		default:
			return -3;
	}
	e->wide = wide;
	e->tall = tall;
	if(planes != 1 || numberOfColors > 256) { /*we did something wrong!*/
		return -4;
	}
	return 0;
}

paletteEntry *readImageBitmapPalette(FILE *stream, bmpHeader *b) {
	uint16_t i;
	paletteEntry * pal;
	if(b->bitsPerPixel == 8 || b->bitsPerPixel == 4 || b->bitsPerPixel == 2
	|| b->bitsPerPixel == 1) { /*check if it really has palette...*/
		b->paletteEntries = (1<<(b->bitsPerPixel)); /*number of palette entries to read*/
		pal = (paletteEntry *)malloc(sizeof(paletteEntry)*b->paletteEntries);
		if(pal == NULL) { return NULL; }
		for(i = 0; i < b->paletteEntries; i++) { /*read the palette thing*/
			pal[i].blue = fgetc(stream); /*bitmap has reverse RGB order for each entry*/
			pal[i].green = fgetc(stream);
			pal[i].red = fgetc(stream);
			if(b->headerSize >= 40) { /*later versions require 4 byte palette elements*/
				fgetc(stream); /*padding*/
			}
		}
		return (paletteEntry *)pal; /*return the allocated palette*/
	} else {
		return NULL; /*16 and 24 bit bitmaps dosent has palette*/
	}
}

uint8_t *readImageBitmapData(FILE *stream, bmpHeader *b) {
	static int32_t i,i2;
	static uint16_t hold;
	uint8_t *data;
	if(b->tall == 0 || b->wide == 0) {
		return 0;
	}
	data = (uint8_t *)malloc((b->wide*b->tall)); /*allocate data for bitmap*/
	if(data == NULL) { return 0; }
	switch(b->compression) {
		case 0: /*no compression*/
			switch(b->bitsPerPixel) {
				case 8: /*256 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							fread(&hold,sizeof(uint8_t),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = hold;
						}
						if(((((b->wide*8)+7)>>3)&3)) { /*skip padding (dword)*/
							fseek(stream,3-((((b->wide*8)+7)>>3)&3),SEEK_CUR);
						}
					}
					break;
				case 4: /*16 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							fread(&hold,sizeof(uint8_t),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = (hold>>4)&0x0F;
							i2++;
							data[(i2+((b->tall-i)*b->wide))] = hold&0x0F;
						}
						if(((((b->wide*4)+7)>>3)&3)) {
							fseek(stream,3-((((b->wide*4)+7)>>3)&3),SEEK_CUR);
						}
					}
					break;
				case 2: /*4 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							if((i2&3) == 0) {
								fread(&hold,sizeof(uint8_t),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = (hold>>6)&3;
							hold = hold<<2;
						}
						if(((((b->wide*2)+7)>>3)&3)) {
							fseek(stream,3-((((b->wide*2)+7)>>3)&3),SEEK_CUR);
						}
					}
					break;
				case 1: /*2 colors*/
					for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; i2 < b->wide; i2++) {
							if(!(i2&7)) {
								fread(&hold,sizeof(uint8_t),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = ((hold>>7)&1);
							hold <<= 1;
						}
						if(((((b->wide*1)+7)>>3)&3)) {
							fseek(stream,3-((((b->wide*1)+7)>>3)&3),SEEK_CUR);
						}
					}
					break;
				default:
					free(data); /*de allocate data if we have invalid setup*/
					return 0;
			}
			break;
		default:
			free(data); /*de allocate data if we have invalid setup*/
			return 0;
	}
	return (uint8_t *)data;
}
