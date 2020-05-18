/*
 * Leaf-Engine fbmp
 * Copyright (C) 2020 Jesus A. Diaz <jesusantonio30122016@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include "fbmp.h"

signed int Leaf_imageBitmap(const char * filename, Leaf_Image * img)
{
	unsigned short sHold;
	DDBheader db_DDBheader;
	BmpFileHeader db_WinBmpFileHeader;
	WinBmpHeader db_WinBmpHeader;
	WinNTBmpMasks db_WinNTBmpMasks;
	Win95BmpHeaderExtension db_Win95BmpHeaderExtension;
	WinOldBmpFileHeader db_WinOldBmpFileHeader;
	WinNewBmpFileHeader db_WinNewBmpFileHeader;
	size_t i; size_t i2;
	signed long isl; signed long isl2;
	unsigned short paletteEntries;
	
	unsigned char hold;
	
	unsigned long compression = 0;
	unsigned long bitsPerPixel = 0;
	signed long wide = 0; signed long tall = 0;
	FILE * fp;
	
	fp = fopen(filename,"rb");
	if(!fp) {
		return -1;
	}
	
	/*Read the bitmap header*/

	/*Get signature*/
	if(fread(&sHold,sizeof(unsigned short),1,fp) != 1) {
		fclose(fp);
		return -2;
	}
	
	/*Read bitmap header*/
	/*DDB File*/
	if(sHold == 0) {
		if(fread(&db_DDBheader,sizeof(DDBheader),1,fp) != 1) {
			fclose(fp);
			return -3;
		}
	}
	/*Windows Bitmap File*/
	else if(sHold == 0x4D42) {
		/*File Info Header*/
		if(fread(&db_WinBmpFileHeader,sizeof(WinBmpFileHeader),1,fp) != 1) {
			return -4;
		}
		/*Windows 2.x uses short for sizes*/
		if(db_WinBmpFileHeader.headerSize == 12) {
			if(fread(&db_WinOldBmpFileHeader,sizeof(WinOldBmpFileHeader),1,fp) != 1) {
				return -5;
			}
			bitsPerPixel = db_WinOldBmpFileHeader.bitsPerPixel;
			wide = (signed long)db_WinOldBmpFileHeader.wide;
			tall = (signed long)db_WinOldBmpFileHeader.tall;
		}
		/*Windows 3.x+ uses signed long for sizes*/
		else if(db_WinBmpFileHeader.headerSize >= 40) {
			if(fread(&db_WinNewBmpFileHeader,sizeof(WinNewBmpFileHeader),1,fp) != 1) {
				return -6;
			}
			bitsPerPixel = db_WinNewBmpFileHeader.bitsPerPixel;
			wide = db_WinNewBmpFileHeader.wide;
			tall = db_WinNewBmpFileHeader.tall;
		}
		else {
			return -16;
		}
		
		/*Bitmap Info Header*/
		if(db_WinBmpFileHeader.headerSize >= 40) {
			/*Windows 3.x/NT Bitmap*/
			if(fread(&db_WinBmpHeader,sizeof(WinBmpHeader),1,fp) != 1) {
				return -8;
			}
			compression = db_WinBmpHeader.compression;
			
			/*Windows NT added masks in Compression 3*/
			if(db_WinBmpFileHeader.headerSize == 40) {
				if(compression == 3) {
					if(fread(&db_WinNTBmpMasks,sizeof(WinNTBmpMasks),1,fp) != 1) {
						return -9;
					}
				}
			}
			/*Windows 95/98 Bitmap Extended Windows 3.x Header*/
			else if(db_WinBmpFileHeader.headerSize == 108) {
				if(fread(&db_Win95BmpHeaderExtension,sizeof(Win95BmpHeaderExtension),1,fp) != 1) {
					return -10;
				}
			}
		}
	}
	else {
		fclose(fp);
		return -11;
	}
	
	/*Check that information is correct*/
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
			break;
		default:
			fclose(fp);
			return -17;
	}
	
	/*Read the palette*/
	/*Only use palette for 8-bit images*/
	if(bitsPerPixel <= 8) {
		if(db_WinBmpFileHeader.headerSize == 12) {
			paletteEntries = (db_WinBmpFileHeader.offset-sizeof(BmpFileHeader)-sizeof(WinOldBmpFileHeader))/4;
		} else {
			paletteEntries = (1<<(bitsPerPixel));
		}
		img->palette = malloc(sizeof(paletteEntry)*paletteEntries);
		if(img->palette == NULL) {
			return -12;
		}
		
		/*Read the palette*/
		for(i = 0; i < paletteEntries; i++) {
			img->palette[i].b = fgetc(fp); /*Bitmap has reverse RGB order for each entry*/
			img->palette[i].g = fgetc(fp);
			img->palette[i].r = fgetc(fp);
			
			/*Later versions require 4-byte aligned palette*/
			if(db_WinBmpFileHeader.headerSize >= 40) {
				fgetc(fp);
			}
		}
		img->nPal = paletteEntries;
	}
	
	/*Read the bitmap data*/
	img->data = malloc(wide*tall);
	if(img->data == NULL) {
		if(img->palette != NULL) {
			free(img->palette);
		}
		fclose(fp);
		return -13;
	}
	
	switch(compression) {
		case 0:
			switch(bitsPerPixel) {
				/*Reverse read wide, but not tall*/
				case 8: /*256 colors*/
					for(isl = 0; isl < tall; isl++) {
						for(isl2 = 0; isl2 < wide; isl2++) {
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[((tall-(isl+1))*wide)+isl2] = hold;
						}
						/*Skip padding (dword)*/
						if(wide&3) {
							fseek(fp,SEEK_CUR,(int)(wide&3));
						}
					}
					break;
				case 4: /*16 colors*/
					for(i = 1; (signed long)i < tall+1; i++) {
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[((tall-i)*wide)+i2] = ((hold>>4)&0x0F);
							i2++;
							img->data[((tall-i)*wide)+i2] = (hold&0x0F);
						}
						if((((wide<<2)+7)>>3)&3) {
							fseek(fp,SEEK_CUR,(int)(3-((((wide<<2)+7)>>3)&3)));
						}
					}
					break;
				case 2: /*4 colors*/
					for(i = 1; (signed long)i < tall+1; i++) {
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							if(!(i2&3)) {
								fread(&hold,sizeof(unsigned char),1,fp);
							}
							img->data[((tall-i)*wide)+i2] = (hold>>6)&3;
							hold = (hold<<2);
						}
						if((((wide<<1)+7)>>3)&3) {
							fseek(fp,SEEK_CUR,(int)(3-((((wide<<1)+7)>>3)&3)));
						}
					}
					break;
				case 1: /*2 colors*/
					for(i = 1; (signed long)i < tall+1; i++) {
						for(i2 = 0; (signed long)i2 < wide; i2++) {
							if(!(i2&7)) {
								fread(&hold,sizeof(unsigned char),1,fp);
							}
							img->data[((tall-i)*wide)+i2] = ((hold>>7)&1);
							hold = (hold<<1);
						}
						if(((wide+7)>>3)&3) {
							fseek(fp,SEEK_CUR,(int)(3-((((wide)+7)>>3)&3)));
						}
					}
					break;
				default:
					if(img->data != NULL) {
						free(img->data);
					}
					if(img->palette == NULL) {
						free(img->palette);
					}
					fclose(fp);
					return -14;
			}
			break;
		default:
			if(img->data != NULL) {
				free(img->data);
			}
			if(img->palette == NULL) {
				free(img->palette);
			}
			fclose(fp);
			return -15;
	}
	
	img->wide = wide;
	img->tall = tall;
	
	fclose(fp);
	return 0;
}
