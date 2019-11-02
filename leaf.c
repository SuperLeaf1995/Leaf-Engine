#ifdef _LEAF_ERROR

void _Cdecl setLeafError(uint8_t id) {
	globalLeafErrorHandler = id;
	return;
}

char * _Cdecl leafError(void) {
	char *str;
	str = (char *)malloc(16);
	if(str == NULL) {
		return NULL;
	}
	switch(globalLeafErrorHandler) {
		case 0:
			strcpy(str,"ERR_ALLOC\0");
			break;
		case 1:
			strcpy(str,"ERR_MOUSE_INIT\0");
			break;
		default:
			strcpy(str,"ERR_UNDEF\0");
			break;
	}
	return str;
}

#endif

/*
@Action: Sets video to *video*
@Parameters: video=16-bit long value of the desired video mode
@Output: Old video mode
*/
int16_t _Cdecl setVideo(register int16_t video) { /*Sets the video using int 10h*/
	static int16_t oldVideo;
	in.h.ah = 0x0F;
    int86(0x10,&in,&out);
    oldVideo = out.h.al;
    if(oldVideo == 0x29) { /*Realtek RTVGA BIOS v3.C10 crashes when switching to mode 0x21 or 0x27*/
		if(video == 0x21 || video == 0x27) {
			in.h.ah = 0x00;
			in.h.al = 0x13;
			int86(0x10,&in,&out);
		}
	}
    in.h.ah = 0x00;
    in.h.al = video;
    int86(0x10,&in,&out);
    return oldVideo;
}

/*
@Action: Reads bitmap file header (before information)
@Parameters: stream=file stream. structure bitmapFileHeader=structure pointer, we will write data there!
@Output: void, but overwrites structure bitmapFileHeader
*/
void _Cdecl readBitmapHeader(FILE *stream, struct bitmapFileHeader *b, struct bitmapInfoHeader *e) {
	static uint32_t headerSize;
    static uint32_t wide;
    static uint32_t tall;
    static uint16_t planes;
    static uint16_t bitsPerPixel;
    static uint32_t compression;
    static uint32_t sizeOfImage;
    static uint32_t xPixelsPerMeter;
    static uint32_t yPixelsPerMeter;
    static uint32_t numberOfColors;
    static uint32_t importantColors;
    static uint32_t sizeOfFile,reserved,offset;
    /*Read file header*/
    fread(b->type,sizeof(uint16_t),1,stream);
    fread(&sizeOfFile,sizeof(uint32_t),1,stream);
    fread(&reserved,sizeof(uint32_t),1,stream);
    fread(&offset,sizeof(uint32_t),1,stream);
    b->sizeOfFile = sizeOfFile;
    b->reserved = reserved;
    b->offset = offset;
    /*Now information*/
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
    e->headerSize = headerSize;
    e->wide = wide;
    e->tall = tall;
    e->planes = planes;
    e->bitsPerPixel = bitsPerPixel;
    e->compression = compression;
    e->sizeOfImage = sizeOfImage;
    e->xPixelsPerMeter = xPixelsPerMeter;
    e->yPixelsPerMeter = yPixelsPerMeter;
    e->numberOfColors = numberOfColors;
    e->importantColors = importantColors;
    return;
}

/*
@Action: Reads the image of a bitmap and then outputs it
@Parameters: stream=file stream. wide=wide of image. tall=tall of image.
@Output: data pointer
*/
uint8_t * _Cdecl readBitmapData(FILE *stream, uint32_t wide, uint32_t tall) {
    static uint32_t i;
    static uint32_t i2;
    static uint16_t hold;
    uint8_t *data;
    fpos_t pos;
    if(tall == 0 || wide == 0) {
        return 0;
    }
    data = (uint8_t *)malloc(wide*tall);
    if(data == NULL) {
		#ifdef _LEAF_ERROR
		setLeafError(0);
		#endif
        return 0; /*Up to caller's, how to handle errors*/
    }
    fgetpos(stream,&pos);
    for(i = 1; i < tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) {
			fread(&hold,sizeof(uint8_t),1,stream);
			data[(i2+((tall-i)*wide))] = hold;
		}
    }
    return (uint8_t *)data;
}

