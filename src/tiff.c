/*TIFF.C
 *File to read TIFF files*/

#include "tiff.h"

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
