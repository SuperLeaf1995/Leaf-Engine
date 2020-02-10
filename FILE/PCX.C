/*PCX.C
 *ZSoft paintbrush file format parser and reader*/

typedef struct pcxHeader {
	uint8_t type;
	uint8_t version;
	uint8_t compression;
	uint8_t bitsPerPixel;
	uint16_t xStart;
	uint16_t yStart;
	uint16_t xEnd;
	uint16_t yEnd;
	uint16_t horizontalResolution;
	uint16_t verticalResolution;
	uint8_t reserved;
	uint8_t planes;
	uint16_t bytesPerLine;
	uint16_t paletteType;
	uint16_t horizontalScreenSize;
	uint16_t verticalScreenSize;
	uint8_t *reserved2;
	
	paletteEntry *egaPalette;
	paletteEntry *vgaPalette;
}pcxHeader;

int8_t _Cdecl readImagePcxHeader(FILE *stream, pcxHeader *p) {
	static uint8_t type,version,compression,bitsPerPixel,reserved,planes;
	static uint8_t paletteType,horizontalScreenSize,verticalScreenSize;
	static uint16_t xStart,yStart,xEnd,yEnd,horizontalResolution,verticalResolution,bytesPerLine;
	static uint8_t i; /*iterator*/
	uint8_t *reserved2; /*54/58 bytes*/
	fread(&type,sizeof(uint8_t),1,stream);
	if(type != 10) { return -1; } /*pcx specification states that 10h is signature*/
	fread(&version,sizeof(uint8_t),1,stream);
	if(version > 5) { return -2; } /*pcx hasn't any version above 3.0 (0x05)*/
	fread(&compression,sizeof(uint8_t),1,stream);
	if(compression != 1) { return -3; } /*pcx is rle-only file*/
	fread(&bitsPerPixel,sizeof(uint8_t),1,stream);
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
			break;
		default:
			return -4; /*not valid!*/
	}
	fread(&xStart,sizeof(uint16_t),1,stream); /*useless stuff, but useful*/
	fread(&yStart,sizeof(uint16_t),1,stream); /*if the game has some sort of magic*/
	fread(&xEnd,sizeof(uint16_t),1,stream); /*stuff that makes pcx files to fly*/
	fread(&yEnd,sizeof(uint16_t),1,stream);
	fread(&horizontalResolution,sizeof(uint16_t),1,stream);
	fread(&verticalResolution,sizeof(uint16_t),1,stream);
	/*read the EGA palette for once*/
	for(i = 0; i < 16; i++) {
		p->egaPalette[i].red = fgetc(stream);
		p->egaPalette[i].green = fgetc(stream);
		p->egaPalette[i].blue = fgetc(stream);
	}
	fread(&reserved,sizeof(uint8_t),1,stream);
	if(reserved != 0) { return -6; } /*reserved is always 0*/
	fread(&planes,sizeof(uint8_t),1,stream);
	fread(&bytesPerLine,sizeof(uint16_t),1,stream);
	fread(&paletteType,sizeof(uint16_t),1,stream);
	fread(&horizontalScreenSize,sizeof(uint16_t),1,stream);
	fread(&verticalScreenSize,sizeof(uint16_t),1,stream);
	reserved2 = (uint8_t *)malloc(54);
	if(reserved2 == NULL) { return -7; }
	fread((uint8_t *)reserved2,sizeof(uint8_t),54,stream);
	/*save the stuff in the struct if all went ok*/
	p->type = type;
	p->version = version;
	p->compression = compression;
	p->bitsPerPixel = bitsPerPixel;
	p->bytesPerLine = bytesPerLine;
	p->xStart = xStart;
	p->yStart = yStart;
	p->xEnd = xEnd;
	p->yEnd = yEnd;
	p->horizontalResolution = horizontalResolution;
	p->verticalResolution = verticalResolution;
	p->reserved = reserved;
	p->reserved2 = reserved2;
	p->planes = planes;
	return 0;
}

uint8_t * _Cdecl readImagePcxData(FILE *stream, pcxHeader *p) {
	register uint8_t rLen,tmp,val;
	register uint32_t index,dataSize,total;
	static uint8_t *data;
	dataSize = (p->xEnd+1)*(p->yEnd+1);
	index = 0; total = 0;
	data = (uint8_t *)malloc(dataSize);
	if(data == NULL) { return 0; }
	while(index < dataSize) {
		tmp = fgetc(stream);
		if((tmp&0xC0) == 0xC0) { /*is it a 2 byte value*/
			rLen = tmp&0x3F; val = fgetc(stream);
		} else { /*1 byte value*/
			rLen = 1; val = tmp;
		}
		for(total += rLen; rLen&&index < dataSize; rLen--, index++) { /*decompress the data*/
			data[index] = val;
		}
	}
	return (uint8_t *)data;
}

/*TODO: Fix this function*/
paletteEntry * _Cdecl readImagePcxVgaPalette(FILE *stream) {
	register int16_t vgaPaletteChecker;
	register uint16_t i;
	paletteEntry * pal;
	vgaPaletteChecker = fgetc(stream); /*is last byte 0x0C or EOF?*/
	if(vgaPaletteChecker != 0x0C || vgaPaletteChecker == EOF) { return NULL; }
	pal = (paletteEntry *)malloc(256*sizeof(paletteEntry));
	if(pal == NULL) { return NULL; } /*allocation error*/
	/*read rgb components into the palette entry*/
	for(i = 0; i < 256; i++) {
		pal[i].red = fgetc(stream);
		pal[i].green = fgetc(stream);
		pal[i].blue = fgetc(stream);
	}
	return (paletteEntry *)pal;
}

