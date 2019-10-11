/*
@Action: Reads bitmap file header (before information)
@Parameters: stream=file stream. structure bitmapFileHeader=structure pointer, we will write data there!
@Output: void, but overwrites structure bitmapFileHeader
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl readBitmapFileHeader(FILE *stream, struct bitmapFileHeader *b) {
    static uint32_t sizeOfFile,reserved,offset;
    fread(b->type,sizeof(uint16_t),1,stream);
    fread(&sizeOfFile,sizeof(uint32_t),1,stream);
    fread(&reserved,sizeof(uint32_t),1,stream);
    fread(&offset,sizeof(uint32_t),1,stream);
    b->sizeOfFile = sizeOfFile;
    b->reserved = reserved;
    b->offset = offset;
    return;
}

/*
@Action: Reads bitmap information
@Parameters: stream=file stream. structure bitmapInfoHeader=structure pointer, we will write data there!
@Output: void, but overwrites bitmapInfoHeader
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl readBitmapInformationHeader(FILE *stream, struct bitmapInfoHeader *b) {
    static uint32_t headerSize;
    static uint32_t wide;
    static uint32_t tall;
    static uint16_t planes;
    static uint16_t bitsPerPixel;
    static uint32_t compression;
    static uint32_t sizeOfImage;
    static uint32_t xPixelsPerMeter;
    static uint32_t yPixelsPerMeter;
    static uint32_t numberOfColors;
    static uint32_t importantColors;
    fread(&headerSize,sizeof(uint32_t),1,stream);
    fread(&wide,sizeof(int32_t),1,stream);
    fread(&tall,sizeof(int32_t),1,stream);
    fread(&planes,sizeof(uint16_t),1,stream);
    fread(&bitsPerPixel,sizeof(uint32_t),1,stream);
    fread(&compression,sizeof(uint32_t),1,stream);
    fread(&sizeOfImage,sizeof(uint32_t),1,stream);
    fread(&xPixelsPerMeter,sizeof(uint32_t),1,stream);
    fread(&yPixelsPerMeter,sizeof(uint32_t),1,stream);
    fread(&numberOfColors,sizeof(uint32_t),1,stream);
    fread(&importantColors,sizeof(uint32_t),1,stream);
    b->headerSize = headerSize;
    b->wide = wide;
    b->tall = tall;
    b->planes = planes;
    b->bitsPerPixel = bitsPerPixel;
    b->compression = compression;
    b->sizeOfImage = sizeOfImage;
    b->xPixelsPerMeter = xPixelsPerMeter;
    b->yPixelsPerMeter = yPixelsPerMeter;
    b->numberOfColors = numberOfColors;
    b->importantColors = importantColors;
    return;
}

/*
@Action: Reads the image of a bitmap and then outputs it
@Parameters: stream=file stream. wide=wide of image. tall=tall of image.
@Output: data pointer
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
uint8_t * _Cdecl readBitmapData(FILE *stream, uint32_t wide, uint32_t tall) {
    static uint32_t i;
    static uint32_t i2;
    static uint16_t hold;
    uint8_t *data;
    fpos_t pos;
    if(tall == 0 || wide == 0) {
        return 0;
    }
    data = (uint8_t *)malloc(wide*tall);
    if(data == NULL) {
        return 0; /*Up to caller's, how to handle errors*/
    }
    fgetpos(stream,&pos);
    for(i = 1; i < tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) {
			fread(&hold,sizeof(uint8_t),1,stream);
			data[(i2+((tall-i)*wide))] = hold;
		}
    }
    return (uint8_t *)data;
}

/*
@Action: Displays image
@Parameters: data=data pointer. x=x pos. y=y pos. wide=wide of image. tall=tall of image
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl displayImage(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall) {
    static uint32_t i;
    static uint32_t i2;
    for(i2 = 0; i2 < tall; i2++) {
        for(i = 0; i < wide; i++) {
            plotPixel(x+i,y+i2,data[i+(i2*wide)]);
        }
    }
    return;
}

/*
@Action: Displays image in tiled mode
@Parameters: data=data pointer. x=x pos. y=y pos. wide=wide of tile. tall=tall of tile. index=tile number
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl displayImageTile(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall, uint32_t index) {
    static uint32_t i;
    static uint32_t i2;
    for(i2 = 0; i2 < tall; i2++) {
        for(i = 0; i < wide; i++) {
            plotPixel(x+i,y+i2,data[(index*(wide*tall))+i+(i2*wide)]);
        }
    }
    return;
}

/*
@Action: Saves bitmap
@Parameters: stream=file stream. bfh=bitmap file header pointer. bih=bitmap info header pointer.
data=data pointer
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl writeBitmap(FILE *stream, struct bitmapFileHeader *bfh, struct bitmapInfoHeader *bih, uint8_t *data) {
	static uint32_t sizeOfFile,reserved,offset; /*We do the same as in our read routines*/
    static uint32_t headerSize;					/*we just switch the order of stuff and*/
    static uint32_t wide;						/*change read with write.*/
    static uint32_t tall;
    static uint16_t planes;
    static uint16_t bitsPerPixel;
    static uint32_t compression;
    static uint32_t sizeOfImage;
    static uint32_t xPixelsPerMeter;
    static uint32_t yPixelsPerMeter;
    static uint32_t numberOfColors;
    static uint32_t importantColors;
    static uint32_t i;
    static uint32_t i2;
    static uint16_t hold;
	sizeOfFile = bfh->sizeOfFile;
    reserved = bfh->reserved;
    offset = bfh->offset;
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
    fwrite(bfh->type,sizeof(uint16_t),1,stream);
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
    /*Palette*/
    for(i = 0; i < 1022; i++) {
		fwrite(&i,sizeof(uint8_t),1,stream);
	}
	/*Bitmap data*/
	for(i = 1; i < tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) { /*We just replotting our shit back again!*/
			hold = data[(i2+((tall-i)*wide))];
			fwrite(&hold,sizeof(uint8_t),1,stream);
		}
    }
	return;
}
