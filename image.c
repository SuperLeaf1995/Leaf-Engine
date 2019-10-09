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
    static int32_t wide;
    static int32_t tall;
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
@Parameters: stream=file stream. data=data pointer (must not be allocated!). wide=wide of image. tall=tall of image.
x=x position of the image. y=y position of the image
@Output: last file position (fpos_t) so we can rewind back to the image
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
fpos_t _Cdecl readBitmapData(FILE *stream, uint8_t *data, int32_t wide, int32_t tall, uint16_t x, uint16_t y) {
    static uint16_t i;
    static uint16_t i2;
    fpos_t pos;
    if(tall == 0 || wide == 0) {
        return 0;
    }
    data = (uint8_t *)malloc(wide*tall);
    if(data == NULL) {
        return 0; /*Up to caller's, how to handle errors*/
    }
    fgetpos(stream,&pos);
    fread(data,sizeof(uint8_t),wide*tall,stream);
    for(i = 0; i < tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide+1; i2++) {
			plotPixel(i2+x,i+y,data[i2+(((tall-i)-1)*wide-1)]);
		}
    }
    return pos; /*Return file position of the last RW, so we can rewind to it a later time!*/
}

/*
@Action: Writes bitmap file header (before information)
@Parameters: stream=file stream. structure bitmapFileHeader=structure pointer, we will read data
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl writeBitmapFileHeader(FILE *stream, struct bitmapFileHeader *b) {
    static uint32_t sizeOfFile,reserved,offset;
    sizeOfFile = b->sizeOfFile;
    reserved = b->reserved;
    offset = b->offset;
    fwrite(b->type,sizeof(uint16_t),1,stream);
    fwrite(&sizeOfFile,sizeof(uint32_t),1,stream);
    fwrite(&reserved,sizeof(uint32_t),1,stream);
    fwrite(&offset,sizeof(uint32_t),1,stream);
    return;
}

/*
@Action: Writes bitmap information
@Parameters: stream=file stream. structure bitmapInfoHeader=structure pointer, we will read data
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl writeBitmapInformationHeader(FILE *stream, struct bitmapInfoHeader *b) {
    static uint32_t headerSize;
    static int32_t wide;
    static int32_t tall;
    static uint16_t planes;
    static uint16_t bitsPerPixel;
    static uint32_t compression;
    static uint32_t sizeOfImage;
    static uint32_t xPixelsPerMeter;
    static uint32_t yPixelsPerMeter;
    static uint32_t numberOfColors;
    static uint32_t importantColors;
    headerSize = b->headerSize;
    wide = b->wide;
    tall = b->tall;
    planes = b->planes;
    bitsPerPixel = b->bitsPerPixel;
    compression = b->compression;
    sizeOfImage = b->sizeOfImage;
    xPixelsPerMeter = b->xPixelsPerMeter;
    yPixelsPerMeter = b->yPixelsPerMeter;
    numberOfColors = b->numberOfColors;
    importantColors = b->importantColors;
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
    return;
}

/*
@Action: Displays intro of leaf engine, optional. Use it if you want :)
@Parameters: time=time in seconds to show the intro, fileOfLeafEngine=file address
@Output: void
@Compatible platforms: MSDOS (*T), FreeDOS (*UT), AppleII+ (*UT)
*/
void _Cdecl displayIntro(uint64_t time, char *fileOfLeafEngine) {
    /*Create a big memory*/
    static FILE *fp;
    static struct bitmapFileHeader bfh;
    static struct bitmapInfoHeader bih;
    static char *data;
    static fpos_t pos;
    static uint64_t i;
    fp = fopen(fileOfLeafEngine,"rb");
    if(!fp) {
        return;
    }
    readBitmapFileHeader(fp,&bfh);
    readBitmapInformationHeader(fp,&bih);
    for(i = 0; i < 1024; i++) { /*We cant read palettes yet, just, drop it out!*/
		fgetc(fp);
	}
    for(i = 0; i < time; i++) {
        pos = readBitmapData(fp,data,bih.wide,bih.tall,0,0);
        fsetpos(fp,&pos);
    }
    free(data);
    fclose(fp);
    return;
}