/*
@Action: Displays image
@Parameters: data=data pointer. x=x pos. y=y pos. wide=wide of image. tall=tall of image
@Output: void
*/
void _Cdecl displayImage(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall) {
    static uint32_t i;
    static uint32_t i2;
    for(i2 = 0; i2 < tall; i2++) {
        for(i = 0; i < wide; i++) {
            plotPixel(x+i,y+i2,data[i+(i2*wide)]);
        }
    }
    return;
}

/*
@Action: Displays image while reading file
@Parameters: x=x pos. y=y pos. wide=wide of image. tall=tall of image
@Output: void
*/
void _Cdecl displayBitmapImageWhileReading(FILE *stream, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall) {
    static uint32_t i;
    static uint32_t i2;
    static uint16_t hold;
    for(i = tall; i > 0; i--) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) {
			fread(&hold,sizeof(uint8_t),1,stream);
			/*(i2+((tall-i)*wide))*/
			plotPixel(x+i2,y+i,hold);
		}
    }
    return;
}

/*
@Action: Displays image in tiled mode
@Parameters: data=data pointer. x=x pos. y=y pos. wide=wide of tile. tall=tall of tile. index=tile number
@Output: void
*/
void _Cdecl displayImageTile(uint8_t *data, uint32_t x, uint32_t y, uint32_t wide, uint32_t tall, uint32_t index) {
    static uint32_t i;
    static uint32_t i2;
    for(i2 = 0; i2 < tall; i2++) {
        for(i = 0; i < wide; i++) {
			if(data[(index*(wide*tall))+i+(i2*wide)] != 0x24) {
				plotPixel(x+i,y+i2,data[(index*(wide*tall))+i+(i2*wide)]);
			}
        }
    }
    return;
}

/*
@Action: Saves bitmap
@Parameters: stream=file stream. bfh=bitmap file header pointer. bih=bitmap info header pointer.
data=data pointer
@Output: void
*/
void _Cdecl writeBitmap(FILE *stream, struct bitmapFileHeader *bfh, struct bitmapInfoHeader *bih, uint8_t *data) {
	static uint32_t sizeOfFile,reserved,offset; /*We do the same as in our read routines*/
    static uint32_t headerSize;					/*we just switch the order of stuff and*/
    static uint32_t wide;						/*change read with write.*/
    static uint32_t tall;
    static uint16_t planes;
    static uint16_t bitsPerPixel;
    static uint32_t compression;
    static uint32_t sizeOfImage;
    static uint32_t xPixelsPerMeter;
    static uint32_t yPixelsPerMeter;
    static uint32_t numberOfColors;
    static uint32_t importantColors;
    static uint32_t i;
    static uint32_t i2;
    static uint16_t hold;
	sizeOfFile = bfh->sizeOfFile;
    reserved = bfh->reserved;
    offset = bfh->offset;
    headerSize = bih->headerSize;
    wide = bih->wide;
    tall = bih->tall;
    planes = bih->planes;
    bitsPerPixel = bih->bitsPerPixel;
    compression = bih->compression;
    sizeOfImage = bih->sizeOfImage;
    xPixelsPerMeter = bih->xPixelsPerMeter;
    yPixelsPerMeter = bih->yPixelsPerMeter;
    numberOfColors = bih->numberOfColors;
    importantColors = bih->importantColors;
    fwrite(bfh->type,sizeof(uint16_t),1,stream);
    fwrite(&sizeOfFile,sizeof(uint32_t),1,stream);
    fwrite(&reserved,sizeof(uint32_t),1,stream);
    fwrite(&offset,sizeof(uint32_t),1,stream);
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
    /*Palette*/
    for(i = 0; i < 1022; i++) {
		fwrite(&i,sizeof(uint8_t),1,stream);
	}
	/*Bitmap data*/
	for(i = 1; i < tall+1; i++) { /*Reverse scan, reverse tall, but not wide*/
		for(i2 = 0; i2 < wide; i2++) { /*We just replotting our shit back again!*/
			hold = data[(i2+((tall-i)*wide))];
			fwrite(&hold,sizeof(uint8_t),1,stream);
		}
    }
	return;
}

