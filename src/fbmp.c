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

signed int imageBitmap(const char * filename, Image * img)
{
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
	
	unsigned long compression = 0;
	unsigned long bitsPerPixel = 0;
	signed long wide = 0; signed long tall = 0;
	FILE * fp;
	
	fp = fopen(filename,"rb");
	if(!fp)
	{
		return -1;
	}
	
	/*Read the bitmap header*/

	/*Get signature*/
	if(fread(&sHold,sizeof(unsigned short),1,fp) != 1)
	{
		return -2;
	}
	
	/*Read bitmap header*/
	/*DDB File*/
	if(sHold == 0)
	{
		if(fread(&db_DDBheader,sizeof(DDBheader),1,fp) < 1)
		{
			return -3;
		}
	}
	/*Windows Bitmap File*/
	else if(sHold == 0x4D42)
	{
		/*File Info Header*/
		
		if(fread(&db_WinBmpFileHeader,sizeof(WinBmpFileHeader),1,fp) < 1)
		{
			return -4;
		}
		
		/*Windows 2.x uses short for sizes*/
		if(db_WinBmpFileHeader.headerSize == 12) {
			if(fread(&db_Win2xBmpFileHeader,sizeof(Win2xBmpFileHeader),1,fp) < 1)
			{
				return -5;
			}
			bitsPerPixel = db_Win2xBmpFileHeader.bitsPerPixel;
			wide = (signed long)db_Win2xBmpFileHeader.wide;
			tall = (signed long)db_Win2xBmpFileHeader.tall;
		/*Windows 3.x+ uses long for sizes*/
		}
		else if(db_WinBmpFileHeader.headerSize >= 40)
		{
			if(fread(&db_Win3xBmpFileHeader,sizeof(Win3xBmpFileHeader),1,fp) < 1)
			{
				return -6;
			}
			bitsPerPixel = db_Win3xBmpFileHeader.bitsPerPixel;
			wide = db_Win3xBmpFileHeader.wide;
			tall = db_Win3xBmpFileHeader.tall;
		}
		
		/*Bitmap Info Header*/
		
		/*Windows 3.x Bitmap*/
		if(db_WinBmpFileHeader.headerSize == 40)
		{
			if(fread(&db_Win3xBmpHeader,sizeof(Win3xBmpHeader),1,fp) < 1)
			{
				return -7;
			}
			compression = db_Win3xBmpHeader.compression;
		}
		/*Windows NT Bitmap*/
		else if(db_WinBmpFileHeader.headerSize == 40)
		{
			if(fread(&db_WinNTBmpHeader,sizeof(WinNTBmpHeader),1,fp) < 1)
			{
				return -8;
			}
			if(db_WinNTBmpHeader.compression == 3)
			{
				if(fread(&db_WinNTBmpMasks,sizeof(WinNTBmpMasks),1,fp) < 1)
				{
					return -9;
				}
			}
			compression = db_WinNTBmpHeader.compression;
		}
		/*Windows 95/98 Bitmap*/
		else if(db_WinBmpFileHeader.headerSize == 108)
		{
			if(fread(&db_Win95BmpHeader,sizeof(Win95BmpHeader),1,fp) < 1)
			{
				return -10;
			}
			compression = db_Win95BmpHeader.compression;
		}
	}
	else
	{
		return -11;
	}
	
	/*Read the palette*/
	
	/*Only use palette for 8-bit images*/
	if(bitsPerPixel < 8)
	{
		paletteEntries = (1<<(bitsPerPixel));
		img->palette = (paletteEntry *)malloc(sizeof(paletteEntry)*paletteEntries);
		if(img->palette == NULL)
		{
			return -12;
		}
		
		/*Read the palette*/
		for(i = 0; i < paletteEntries; i++)
		{
			img->palette[i].b = fgetc(fp); /*Bitmap has reverse RGB order for each entry*/
			img->palette[i].g = fgetc(fp);
			img->palette[i].r = fgetc(fp);
			
			/*Later versions require 4-byte aligned palette*/
			if(db_WinBmpFileHeader.headerSize >= 40)
			{
				fgetc(fp);
			}
		}
	}
	
	/*Read the bitmap data*/
	img->data = (unsigned char *)malloc(wide*tall);
	if(img->data == NULL)
	{
		if(img->palette != NULL)
		{
			free(img->palette);
		}
		return -13;
	}
	
	switch(compression)
	{
		case 0:
			switch(bitsPerPixel)
			{
				case 8: /*256 colors*/
					/*Reverse read wide, but not tall*/
					for(i = 1; (signed long)i < tall+1; i++)
					{
						for(i2 = 0; (signed long)i2 < wide; i2++)
						{
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[(i2+((tall-i)*wide))] = (hold);
						}
						/*Skip padding (dword)*/
						if((wide&3) != 0)
						{
							fseek(fp,SEEK_CUR,(int)(wide&3));
						}
					}
					break;
				case 4: /*16 colors*/
					/*Reverse read wide, but not tall*/
					for(i = 1; (signed long)i < tall+1; i++)
					{
						for(i2 = 0; (signed long)i2 < wide; i2++)
						{
							fread(&hold,sizeof(unsigned char),1,fp);
							img->data[(i2+((tall-i)*wide))] = ((hold>>4)&0x0F);
							i2++;
							img->data[(i2+((tall-i)*wide))] = (hold&0x0F);
						}
						if(((((wide*4)+7)>>3)&3))
						{
							fseek(fp,SEEK_CUR,(int)(3-((((wide*4)+7)>>3)&3)));
						}
					}
					break;
				case 2: /*4 colors*/
					/*Reverse read wide, but not tall*/
					for(i = 1; (signed long)i < tall+1; i++)
					{
						for(i2 = 0; (signed long)i2 < wide; i2++)
						{
							if(!(i2&3))
							{
								fread(&hold,sizeof(unsigned char),1,fp);
							}
							img->data[(i2+((tall-i)*wide))] = (hold>>6)&3;
							hold = (hold<<2);
						}
						if(((((wide*2)+7)>>3)&3))
						{
							fseek(fp,SEEK_CUR,(int)(3-((((wide*2)+7)>>3)&3)));
						}
					}
					break;
				case 1: /*2 colors*/
					/*Reverse read wide, but not tall*/
					for(i = 1; (signed long)i < tall+1; i++)
					{
						for(i2 = 0; (signed long)i2 < wide; i2++)
						{
							if(!(i2&7))
							{
								fread(&hold,sizeof(unsigned char),1,fp);
							}
							img->data[(i2+((tall-i)*wide))] = ((hold>>7)&1);
							hold = (hold<<1);
						}
						if(((((wide*1)+7)>>3)&3))
						{
							fseek(fp,SEEK_CUR,(int)(3-((((wide*1)+7)>>3)&3)));
						}
					}
					break;
				default:
					if(img->data != NULL)
					{
						free(img->data);
					}
					if(img->palette == NULL)
					{
						free(img->palette);
					}
					return -14;
			}
			break;
		default:
			if(img->data != NULL)
			{
				free(img->data);
			}
			if(img->palette == NULL)
			{
				free(img->palette);
			}
			return -15;
	}
	
	img->wide = wide;
	img->tall = tall;
	
	fclose(fp);
	return 0;
}
