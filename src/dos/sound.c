#include "sound.h"

/**
@brief Initializes everything needed for sound-support
*/
signed char Leaf_InitSound(void)
{
	return 0;
}

/**
@brief Plays a sound with the given freq

@param freq Is the frequency for the sound
*/
void Leaf_PlaySound(unsigned long freq)
{
	 unsigned long cot; /*countdown value*/
	 unsigned char tmp; /*temporal value stuff*/
	cot = (unsigned long)(1193180/freq);
	outp(0x43,0xb6); /*say to the speaker we are going to send data*/
	outp(0x42,(unsigned char)cot); /*output low byte of countdown*/
	outp(0x42,(unsigned char)cot>>8); /*and then the high byte*/
	tmp = inp(0x61); /*set PIT2 timer*/
	if(tmp != (tmp|3)) {
		outp(0x61,tmp|3);
	}
	return;
}

/**
@brief Stops all playing sounds
*/
void Leaf_StopSound(void)
{
	 unsigned char tmp = inp(0x61)&0xfc; /*shutdown speaker command*/
	outp(0x61,tmp);
	return;
}
