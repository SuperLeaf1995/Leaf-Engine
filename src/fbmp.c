#include "fbmp.h"

signed int imageBitmap(const char * filename, Image * img) {
	unsigned short sHold;
	DDBheader db_DDBheader;
	WinBmpFileHeader db_WinBmpFileHeader;
	Win3xBmpHeader db_Win3xBmpHeader;
	WinNTBmpHeader db_WinNTBmpHeader;
	WinNTBmpMasks db_WinNTBmpMasks;
	Win95BmpHeader db_Win95BmpHeader;
	Win2xBmpFileHeader db_Win2xBmpFileHeader;
	Win3xBmpFileHeader db_Win3xBmpFileHeader;
	size_t i; size_t i2;
	unsigned short paletteEntries;
	
	unsigned char hold;
	
	unsigned long compression;
	unsigned long bitsPerPixel;
	signed long wide; signed long tall;
	
	FILE * fp;
	
	fp = fopen(filename,"rb");
	if(!fp) {
		return -1;
	}
	
	/*Read the bitmap header*/

	/*Get signature*/
	if(fread(&sHold,sizeof(unsigned short),1,fp) != 1) {
		return -2;
	}
	
	/*Read bitmap header*/
	/*DDB File*/
	if(sHold == 0) {
		if(fread(&db_DDBheader,sizeof(DDBheader),1,fp) < 1) {
			return -3;
		}
		printf("WINDOWS 1.X\n");
	}
	/*Windows Bitmap File*/
	else if(sHold == 0x4D42) {
		/*File Info Header*/
		
		if(fread(&db_WinBmpFileHeader,sizeof(WinBmpFileHeader),1,fp) < 1) {
			return -4;
		}
		
		/*Windows 2.x uses short for sizes*/
		if(db_WinBmpFileHeader.headerSize == 12) {
			if(fread(&db_Win2xBmpFileHeader,sizeof(Win2xBmpFileHeader),1,fp) < 1) {
				return -4;
			}
			bitsPerPixel = db_Win2xBmpFileHeader.bitsPerPixel;
			wide = db_Win2xBmpFileHeader.wide;
			tall = db_Win2xBmpFileHeader.tall;
		/*Windows 3.x+ uses long for sizes*/
		} else if(db_WinBmpFileHeader.headerSize > 40) {
			if(fread(&db_Win3xBmpFileHeader,sizeof(Win3xBmpFileHeader),1,fp) < 1) {
				return -4;
			}
			bitsPerPixel = db_Win3xBmpFileHeader.bitsPerPixel;
			wide = db_Win3xBmpFileHeader.wide;
			tall = db_Win3xBmpFileHeader.tall;
		}
		
		/*Bitmap Info Header*/
		
		/*Windows 3.x Bitmap*/
		if(db_WinBmpFileHeader.headerSize == 40) {
			if(fread(&db_Win3xBmpHeader,sizeof(Win3xBmpHeader),1,fp) < 1) {
				return -5;
			}
			compression = db_Win3xBmpHeader.compression;
			printf("WINDOWS 3.X\n");
		}
		/*Windows NT Bitmap*/
		else if(db_WinBmpFileHeader.headerSize == 40) {
			if(fread(&db_WinNTBmpHeader,sizeof(WinNTBmpHeader),1,fp) < 1) {
				return -5;
			}
			if(db_WinNTBmpHeader.compression == 3) {
				if(fread(&db_WinNTBmpMasks,sizeof(WinNTBmpMasks),1,fp) < 1) {
					return -5;
				}
			}
			compression = db_WinNTBmpHeader.compression;
			printf("WINDOWS NT\n");
		}
		/*Windows 95/98 Bitmap*/
		else if(db_WinBmpFileHeader.headerSize == 108) {
			if(fread(&db_Win95BmpHeader,sizeof(Win95BmpHeader),1,fp) < 1) {
				return -5;
			}
			compression = db_Win95BmpHeader.compression;
			printf("fsize: %lu\n",db_WinBmpFileHeader.fileSize);
			printf("hsize: %lu\n",db_WinBmpFileHeader.headerSize);
			printf("offset: %lu\n",db_WinBmpFileHeader.offset);
			printf("resolution: %lux%lu\n",db_Win3xBmpFileHeader.wide,db_Win3xBmpFileHeader.tall);
			printf("bpp: %u\n",db_Win3xBmpFileHeader.bitsPerPixel);
			printf("WINDOWS 95/98\n");
		}
	} else {
		printf("UNKNOWN\n");
		return -6;
	}
	
	/*Only use palette for 8-bit images*/
	if(bitsPerPixel < 8) {
		paletteEntries = (1<<(bitsPerPixel));
		img->palette = (paletteEntry *)malloc(sizeof(paletteEntry)*paletteEntries);
		if(img->palette == NULL) {
			return -7;
		}
		for(i = 0; i < paletteEntries; i++) { /*Read the palette*/
			img->palette[i].b = fgetc(fp); /*Bitmap has reverse RGB order for each entry*/
			img->palette[i].g = fgetc(fp);
			img->palette[i].r = fgetc(fp);
			if(db_WinBmpFileHeader.headerSize >= 40) { /*Later versions require 4-byte aligned palette*/
				fgetc(fp);
			}
		}
	}
	
	img->data = (unsigned char *)malloc(wide*tall);
	if(img->data == NULL) {
		return -8;
	}
	
	switch(compression) {
		case 0:
			switch(bitsPerPixel) {
				case 8: /*256 colors*/
					for(i = 1; (signed long)i < tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[(i2+((tall-i)*wide))] = hold;
						}
						if(((((wide*8)+7)>>3)&3)) { /*skip padding (dword)*/
							fseek(fp,SEEK_CUR,3-((((wide*8)+7)>>3)&3));
						}
					}
					break;
				case 4: /*16 colors*/
					for(i = 1; (signed long)i < tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[(i2+((tall-i)*wide))] = (hold>>4)&0x0F;
							i2++;
							img->data[(i2+((tall-i)*wide))] = hold&0x0F;
						}
						if(((((wide*4)+7)>>3)&3)) {
							fseek(fp,SEEK_CUR,3-((((wide*4)+7)>>3)&3));
						}
					}
					break;
				case 2: /*4 colors*/
					for(i = 1; (signed long)i < tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							if(!(i2&3)) {
								fread(&hold,sizeof(unsigned char),1,fp);
							}
							img->data[(i2+((tall-i)*wide))] = (hold>>6)&3;
							hold = hold<<2;
						}
						if(((((wide*2)+7)>>3)&3)) {
							fseek(fp,SEEK_CUR,3-((((wide*2)+7)>>3)&3));
						}
					}
					break;
				case 1: /*2 colors*/
					for(i = 1; (signed long)i < tall+1; i++) { /*reverse read wide, but not tall*/
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							if(!(i2&7)) {
								fread(&hold,sizeof(unsigned char),1,fp);
							}
							img->data[(i2+((tall-i)*wide))] = ((hold>>7)&1);
							hold <<= 1;
						}
						if(((((wide*1)+7)>>3)&3)) {
							fseek(fp,SEEK_CUR,3-((((wide*1)+7)>>3)&3));
						}
					}
					break;
				default:
					if(img->data != NULL) {
						free(img->data);
					}
					return -9;
			}
			break;
		default:
			if(img->data != NULL) {
				free(img->data);
			}
			return -10;
	}
	
	fclose(fp);
	return 0;
}