/*
@Action: Gets video adapter
@Parameters: void
@Output: Video adapter [see below]
-0 Unknown (probably SVGA)
-1 AHEAD adapters
-2 PARADISE adapters
-3 OAK TECH adapters
-5 ATI 18800
-6 ATI 18800-1
-7 ATI 18800-2
-8 ATI 18800-4
-9 ATI 18800-5
-10 ATI 68800AX
-11 EGA Wonder
-12 VGA Wonder
-13 EGA Wonder8000+
-14 Genoa 6200/6300
-15 Genoa 6400/6600
-16 Genoa 6100
-17 Genoa 5100/5200
-18 Genoa 5300/5400
*/
int32_t _Cdecl getVideoAdapter(void) {
	static uint8_t *ptr;
	static uint8_t *txt;
	txt = (uint8_t *)malloc(sizeof(uint8_t)*6);
	if(txt == NULL) {
		return -1;
	}
	ptr = (uint8_t *)0xC0000025; /*Check for AHEAD adapters*/
	strcpy(txt,"AHEAD\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		return 1;
	}
	txt = (uint8_t *)realloc(txt,sizeof(uint8_t)*5);
	ptr = (uint8_t *)0xC000007D; /*Check for PARADISE adapters*/
	strcpy(txt,"VGA=\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		return 2;
	}
	txt = (uint8_t *)realloc(txt,sizeof(uint8_t)*8);
	ptr = (uint8_t *)0xC0000008; /*Check for OAK TECH adapters*/
	strcpy(txt,"OAK VGA\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		return 3;
	}
	txt = (uint8_t *)realloc(txt,sizeof(uint8_t)*10);
	ptr = (uint8_t *)0xC0000031; /*Check for ATI adapters*/
	strcpy(txt,"761295520\0"); /*Comparing memory, with string!*/
	if(memcmp(ptr,txt,5) == 0) {
		ptr = (uint8_t *)0xC0000043; /*We also have to put the stupid revision*/
		switch(*ptr) {
			case 0x31:
				free(txt);
				return 5; /*18800*/
			case 0x32:
				free(txt);
				return 6; /*18800-1*/
			case 0x33:
				free(txt);
				return 7; /*18800-2*/
			case 0x34:
				free(txt);
				return 8; /*18800-4*/
			case 0x35:
				free(txt);
				return 9; /*18800-5*/
			case 0x62:
				free(txt);
				return 10; /*68800AX*/
		}
		ptr = (uint8_t *)0xC0000040;
		switch(ptr[0]) {
			case '2':
				switch(ptr[1]) {
					case '2':
						return 11; /*EGA Wonder*/
				}
				break;
			case '3':
				switch(ptr[1]) {
					case '1':
						return 12; /*VGA Wonder*/
					case '2':
						return 13; /*EGA Wonder8000+*/
				}
				break;
		}
	}
	free(txt);
	ptr = (uint8_t *)0xC0000037; /*Genoa Graphics Adapter*/
	ptr = (uint8_t *)0xC0000000+(*ptr); /*Fuck, what a mess!*/
	if(ptr[0] == 0x77 && ptr[2] == 0x99 && ptr[3] == 0x66) {
		switch(ptr[1]) {
			case 0x00:
				return 14; /*Genoa 6200/6300*/
			case 0x11:
				return 15; /*Genoa 6400/6600*/
			case 0x22:
				return 16; /*Genoa 6100*/
			case 0x33:
				return 17; /*Genoa 5100/5200*/
			case 0x55:
				return 18; /*Genoa 5300/5400*/
		}
	}
	return 0;
}

