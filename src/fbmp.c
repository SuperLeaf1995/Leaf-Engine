#include "fbmp.h"

signed int readImageBitmapHeader(FILE * stream, bmpHeader * e) {
	unsigned long sizeOfFile;
	unsigned long reserved;
	unsigned long offset;
	unsigned long headerSize;
	signed long wide;
	signed long tall;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned long compression;
	unsigned long sizeOfImage;
	unsigned long xPixelsPerMeter;
	unsigned long yPixelsPerMeter;
	unsigned long numberOfColors;
	unsigned long importantColors;
	unsigned long mask[4];
	if(!stream) { /*check if file is open*/
		return -1; /*nope, bail out!*/
	}
	fread(e->type,sizeof(unsigned short),1,stream); /*read file header*/
	/*check that it is a actual bitmap*/
	if(strncmp((const char *)e->type,"BM",2) != 0
	&& strncmp((const char *)e->type,"BA",2) != 0
	&& strncmp((const char *)e->type,"IC",2) != 0
	&& strncmp((const char *)e->type,"PT",2) != 0
	&& strncmp((const char *)e->type,"CI",2) != 0
	&& strncmp((const char *)e->type,"CP",2) != 0) {
		return -2; /*invalid bitmap!*/
	}
	if(fread(&sizeOfFile,sizeof(unsigned long),1,stream) != 1) {
		return -3;
	}
	if(fread(&reserved,sizeof(unsigned long),1,stream) != 1) { /*reserved has an actual mean in OS/2*/
		return -4;
	}
	fread(&offset,sizeof(unsigned long),1,stream);
	fread(&headerSize,sizeof(unsigned long),1,stream);
	/*check the header size*/
	if(headerSize == 40) { /*Windows 3.x bitmap*/
		fread(&wide,sizeof(signed long),1,stream);
		fread(&tall,sizeof(signed long),1,stream);
		fread(&planes,sizeof(unsigned short),1,stream);
		fread(&bitsPerPixel,sizeof(unsigned short),1,stream);
		fread(&compression,sizeof(unsigned long),1,stream);
		fread(&sizeOfImage,sizeof(unsigned long),1,stream);
		fread(&xPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&yPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&numberOfColors,sizeof(unsigned long),1,stream);
		fread(&importantColors,sizeof(unsigned long),1,stream);
	} else if(headerSize == 12) { /*OS/2 1.x bitmap*/
		fread(&wide,sizeof(unsigned short),1,stream);
		fread(&tall,sizeof(unsigned short),1,stream);
		fread(&planes,sizeof(unsigned short),1,stream);
		fread(&bitsPerPixel,sizeof(unsigned short),1,stream);
	} else if(headerSize >= 56 && headerSize <= 64) { /*Windows 95 bitmap*/
		fread(&wide,sizeof(signed long),1,stream);
		fread(&tall,sizeof(signed long),1,stream);
		fread(&planes,sizeof(unsigned short),1,stream);
		fread(&bitsPerPixel,sizeof(unsigned short),1,stream);
		fread(&compression,sizeof(unsigned long),1,stream);
		fread(&sizeOfImage,sizeof(unsigned long),1,stream);
		fread(&xPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&yPixelsPerMeter,sizeof(unsigned long),1,stream);
		fread(&numberOfColors,sizeof(unsigned long),1,stream);
		fread(&importantColors,sizeof(unsigned long),1,stream);
		fread(&mask[0],sizeof(unsigned long),1,stream);
		fread(&mask[1],sizeof(unsigned long),1,stream);
		fread(&mask[2],sizeof(unsigned long),1,stream);
		fread(&mask[3],sizeof(unsigned long),1,stream);
		e->mask[0] = mask[0]; /*inmediately place masks, on the struct*/
		e->mask[1] = mask[1];
		e->mask[2] = mask[2];
		e->mask[3] = mask[3];
	} else {
		return -5;
	}
	/*check if bit's are valid*/
	if((bitsPerPixel != 1)
	&& (bitsPerPixel != 2)
	&& (bitsPerPixel != 4)
	&& (bitsPerPixel != 8)
	&& (bitsPerPixel != 16)
	&& (bitsPerPixel != 24)
	&& (bitsPerPixel != 32)) {
		return -6;
	}
	if(planes != 1) { /*we did something wrong!*/
		return -7;
	}
	if(numberOfColors > 256) {
		return -8;
	}
	e->planes = planes; /*save everything in the struct pointer thing*/
	e->bitsPerPixel = bitsPerPixel;
	e->compression = compression;
	e->sizeOfImage = sizeOfImage;
	e->xPixelsPerMeter = xPixelsPerMeter;
	e->yPixelsPerMeter = yPixelsPerMeter;
	e->numberOfColors = numberOfColors;
	e->importantColors = importantColors;
	e->wide = wide;
	e->tall = tall;
	e->sizeOfFile = sizeOfFile;
	e->reserved = reserved;
	e->offset = offset;
	e->headerSize = headerSize;
	return 0;
}

