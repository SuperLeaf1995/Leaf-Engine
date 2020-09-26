#include "sound.h"

#if defined(OPENAL)
ALuint buffer;
#endif

/**
@brief Initializes everything needed for sound-support
*/
signed char Leaf_InitSound(void)
{
#if defined(OPENAL)
	/*Open default AL device*/
	Leaf_CurrentContext->alDev = alcOpenDevice(NULL); /*Choose default device (NULL) for playback*/
	if(!Leaf_CurrentContext->alDev) {
		return -1;
	}
	/*Create a OpenAL context*/
	Leaf_CurrentContext->alCtx = alcCreateContext(Leaf_CurrentContext->alDev,NULL);
	if(!alcMakeContextCurrent(Leaf_CurrentContext->alCtx)) {
		return -2;
	}
	/*Create a main audio object*/
	alGenSources(1,&Leaf_CurrentContext->alSoundSrc);
	alGenBuffers(1,&buffer);
	
	alSourcei(Leaf_CurrentContext->alSoundSrc,AL_BUFFER,buffer);
#endif
	return 0;
}

/**
@brief Plays a sound with the given freq

@param freq Is the frequency for the sound
*/
/*TODO: Clean this mess*/
void Leaf_PlaySound(unsigned long freq)
{
#if defined(OPENAL)
	size_t i;
	ALint srcs; unsigned char * data;
	
	data = malloc(32);
	if(data == NULL) { return; }
	
	for(i = 0; i < 32; i++) {
		data[i] = freq;
	}
	
	alBufferData(buffer,AL_FORMAT_MONO8,data,32,8000);
	alGetSourcei(Leaf_CurrentContext->alSoundSrc,AL_SOURCE_STATE,&srcs);
	while(srcs == AL_PLAYING) {
		alSourcePlay(Leaf_CurrentContext->alSoundSrc);
	}
	
	free(data);
#endif
	return;
}

/**
@brief Stops all playing sounds
*/
void Leaf_StopSound(void)
{
#if defined(OPENAL)
	alSourceStop(Leaf_CurrentContext->alSoundSrc);
#endif
	return;
}