/*
@Action: Plots a line
@Parameters: fx=from x. fy=from y. tx=to x. ty=to y. color=color (byte)
@Output: void
*/
/*Adapted from wikipedia's pseudo code*/
void plotLine(int16_t fx, int16_t fy, int16_t tx, int16_t ty, uint8_t color) {
	static int16_t dx;
	static int16_t dy;
	static int16_t xi;
	static int16_t yi;
	static int16_t d;
	static int16_t x;
	static int16_t y;
	static int16_t i;
	if(abs(ty-fy) < abs(tx-fx)) { /*If ty-fy < tx-fx the line is more horizontal*/
		if(fx > tx) { /*STarting X is higher than ending X*/
			dx=fx-tx;
			dy=fy-ty;
			yi=1;
			if(dy < 0) {
				yi = -1;
				dy = -dy;
			}
			d = 2*dy - dx;
			y = fy;
			for(i = fx; !(i > tx); i++) {
				plotPixel(i,y,color);
				if(d > 0) {
					y = y+yi;
					d = d -2*dx;
				}
				d = d +2*dy;
			}
		}
		else {
			dx=tx-fx;
			dy=ty-fy;
			yi=1;
			if(dy < 0) {
				yi = -1;
				dy = -dy;
			}
			d = 2*dy - dx;
			y = fy;
			for(i = fx; !(i > tx); i++) {
				plotPixel(i,y,color);
				if(d > 0) {
					y = y+yi;
					d = d -2*dx;
				}
				d = d +2*dy;
			}
		}
	}
	else {
		if(fy > ty) { /*Starting Y is higher than ending Y*/
			dx = fx-tx;
			dy = fy-ty;
			xi = 1;
			if(dx < 0) {
				xi = -1;
				dx = -dx;
			}
			d = 2*dx-dy;
			x=fx;
			for(i = fy; !(i > ty); i++) {
				plotPixel(x,i,color);
				if(d > 0) {
					x = x+xi;
					d = d-2*dy;
				}
				d=d+2*dx;
			}
		}
		else {
			/*plotLineHigh(x0, y0, x1, y1)*/
			dx = tx-fx;
			dy = ty-fy;
			xi = 1;
			if(dx < 0) {
				xi = -1;
				dx = -dx;
			}
			d = 2*dx-dy;
			x=fx;
			for(i = fy; !(i > ty); i++) {
				plotPixel(x,i,color);
				if(d > 0) {
					x = x+xi;
					d = d-2*dy;
				}
				d=d+2*dx;
			}
		}
	}
	return;
}

/*
@Action: Initializes mouse
@Parameters: structure mouse=structure of the mouse, there should be one per program (recommended)
because this structure will hold all mouses. And i don't think anyone uses more than 2
bloody mouses.
@Output: void, but returns overwritten structure *m
*/
char _Cdecl initMouse(struct mouse *m) {
	in.x.ax = 0x00;
	in.x.bx = 0x00;
	int86(0x33,&in,&out);
	if((in.x.bx&2) != 0) { /*Two button mouse*/
        m->buttons = 2;
	}
	else if((in.x.bx&3) != 0) { /*Three button mouse*/
        m->buttons = 3;
	}
	else { /*Unknown buttons*/
        m->buttons = 0;
	}
	#ifdef _LEAF_ERROR
	if(out.x.ax == 0) {
		setLeafError(1);
	}
	#endif
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
}

/*
@Action: Sets position of mouse
@Parameters: x=x position. y=y position
@Output: void
*/
void _Cdecl setMousePosition(uint16_t x,uint16_t y) {
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
	return;
}

/*
@Action: Shows mouse
@Parameters: void
@Output: void
*/
void _Cdecl showMouse(void) {
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
	return;
}

/*
@Action: Hide mouse
@Parameters: void
@Output: void
*/
void _Cdecl hideMouse(void) {
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
	return;
}

/*
@Action: Get mouse status
@Parameters: structure mouse=structure of the mouse to write to
@Output: void, but returns overwritten structure *m
*/
void _Cdecl getMouseStatus(struct mouse *m) {
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
	return;
}

/*
@Action: "redraws" black area left by initialized mouse once a thing has been draw
@Parameters: structure mouse=structure of the mouse
@Output: void
*/
void _Cdecl redrawOnMouse(struct mouse *m) {
    static uint16_t i,i2;
    for(i = 0; i < 16; i++) {
        for(i2 = 0; i2 < 16; i2++) {
            plotPixel(i+(m->x/2),i2+(m->y/2),fetchPixel(i+(m->x/2),i2+(m->y/2)));
        }
    }
    return;
}

/*
@Action: Skips *n* number of chars
@Parameters: stream=file stream. n=number of chars to skip
@Output: void
*/
void _Cdecl fskip(FILE *stream, uint64_t n) {
    static uint64_t i;
    for(i = 0; i < (n+1); i++) {
        fgetc(stream); /*Skip characters*/
    }
    return;
}
