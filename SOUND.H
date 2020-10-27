#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdlib.h>

/* transfer mode */
#define SB_SINGLE			0x48
#define SB_AUTO				0x58

#define SB_PLAYSOUND		0x00

struct Leaf_Sound_Context {
	unsigned irq;
	unsigned base;
};

struct Leaf_Sound {
	void * data;
	size_t len;
	size_t hz;
};

int Leaf_Sound_Init(struct Leaf_Sound_Context * ctx, char bits);
int Leaf_Sound_InitChannel(struct Leaf_Sound_Context * ctx, int channel);
int Leaf_Sound_Play(struct Leaf_Sound_Context * ctx, struct Leaf_Sound * sample, int channel);
int Leaf_Sound_CreateSample(struct Leaf_Sound * sample, unsigned hz, unsigned sec);
int Leaf_Sound_DestroySample(struct Leaf_Sound * sample);

int Leaf_Sound_DriverSB_Init(int base, int irq, char inout, int bits);
int Leaf_Sound_DriverSB_InitPlay(int base);
int Leaf_Sound_DriverSB_InitChannel(int base, int channel, int mode);
int Leaf_Sound_DriverSB_SetChannel(int base, signed int channel,int hz, int transmode, int soundtype, void * data, size_t len);
int Leaf_Sound_DriverSB_Pause(int base, int bits);
int Leaf_Sound_DriverSB_Continue(int base, int bits);

#endif
