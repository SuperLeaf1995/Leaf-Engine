/*
@Action: Reads bitmap file header (before information)
@Param: stream=file stream. struct bitmapFileHeader=struct pointer, we will write data there!
@Output: void, but overwrites struct bitmapFileHeader
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl readBitmapFileHeader(FILE *stream, struct bitmapFileHeader *b) {
    uint32_t sizeOfFile,reserved,offset;
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
@Param: stream=file stream. struct bitmapInfoHeader=struct pointer, we will write data there!
@Output: void, but overwrites bitmapInfoHeader
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl readBitmapInformationHeader(FILE *stream, struct bitmapInfoHeader *b) {
    uint32_t headerSize;
    int32_t wide;
    int32_t tall;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t sizeOfImage;
    uint32_t xPixelsPerMeter;
    uint32_t yPixelsPerMeter;
    uint32_t numberOfColors;
    uint32_t importantColors;
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
@Param: stream=file stream. wide=wide of image. tall=tall of image. data=data pointer (must not be allocated!)
@Output: Total size of image in an int32_t, higher half is tall, lower half is wide
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
uint32_t _Cdecl readBitmapData(FILE *stream, uint32_t wide, uint32_t tall, uint8_t *data) {
    uint16_t i;
    uint16_t i2;
    if(tall == 0 || wide == 0) {
        return 0;
    }
    data = (uint8_t *)malloc(wide*tall);
    if(data == NULL) {
        return 0; /*Up to caller's, how to handle errors*/
    }
    fread(data,sizeof(uint8_t),wide*tall,stream);
    for(i = 0; i < tall; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) {
			plotPixel(i2,i,data[i2+(((tall-i)-1)*wide)]);
		}
    }
    return (tall<<16)+wide; /*Store TALL in higher half of value, and wide in the lower one, easy!*/
}
