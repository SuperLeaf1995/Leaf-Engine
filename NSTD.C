void _Cdecl getBIOSEquipementList(struct computerBIOSEquipementList *comp) {
	register word a;
	register word c;
	a = biosequip();
	comp->diskDrive = ((a & 0x1) >= 0x1) ? 1 : 0;
	comp->mathCoProcessor = ((a & 0x2) >= 0x2) ? 1 : 0;
	/*How many memory we have*/
	comp->systemMemory = ((a & 3) >= 3) ? (((a & 7) >= 7) ? 64000 : 32000) : (((a & 7) >= 7) ? 48000 : 16000);
	comp->drives = ((a & 0x40-1) >= 0x40-1) ? 1 : 0;
	comp->drives += ((a & 0x80-1) >= 0x80-1) ? 1 : 0;
	comp->noDMA = ((a & 0x100-1) >= 0x100-1) ? 1 : 0;
	comp->serialPorts = ((a & 0x200-1) >= 0x200-1) ? 0x1 : 0x0;
	comp->serialPorts += ((a & 0x400-1) >= 0x400-1) ? 0x2 : 0;
	comp->serialPorts += ((a & 0x800-1) >= 0x800-1) ? 0x4 : 0;
	comp->gameAdapter = ((a & 0x1000-1) >= 0x1000-1) ? 1 : 0;
	/*Sorry PS/2 users*/
	comp->serialInstalled = ((a & 0x2000-1) >= 0x2000-1) ? 1 : 0;
	comp->serialPrinter = ((a & 0x4000-1) >= 0x4000-1) ? 1 : 0;
	comp->serialPrinter += ((a & 0x8000-1) >= 0x8000-1) ? 2 : 0;
	return;
}

/*Display routines*/

/*Initialize mode*/
void _Cdecl initGraph(void)
{
	/*Lets use our mind to change to VGA 256 mode*/
	in.h.ah = 0x00;
	in.h.al = 0x13;
	int86(0x10 ,&in, &out);
	return;
}

/*Plot a pixel with X and Y auto calculation*/
void _Cdecl plotPixel(register word x, register word y, byte color)
{
	vgaMemory[((y<<8)+(y<<6)+x)] = color;
	return;
}

/*Plot a pixel without taking X, Y in account*/
void _Cdecl plotLinearPixel(register word coord, register byte color) {
	vgaMemory[coord] = color;
	return;
}

