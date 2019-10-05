/*18 y luego 30*/
void decodeBMP(FILE *stream, struct image *im) {
    uint8_t *skip;
    static uint16_t i;
    uint16_t wide;
    uint16_t tall;
    skip = (uint8_t *)malloc(64);
    if(skip == NULL) {
        return;
    }
    fread(skip,sizeof(uint8_t),2,stream);
    if(skip[0] != 'B' || skip[1] != 'M') {
        return;
    }
    fskip(stream,12);
    fread(&wide,sizeof(uint32_t),1,stream);
    fread(&tall,sizeof(uint32_t),1,stream);
    im->wide = wide;
    im->tall = tall;
    /*for(i = 0; i < 8; i++) {
        fread(skip,sizeof(uint8_t),1,stream);
        printf("HEXDUMP %i: 0x%X\n",i,fgetc(stream));
    }*/
    /*fread(skip,sizeof(uint8_t),31,stream);*/
    fskip(stream,31+1024);
    im->data = (uint8_t *)malloc(((im->wide)*(im->tall))+1); /*Allocate only needed memory*/
    if(im->data == NULL) {
        free(skip);
        return;
    }
    /*Now read data*/
    fread(im->data,sizeof(uint8_t),(im->tall*im->wide),stream);
    free(skip);
    return;
}
