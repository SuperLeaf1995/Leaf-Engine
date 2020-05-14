#if defined(OPENAL)
ALuint b;
#endif

/**
@brief Initializes everything needed for sound-support
*/
signed char initSound(void) {
#if defined(OPENAL)
	/*Open default AL device*/
	leafCurrentCtx->alDev = alcOpenDevice(NULL); /*Choose default device (NULL) for playback*/
	if(!leafCurrentCtx->alDev) {
		return -1;
	}
	/*Create a OpenAL context*/
	leafCurrentCtx->alCtx = alcCreateContext(leafCurrentCtx->alDev,NULL);
	if(!alcMakeContextCurrent(leafCurrentCtx->alCtx)) {
		return -2;
	}
	/*Create a main audio object*/
	alGenSources((ALuint)1,&leafCurrentCtx->alSoundSrc);
	alSourcef(leafCurrentCtx->alSoundSrc,AL_PITCH,1);
	alSourcef(leafCurrentCtx->alSoundSrc,AL_GAIN,1);
	alSource3f(leafCurrentCtx->alSoundSrc,AL_POSITION,0,0,0);
	alSource3f(leafCurrentCtx->alSoundSrc,AL_VELOCITY,0,0,0);
	alSourcei(leafCurrentCtx->alSoundSrc,AL_LOOPING,AL_FALSE);
	
	alGenBuffers(1,&b);
#endif
	return 0;
}

/**
@brief Plays a sound with the given freq

@param freq Is the frequency for the sound
*/
void playSound(unsigned long freq) {
#if defined(OPENAL)
	unsigned rate = 80000; float sec = 0.5;
	size_t i; size_t bsize = (sec*rate);
	signed short s[4000];
	
	for(i = 0; i < bsize; i++) {
		s[i] = (signed short)(32760*sin((3.14*freq)/rate*i));
	}
	alBufferData(b,AL_FORMAT_MONO8,s,bsize,rate);
	alSourcePlay(freq);
#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	register unsigned long cot; /*countdown value*/
	register unsigned char tmp; /*temporal value stuff*/
	cot = (unsigned long)(1193180/freq);
	outp(0x43,0xb6); /*say to the speaker we are going to send data*/
	outp(0x42,(unsigned char)cot); /*output low byte of countdown*/
	outp(0x42,(unsigned char)cot>>8); /*and then the high byte*/
	tmp = inp(0x61); /*set PIT2 timer*/
	if(tmp != (tmp|3)) {
		outp(0x61,tmp|3);
	}
#endif
	return;
}

/**
@brief Stops all playing sounds
*/
void stopSound(void) {
#if defined(OPENAL)

#endif
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	register unsigned char tmp = inp(0x61)&0xfc; /*shutdown speaker command*/
	outp(0x61,tmp);
#endif
	return;
}
