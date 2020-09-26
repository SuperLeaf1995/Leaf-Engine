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

signed int Leaf_ImageBitmap(const char * filename, Leaf_Image * img)
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
	
	signed int retval = 0;
	
	unsigned char hold;
	
	unsigned long compression = 0;
	unsigned long bitsPerPixel = 0;
	signed long wide = 0; signed long tall = 0;
	FILE * fp;
	
	fp = fopen(filename,"rb");
	if(!fp) {
		retval = -1; goto end;
	}
	
	/*Read the bitmap header*/

	/*Get signature*/
	if(fread(&sHold,sizeof(unsigned short),1,fp) != 1) {
		retval = -2; goto end;
	}
	
	/*Read bitmap header*/
	/*DDB File*/
	if(sHold == 0) {
		if(fread(&db_DDBheader,sizeof(DDBheader),1,fp) != 1) {
			retval = -3; goto end;
		}
	}
	/*Windows Bitmap File*/
	else if(sHold == 0x4D42) {
		/*File Info Header*/
		if(fread(&db_WinBmpFileHeader,sizeof(BmpFileHeader),1,fp) != 1) {
			retval = -4; goto end;
		}
		/*Windows 2.x uses short for sizes*/
		if(db_WinBmpFileHeader.headerSize == 12) {
			if(fread(&db_WinOldBmpFileHeader,sizeof(WinOldBmpFileHeader),1,fp) != 1) {
				retval = -5; goto end;
			}
			bitsPerPixel = db_WinOldBmpFileHeader.bitsPerPixel;
			wide = (signed long)db_WinOldBmpFileHeader.wide;
			tall = (signed long)db_WinOldBmpFileHeader.tall;
		}
		/*Windows 3.x+ uses signed long for sizes*/
		else if(db_WinBmpFileHeader.headerSize >= 40) {
			if(fread(&db_WinNewBmpFileHeader,sizeof(WinNewBmpFileHeader),1,fp) != 1) {
				retval = -6; goto end;
			}
			bitsPerPixel = db_WinNewBmpFileHeader.bitsPerPixel;
			wide = db_WinNewBmpFileHeader.wide;
			tall = db_WinNewBmpFileHeader.tall;
		}
		else {
			retval = -7; goto end;
		}
		
		/*Bitmap Info Header*/
		if(db_WinBmpFileHeader.headerSize >= 40) {
			/*Windows 3.x/NT Bitmap*/
			if(fread(&db_WinBmpHeader,sizeof(WinBmpHeader),1,fp) != 1) {
				retval = -8; goto end;
			}
			compression = db_WinBmpHeader.compression;
			
			/*Windows NT added masks in Compression 3*/
			if(db_WinBmpFileHeader.headerSize == 40) {
				if(compression == 3) {
					if(fread(&db_WinNTBmpMasks,sizeof(WinNTBmpMasks),1,fp) != 1) {
						retval = -9; goto end;
					}
				}
			}
			/*Windows 95/98 Bitmap Extended Windows 3.x Header*/
			else if(db_WinBmpFileHeader.headerSize == 108) {
				if(fread(&db_Win95BmpHeaderExtension,sizeof(Win95BmpHeaderExtension),1,fp) != 1) {
					retval = -10; goto end;
				}
			}
		}
	}
	else {
		retval = -11; goto end;
	}
	
	/*Check that information is correct*/
	switch(bitsPerPixel) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 24:
		case 32:
			break;
		default:
			retval = -12; goto end;
	}
	
	/*Read the palette*/
	/*Only use palette for 8-bit images*/
	if(bitsPerPixel <= 8) {
		if(db_WinBmpFileHeader.headerSize == 12) {
			paletteEntries = (db_WinBmpFileHeader.offset-sizeof(BmpFileHeader)-sizeof(WinOldBmpFileHeader))>>2;
		} else {
			/*Magic number*/
			/*(1<<(bps-2)) = Magik (wtf???)*/
			paletteEntries = (1<<(bitsPerPixel-2));
		}
		img->palette = malloc(sizeof(Leaf_PaletteEntry)*paletteEntries);
		if(img->palette == NULL) {
			retval = -13; goto end;
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
		retval = -14; goto end;
	}
	
	switch(compression) {
		case 0:
			switch(bitsPerPixel) {
				case 8: /*256 colors*/
					for(isl = 0; isl < tall; isl++) {
						for(isl2 = 0; isl2 < wide; isl2++) {
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[((tall-(isl+1))*wide)+isl2] = hold;
						}
						/*Skip padding (dword)*/
						if(wide&3) {
							fseek(fp,SEEK_CUR,wide&3);
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
							fseek(fp,SEEK_CUR,3-((((wide<<2)+7)>>3)&3));
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
							fseek(fp,SEEK_CUR,3-((((wide<<1)+7)>>3)&3));
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
							fseek(fp,SEEK_CUR,3-((((wide)+7)>>3)&3));
						}
					}
					break;
				default:
					retval = -15; goto end;
			}
			break;
		default:
			retval = -16; goto end;
	}
	
	goto end_noerr;
end:
	if(img->data != NULL) {
		free(img->data);
	}
	if(img->palette == NULL) {
		free(img->palette);
	}
end_noerr:
	if(fp) {
		fclose(fp);
	}

	img->wide = wide;
	img->tall = tall;
	return (retval);
}
