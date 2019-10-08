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

int8_t _Cdecl readBitmapData(FILE *stream, uint32_t wide, uint32_t tall, uint8_t *data) {
    uint16_t i;
    data = (uint8_t *)malloc(wide*tall);
    if(data == NULL) {
        return -1;
    }
    for(i = 0; i < wide*tall; i++) {
        fread(data,sizeof(uint8_t),1,stream);
    }
    return 0;
}
