/*TIFF.C
 *File to read TIFF files*/

typedef struct tiffHeader {
	uint16_t id;
	uint16_t version; /*version and stuff*/
	uint32_t ifdOff; /*first image offset*/
}tiffHeader;

typedef struct tiffTag {
	uint16_t tagId;
	uint16_t dataType;
	uint32_t dataCount;
	uint32_t dataOff;
}tiffTag;

typedef struct tiffIfd {
	uint16_t directoryEntries; /*number of directory entries*/
	tiffTag *tags; /*12-byte wide tags array*/
	uint32_t nextIfdOff; /*next ifd*/
}tiffIfd;

uint8_t readImageTiffHeader(FILE *stream, tiffHeader *t) {
	static uint16_t id,version;
	static uint32_t ifdOff;
	fread(&id,sizeof(uint16_t),1,stream);
	if(id != 0x4949 /*intel*/
	&& id != 0x4D4D /*motorola*/) {
		return -1; /*invalid tiff*/
	}
	fread(&version,sizeof(uint16_t),1,stream);
	fread(&ifdOff,sizeof(uint32_t),1,stream);
	t->version = version;
	t->id = id;
	t->ifdOff = ifdOff;
	return 0;
}
