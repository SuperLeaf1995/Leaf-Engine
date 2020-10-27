#include <dos.h>

#include "sound.h"
#include "ioport.h"

int Leaf_Sound_Init(struct Leaf_Sound_Context * ctx, char bits) {
	unsigned i, j, sb_base, sb_irq;
	
	for(i = 0x00; i < 0xF0; i += 0x10) {
		for(j = 0; j < 3; j++) {
			if(Leaf_Sound_DriverSB_Init(i,0,SB_PLAYSOUND,bits)) {
				sb_base = i;
				sb_irq = j;
				goto putIntoStruct;
			}
		}
	}
	
	putIntoStruct:
	ctx->base = sb_base;
	ctx->irq = sb_irq;
	return 0;
}

int Leaf_Sound_InitChannel(struct Leaf_Sound_Context * ctx, int channel) {
	Leaf_Sound_DriverSB_InitChannel(ctx->base,channel,SB_AUTO);
	return 0;
}

int Leaf_Sound_Play(struct Leaf_Sound_Context * ctx, struct Leaf_Sound * sample, int channel) {
	Leaf_Sound_DriverSB_SetChannel(ctx->base,channel,sample->hz,0xC0,0x00,sample->data,sample->len);
	return 0;
}

int Leaf_Sound_End(struct Leaf_Sound_Context * ctx) {
	return 0;
}

int Leaf_Sound_CreateSample(struct Leaf_Sound * sample, unsigned hz, unsigned sec) {
	sample->hz = hz;
	sample->len = hz*sec;
	
	sample->data = malloc(sample->len);
	if(sample->data == NULL) {
		return 1;
	}
	return 0;
}

int Leaf_Sound_DestroySample(struct Leaf_Sound * sample) {
	free(sample->data);
	return 0;
}

int Leaf_Sound_DriverSB_Init(int base, int irq, char inout, int bits) {
	unsigned int i;
	unsigned dspVersion;
	
	/* Soundblaster IRQ table */
	const int irqToNum[4][2] = {
		{ 0x01, 0x02 },{ 0x02, 0x05 },
		{ 0x04, 0x07 },{ 0x08, 0x0A }};
	
	/* Send first DSP */
	outb((0x206+base),1);
	
	/* Wait 3ms */
	for(i = 0; i < 30000; i++) {
		
	}
	
	/* Do final DSP reset */
	outb((0x206+base),0);
	
	/* Read final 0xAA code */
	if(inportb(0x20A+base) != 0xAA) {
		return 1;
	}
	
	/* Obtain Soundblaster DSP version */
	while(inportb(0x20C+base)&0x80) {}; outportb(0x20C+base,0xE1);
	dspVersion = inportb(0x20A+base)<<8;
	dspVersion |= inportb(0x20A+base);
	
	/* Send IRQ to send data to */
	outb(0x204+base,0x80);
	outb(0x205+base,irqToNum[irq][0]);
	
	switch(inout) {
		case 0:
			/* Start the speaker */
			while(inportb(0x20C+base)&0x80) {}; outb(0x20C+base,0xD1);
			break;
		case 1:
			break;
		default:
			break;
	}
	return 0;
}


int Leaf_Sound_DriverSB_End(void) {
	return 0;
}

int Leaf_Sound_DriverSB_InitChannel(int base, int channel, int mode) {
	/* Initialize playback on channel */
	outb(0x0A,0x05+channel);
	outb(0x0C,0x01);
	outb(0x0B,mode+channel);
	return 0;
}

int Leaf_Sound_DriverSB_SetChannel(int base, signed int channel,
int hz, int transmode, int soundtype, void * data, size_t len) {
	unsigned long def_addr = (unsigned long)data;
	unsigned long addr;
	
	if(channel == 0) {
		channel = -1;
	}
	
	/* Get linear address of data pointer */
	addr = ((def_addr>>16)*16)+(def_addr&0xFFFF);
	addr = (addr&0xFFFE);
	
	/* Output the stuff to the SB16 ports */
	outb(0x01+channel,(addr&0xFF));
	outb(0x01+channel,((addr>>8)&0xFF));
	outb(0x02+channel,(len&0xFF));
	outb(0x02+channel,((len>>8)&0xFF));
	outb(0x0A,1);
	
	/* Set SB16 sample rates */
	outb(0x20C+base,0x41);
	outb(0x20C+base,hz>>8);
	outb(0x20C+base,hz&0xFF);
	
	/* Send mode to the SB16 */
	outb(0x20C+base,transmode);
	outb(0x20C+base,soundtype);
	
	/* Lenght of data - 1*/
	outb(0x20C+base,len-1);
	outb(0x20C+base,(len>>8)-1);
	return 0;
}

int Leaf_Sound_DriverSB_Pause(int base, int bits) {
	switch(bits) {
		case 8:
			outb(0x20C+base,0xD0);
			break;
		case 16:
			outb(0x20C+base,0xD5);
			break;
	}
	return 0;
}

int Leaf_Sound_DriverSB_Continue(int base, int bits) {
	switch(bits) {
		case 8:
			outb(0x20C+base,0xD4);
			break;
		case 16:
			outb(0x20C+base,0xD6);
			break;
	}
	return 0;
}
