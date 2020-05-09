#include "fpcx.h"

/**
@brief Reads header of the PCX

@param stream File stream of the PCX file
@param p PCXHeader used to store metadata
*/
signed int  readImagePcxHeader(FILE * stream, pcxHeader * p)
{
	unsigned char type,version,compression,bitsPerPixel,reserved,planes;
	unsigned char paletteType,horizontalScreenSize,verticalScreenSize;
	unsigned short xStart,yStart,xEnd,yEnd,horizontalResolution,verticalResolution,bytesPerLine;
	unsigned char i; /*iterator*/
	unsigned char *reserved2; /*54/58 bytes*/
	fread(&type,sizeof(unsigned char),1,stream);
	if(type != 10) { return -1; } /*pcx specification states that 10h is signature*/
	fread(&version,sizeof(unsigned char),1,stream);
	if(version > 5) { return -2; } /*pcx hasn't any version above 3.0 (0x05)*/
	fread(&compression,sizeof(unsigned char),1,stream);
	if(compression != 1) { return -3; } /*pcx is rle-only file*/
	fread(&bitsPerPixel,sizeof(unsigned char),1,stream);
	switch(bitsPerPixel)
	{
		case 1:
		case 2:
		case 4:
		case 8:
			break;
		default:
			return -4; /*not valid!*/
	}
	fread(&xStart,sizeof(unsigned short),1,stream); /*useless stuff, but useful*/
	fread(&yStart,sizeof(unsigned short),1,stream); /*if the game has some sort of magic*/
	fread(&xEnd,sizeof(unsigned short),1,stream); /*stuff that makes pcx files to fly*/
	fread(&yEnd,sizeof(unsigned short),1,stream);
	fread(&horizontalResolution,sizeof(unsigned short),1,stream);
	fread(&verticalResolution,sizeof(unsigned short),1,stream);
	/*read the EGA palette for once*/
	for(i = 0; i < 16; i++)
	{
		p->egaPalette[i].r = fgetc(stream);
		p->egaPalette[i].g = fgetc(stream);
		p->egaPalette[i].b = fgetc(stream);
	}
	fread(&reserved,sizeof(unsigned char),1,stream);
	if(reserved != 0) { return -6; } /*reserved is always 0*/
	fread(&planes,sizeof(unsigned char),1,stream);
	fread(&bytesPerLine,sizeof(unsigned short),1,stream);
	fread(&paletteType,sizeof(unsigned short),1,stream);
	fread(&horizontalScreenSize,sizeof(unsigned short),1,stream);
	fread(&verticalScreenSize,sizeof(unsigned short),1,stream);
	reserved2 = (unsigned char *)malloc(54);
	if(reserved2 == NULL)
	{
		return -7;
	}
	fread((unsigned char *)reserved2,sizeof(unsigned char),54,stream);
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

/**
@brief Reads data of the PCX

@param stream File stream of the PCX file
@param p Used for getting correct data
*/
unsigned char *  readImagePcxData(FILE * stream, pcxHeader * p) {
	register unsigned char rLen,tmp,val;
	register unsigned long index,dataSize,total;
	static unsigned char * data;
	dataSize = (p->xEnd+1)*(p->yEnd+1);
	index = 0; total = 0;
	data = (unsigned char *)malloc(dataSize);
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
	return (unsigned char *)data;
}

/*TODO: Fix this function*/
/**
@brief Reads the VGA palette of the PCX

@param stream File stream of the PCX file
*/
paletteEntry *  readImagePcxVgaPalette(FILE * stream) {
	register signed short vgaPaletteChecker;
	register unsigned short i;
	paletteEntry * pal;
	vgaPaletteChecker = fgetc(stream); /*is last byte 0x0C or EOF?*/
	if(vgaPaletteChecker != 0x0C || vgaPaletteChecker == EOF) { return NULL; }
	pal = (paletteEntry *)malloc(256*sizeof(paletteEntry));
	if(pal == NULL) { return NULL; } /*allocation error*/
	/*read rgb components into the palette entry*/
	for(i = 0; i < 256; i++) {
		pal[i].r = fgetc(stream);
		pal[i].g = fgetc(stream);
		pal[i].b = fgetc(stream);
	}
	return (paletteEntry *)pal;
}
