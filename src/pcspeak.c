/* PCSPEAK.C
 * C file for PC Speaker functions
 */

#include "pcspeak.h"

void soundPlay(uint32_t x) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	uint32_t cot; /*countdown value*/
	uint8_t tmp; /*temporal value stuff*/
	cot = (uint32_t)(1193180/x);
	outportb(0x43,0xb6); /*say to the speaker we are going to send data*/
	outportb(0x42,(uint8_t)cot); /*output low byte of countdown*/
	outportb(0x42,(uint8_t)cot>>8); /*and then the high byte*/
	tmp = inportb(0x61); /*set pit2 timer*/
	if(tmp != (tmp|3)) {
		outportb(0x61,tmp|3);
	}
#endif
	x = 1;
	return;
}

void soundStop(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	uint8_t tmp = inportb(0x61)&0xfc; /*shutdown speaker command*/
	outportb(0x61,tmp);
#endif
	return;
}

int8_t soundPlayRawSoundFile(FILE *stream) {
	int16_t i = fgetc(stream);
	if(i == EOF) {
		fseek(stream,SEEK_SET,0); /*rewind file*/
	}
#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
	soundPlay(i*30); /*TODO: know why multiplying by 30 makes a nice noise*/
#endif
	return 0;
}
