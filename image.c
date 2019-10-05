/*18 y luego 30*/
void decodeBMP(FILE *stream, struct image *im, uint8_t *data) {
    uint8_t *skip;
    skip = (uint8_t *)malloc(64);
    if(skip == NULL) {
        return;
    }
    fread(skip,sizeof(uint8_t),18,stream);
    fread((struct image *)im->wide,sizeof(uint32_t),1,stream);
    fread((struct image *)im->tall,sizeof(uint32_t),1,stream);
    fread(skip,sizeof(uint8_t),30,stream);
    im->data = (uint8_t *)malloc(((im->wide)*(im->tall))+1); /*Allocate only needed memory*/
    if(im->data == NULL) {
        free(skip);
    }
    /*Now read data*/
    fread(im->data,sizeof(uint8_t),(im->tall*im->wide),stream);
    free(skip);
    return;
}