paletteEntry * readImageBitmapPalette(FILE *stream, bmpHeader * b) {
	unsigned short i;
	paletteEntry * pal;
	if(b->bitsPerPixel == 8 || b->bitsPerPixel == 4 || b->bitsPerPixel == 2
	|| b->bitsPerPixel == 1) { /*check if it really has palette...*/
		b->paletteEntries = (1<<(b->bitsPerPixel)); /*number of palette entries to read*/
		pal = (paletteEntry *)malloc((sizeof(paletteEntry)*b->paletteEntries));
		if(pal == NULL) { return NULL; }
		for(i = 0; i < b->paletteEntries-3; i++) { /*read the palette thing*/
			pal[i].b = fgetc(stream); /*bitmap has reverse RGB order for each entry*/
			pal[i].g = fgetc(stream);
			pal[i].r = fgetc(stream);
			if(b->headerSize >= 40) { /*later versions require 4 byte palette elements*/
				fgetc(stream); /*padding*/
			}
		}
		return (paletteEntry *)pal; /*return the allocated palette*/
	} else {
		return NULL; /*16 and 24 bit bitmaps dosent has palette*/
	}
}

unsigned char * readImageBitmapData(FILE *stream, bmpHeader * b) {
	unsigned long i,i2;
	unsigned char hold;
	unsigned char *data;
	if((signed long)b->tall <= 0 || (signed long)b->wide <= 0) {
		return NULL;
	}
	data = (unsigned char *)malloc((b->wide*b->tall)); /*allocate data for bitmap*/
	if(data == NULL) {
		return NULL;
	}
	switch(b->compression) {
		case 0: /*no compression*/
			switch(b->bitsPerPixel) {
				case 8: /*256 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = hold;
						}
						if(((((b->wide*8)+7)>>3)&3)) { /*skip padding (dword)*/
							fseek(stream,SEEK_CUR,3-((((b->wide*8)+7)>>3)&3));
						}
					}
					break;
				case 4: /*16 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,stream);
							data[(i2+((b->tall-i)*b->wide))] = (hold>>4)&0x0F;
							i2++;
							data[(i2+((b->tall-i)*b->wide))] = hold&0x0F;
						}
						if(((((b->wide*4)+7)>>3)&3)) {
							fseek(stream,SEEK_CUR,3-((((b->wide*4)+7)>>3)&3));
						}
					}
					break;
				case 2: /*4 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							if(!(i2&3)) {
								fread(&hold,sizeof(unsigned char),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = (hold>>6)&3;
							hold = hold<<2;
						}
						if(((((b->wide*2)+7)>>3)&3)) {
							fseek(stream,SEEK_CUR,3-((((b->wide*2)+7)>>3)&3));
						}
					}
					break;
				case 1: /*2 colors*/
					for(i = 1; (signed long)i < b->tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < b->wide; i2++) {
							if(!(i2&7)) {
								fread(&hold,sizeof(unsigned char),1,stream);
							}
							data[(i2+((b->tall-i)*b->wide))] = ((hold>>7)&1);
							hold <<= 1;
						}
						if(((((b->wide*1)+7)>>3)&3)) {
							fseek(stream,SEEK_CUR,3-((((b->wide*1)+7)>>3)&3));
						}
					}
					break;
				default:
					free(data); /*de allocate data if we have invalid setup*/
					return NULL;
			}
			break;
		default:
			free(data);
			return NULL;
	}
	return (unsigned char * )data;
}