/*Plots a line, be careful*/
void _Cdecl plotLine(word fx, word fy, word tx, word ty, byte color) {
	int dx,dy,px,py,x,y,i;
	float sl;
	if(tx >= fx) {
		dx = tx-fx;
	}
	else {
		dx = fx-tx;
	}
	if(ty >= fy) {
		dy = ty-fy;
	}
	else {
		dy = fy-ty;
	}
	if(abs(dy) >= abs(dx)) {
		sl=(float)dy/(float)dx;
		for(i = 0; i != dx; i+= 1) {
			px = i+fx;
			py = sl*i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	else {
		sl=(float)dx/(float)dy;
		for(i = 0; i != dy; i+= 1) {
			py = sl*i+fx;
			px = i+fy;
			vgaMemory[((py<<8)+(py<<6)+px)] = color;
		}
	}
	return;
}

/*TODO: VGA registers instead of BIOS ones*/
/*Gets current video mode*/
word _Cdecl getVideoMode(void) {
	in.x.ax = 0x0F;
	int86(0x10 ,&in, &out);
	return out.h.al;
}

/*End graphics*/
void _Cdecl endGraph(void)
{
	in.h.ah = 0x00;
	in.h.al = 0x03;
	int86(0x10 ,&in, &out);
	return;
}

/*Other stuff for video*/
/*Custom charset*/
void _Cdecl printCustomCharset(byte *ptr, char * text, word x, word y) {
	register byte a,b,c;
	for(c = 0; c < strlen(text); c++) {
		for(a = 0; a < 8; a++) {
			for(b = 0; b < 8; b++) {
				if(ptr[(((text[c])-65)*64)+(b*8)+a] != 0) {
					plotPixel(x+(a+(c*8)),y+b,ptr[(((text[c])-65)*64)+(b*8)+a]);
				}
			}
		}
	}
	return;
}

/*Plot 16x16 tile/object, etc*/
void _Cdecl plotTile16(byte *ptr, register byte index, register word x, register word y) {
	register word c = 0;
	while(c != 256) {
		/*Do it manually, we need to save the pain of calling
		 a stupid function*/
		vgaMemory[((y+(c>>4))<<8)+((y+(c>>4))<<6)+(x+(c&15))] = ptr[(index<<8)+c];
		c++;
	}
	return;
}

/*Plot a tile of any other size*/
void _Cdecl plotTile(byte *ptr, register byte index, register word x, register word y, size_t w, size_t t) {
	register word c = 0;
	register word d = 0;
	for(c = 0; c < w; c++) {
		for(d = 0; d < t; d++) {
			vgaMemory[(((y+d)<<8)+((y+d)<<6)+(x+c))] = ptr[(index<<8)+(d*w)+c];
		}
	}
	return;
}

/*TODO: Own mouse driver*/
/*Initialize mouse*/
char _Cdecl initMouse(void) {
	in.x.ax = 0;
	in.x.bx = 0;
	int86(0x33, &in, &out);
	if(out.x.ax != 0) {
		return 0;
	}
	else {
		/*Write to STDERR*/
		fprintf(stderr,"Mouse could not initialize");
		return -1;
	}
}

/*Set mouse position*/
void _Cdecl setMousePosition(word x, word y) {
	in.x.ax = 4;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33, &in, &out);
	return;
}

/*Display mouse*/
void _Cdecl showMouse(void) {
	in.x.ax = 1;
	int86(0x33, &in, &out);
	return;
}

/*Hide the mouse*/
void _Cdecl hideMouse(void) {
	in.x.ax = 2;
	int86(0x33, &in, &out);
	return;
}

/*Get Mouse status*/
void _Cdecl getMouse(struct mouse *m) {
	in.x.ax = 3;
	int86(0x33, &in, &out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->bLeft = (out.x.bx & 1);
	m->bRight = (out.x.bx & 2);
	m->bMiddle = (out.x.bx & 4);
	return;
}

/*Get if in BOX area*/
byte _Cdecl mouseBoxGet(struct mouse *b, word fx, word fy, word tx, word ty) {
	getMouse(b);
	if( b->x >= fx && b->x <= tx &&
		b->y >= fy && b->y <= ty) {
		return 1;	
	}
	else {
		return 0;
	}
}

/*Physics engine*/

/*Movable JUMP. Mid-jump interaction*/
void _Cdecl objectPhysicsBasicJump(struct objectBasicProperties *obj, /*Object we are jumping*/
							word force, /*How many we jumped, set to 32 for normal jump*/
							int leftKey, /*Left and Right key, up to the coder!*/
							int rightKey,
							signed char or) /*Needed for inverted axis games, set either -1 for normal or 1 for non normal*/ {
	register word a;
	register int b;
	/*Go UP*/
	for(a = 0; a < force; a++) {
		obj->y++;
		/*Render*/
		if(kbhit()) {
			b = getch();
			switch(b) {
				case leftKey:
					obj->x += or;
					break;
				case rightKey:
					obj->x -= or;
					break;
			}
		}
	}
	/*Down, weee*/
	for(a = force; a > 0; a++) {
		obj->y--;
		/*Render*/
		if(kbhit()) {
			b = getch();
			switch(b) {
				case leftKey:
					obj->x -= or;
					break;
				case rightKey:
					obj->x += or;
					break;
			}
		}
	}
	return;
}
