/* LEAF ENGINE
 * Copyright (C) Jesus Antonio Diaz - 2020
 * Licensed under Apache License, see LICENSE.md
 */

/* BITMAP.C
 * Used to read and write Microsoft Bitmaps (Read/Write) and OS/2 bitmaps (Read)
 * */
 
typedef struct bmpHeader {
	uint8_t type[3]; /*file header*/
	uint32_t sizeOfFile;
	uint32_t reserved;
	uint32_t offset;
	uint32_t headerSize;
	int32_t wide; /*bitmap header*/
	int32_t tall;
	uint16_t planes;
	uint16_t bitsPerPixel;
	uint32_t compression;
	uint32_t sizeOfImage;
	uint32_t xPixelsPerMeter;
	uint32_t yPixelsPerMeter;
	uint32_t numberOfColors;
	uint32_t importantColors;
	uint32_t mask[4];
	
	uint8_t paletteEntries;
	paletteEntry *palette;
}bmpHeader;

int8_t _Cdecl readImageBitmapHeader(FILE *stream, bmpHeader *e) {
	static uint32_t headerSize,wide,tall,planes,bitsPerPixel,compression,sizeOfImage;
	static uint32_t xPixelsPerMeter,yPixelsPerMeter,numberOfColors;
	static uint32_t importantColors,sizeOfFile,reserved,offset,mask[4];
	static uint8_t palEntries;
	static uint16_t i;
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
	if(wide < 0 || tall < 0 || planes != 1 || numberOfColors > 256) { /*we did something wrong!*/
		return -4;
	}
	if((uint64_t)wide > ((uint64_t)(INT32_MAX/bitsPerPixel))
	|| (uint64_t)wide > ((uint64_t)(INT32_MAX/abs(tall))/4)) { /*avoid overflows*/
		return -5;
	}
	b->paletteEntries = (1<<(b->bitsPerPixel)); /*number of palette entries to read*/
	b->palette = malloc(sizeof(paletteEntry)*4);
	if(b->palette == NULL) {
		return 0;
	}
	for(i = 0; i < b->paletteEntries; i++) { /*read the palette thing*/
		b->palette[i]->blue = fgetc(stream); /*bitmap has reverse RGB order for each entry*/
		b->palette[i]->green = fgetc(stream);
		b->palette[i]->red = fgetc(stream);
		if(b->headerSize >= 40) { /*later versions require 4 byte palette elements*/
			fgetc(stream); /*padding*/
		}
	}
	return 0;
}

uint8_t * _Cdecl readImageBitmapData(FILE *stream, bmpHeader *b) {
	static uint32_t i,i2;
	static uint16_t hold;
	uint8_t *data;
	if(b->tall == 0 || b->wide == 0) {
		return 0;
	}
	data = (uint8_t *)malloc((b->wide*b->tall)); /*allocate data for bitmap*/
	if(data == NULL) { return 0; }
	switch(b->bitsPerPixel) {
		case 8: /*256 colors*/
			for(i = 1; i < b->tall+1; i++) { /*reverse read wide, but not tall*/
				for(i2 = 0; i2 < b->wide; i2++) {
					fread(&hold,sizeof(uint8_t),1,stream);
					data[(i2+((b->tall-i)*b->wide))] = hold;
				}
				if(((((b->wide*8)+7)>>3)&3)) { /*skip padding (dword)*/
					fskip(stream,3-((((b->wide*8)+7)>>3)&3));
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
					fskip(stream,3-((((b->wide*4)+7)>>3)&3));
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
					fskip(stream,3-((((b->wide*2)+7)>>3)&3));
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
					fskip(stream,3-((((b->wide*1)+7)>>3)&3));
				}
			}
			break;
		default:
			free(data); /*de allocate data if we have invalid setup*/
			return 0;
	}
	return (uint8_t *)data;
}

void _Cdecl writeImageBitmap(FILE *stream, bmpHeader *bih, uint8_t *data, paletteEntry *pal) {
	static uint32_t headerSize,wide,tall,planes,bitsPerPixel,compression,sizeOfImage;
	static uint32_t xPixelsPerMeter,yPixelsPerMeter,numberOfColors;
	static uint32_t importantColors,sizeOfFile,reserved,offset,mask[4];
	static uint32_t i,i2;
	static uint16_t hold;
	sizeOfFile = bih->sizeOfFile;
	reserved = bih->reserved;
	offset = bih->offset;
	headerSize = bih->headerSize;
	wide = bih->wide;
	tall = bih->tall;
	planes = bih->planes;
	bitsPerPixel = bih->bitsPerPixel;
	compression = bih->compression;
	sizeOfImage = bih->sizeOfImage;
	xPixelsPerMeter = bih->xPixelsPerMeter;
	yPixelsPerMeter = bih->yPixelsPerMeter;
	numberOfColors = bih->numberOfColors;
	importantColors = bih->importantColors;
	fwrite(bih->type,sizeof(uint16_t),1,stream);
	fwrite(&sizeOfFile,sizeof(uint32_t),1,stream);
	fwrite(&reserved,sizeof(uint32_t),1,stream);
	fwrite(&offset,sizeof(uint32_t),1,stream);
	fwrite(&headerSize,sizeof(uint32_t),1,stream);
	fwrite(&wide,sizeof(int32_t),1,stream);
	fwrite(&tall,sizeof(int32_t),1,stream);
	fwrite(&planes,sizeof(uint16_t),1,stream);
	fwrite(&bitsPerPixel,sizeof(uint32_t),1,stream);
	fwrite(&compression,sizeof(uint32_t),1,stream);
	fwrite(&sizeOfImage,sizeof(uint32_t),1,stream);
	fwrite(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
	fwrite(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
	fwrite(&numberOfColors,sizeof(uint32_t),1,stream);
	fwrite(&importantColors,sizeof(uint32_t),1,stream);
	for(i = 0; i < 256; i++) { /*palette*/
		fwrite((uint8_t *)pal[i].blue,sizeof(uint8_t),1,stream); /*reverse palette order*/
		fwrite((uint8_t *)pal[i].green,sizeof(uint8_t),1,stream);
		fwrite((uint8_t *)pal[i].red,sizeof(uint8_t),1,stream);
		fwrite(&i,sizeof(uint8_t),1,stream); /*padding, lets think it's an index number*/
	}
	/*bitmap data*/
	for(i = 1; i < bih->tall+1; i++) { /*reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < bih->wide; i2++) { /*we just replotting our shit back again!*/
			hold = data[(i2+((bih->tall-i)*bih->wide))];
			fwrite(&hold,sizeof(uint8_t),1,stream);
		}
	}
	return;
}

