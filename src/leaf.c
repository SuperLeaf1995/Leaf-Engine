/*
 * Leaf-Engine Base code
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "leaf.h"

void seedRandom(void) {
#if defined(__linux) || defined(linux)
	srand(time(NULL));
#elif defined(__TURBOC__) && !defined(__BORLANDC__)
	srand(*biosClock);
#endif
	return;
}

signed int generateRandom(void) {
#if defined(__GBA__)

#else
	return rand();
#endif
}

signed int leafContextCreate(leafContext * g) {
#if defined(__DJGPP__)
	if(__djgpp_nearptr_enable() == 0) {
		fprintf(stderr,"Cannot enable nearptr\n");
		exit(-1);
	}
#endif /* __DJGPP__ */
#if defined(__GBA__)
	g->vwide = 240; g->vtall = 160;
#else
	g->vwide = 320; g->vtall = 200;
#endif
	videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(videoBuffer == NULL) {
		return -1;
	}
#if defined(__GBA__) || defined(__linux) || defined(linux)
	g->rgbPalette = (paletteEntry *)malloc(sizeof(paletteEntry)*256);
	if(g->rgbPalette == NULL) {
		return -2;
	}
#endif
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
	leafCurrentCtx = g;
	return 0;
}

signed int leafContextDestroy(leafContext * g) {
	if(videoBuffer != NULL) {
		free(videoBuffer);
	}
	if(g->name != NULL) {
		free(g->name);
	}
#if defined(__DJGPP__)
	__djgpp_nearptr_disable();
#endif
	return 0;
}

/**
@brief Initializes mouse driver/cursor

@param m Mouse structure
*/
char initMouse(struct mouse * m) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
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
	return (out.x.ax != 0) ? 0 : -1; /*If the mouse was initialized return 0, else return -1*/
#endif
}

/**
@brief Sets position of the mouse to given parameters

@param x X Coordinates
@param y Y Coordinates
*/
void setMousePosition(register unsigned short x, register unsigned short y) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x04;
	in.x.cx = x;
	in.x.dx = y;
	int86(0x33,&in,&out);
#endif
	return;
}

/**
@brief Shows mouse
*/
void showMouse(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x01;
	int86(0x33,&in,&out);
#endif
	return;
}

/**
@brief Hides mouse
*/
void hideMouse(void) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x02;
	int86(0x33,&in,&out);
#endif
	return;
}

/**
@brief Gets status of buttons and current position

@param m Mouse structure
*/
void getMouseStatus(struct mouse * m) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x03;
	int86(0x33,&in,&out);
	m->x = out.x.cx;
	m->y = out.x.dx;
	m->buttonLeft = (out.x.bx & 1); /*While it is not equal 0, its HOLD/PRESSED, else its RELEASED*/
	m->buttonRight = (out.x.bx & 2);
	m->buttonMiddle = (out.x.bx & 4);
#endif
	return;
}

/**
@brief Gets mouse movement

@param m Mouse structure
*/
void getMouseMovement(struct mouse * m) {
#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
	in.x.ax = 0x0B;
	int86(0x33,&in,&out);
	m->mx = out.x.cx;
	m->my = out.x.dx;
#endif
	return;
}

/**
@brief Saves data to file

@param fileName Name of the file
@param data Pointer to the place of data to put on file
@param n Size of data to write
*/
signed char saveLeafDataFile(const char * fileName, void * data, size_t n) {
	FILE * s;
	s = fopen(fileName,"wb");
	if(!s) {
		return -1;
	}
	fwrite((unsigned char *)data,sizeof(unsigned char),n,s);
	fclose(s);
	return 0;
}

/**
@brief Loads data from file

@param fileName Name of the file
@param data Pointer to the place to put data on
@param n Size of data to read
*/
signed char loadLeafDataFile(const char * fileName, void * data, size_t n) {
	FILE * s;
	s = fopen(fileName,"rb");
	if(!s) {
		return -1;
	}
	fread((unsigned char *)data,sizeof(unsigned char),n,s);
	fclose(s);
	return 0;
}

/**
@brief Opens log file

@param stream Stream/File to open
@param name Filename of the file to open
*/
signed char openLogFile(FILE * stream, const char * name) {
	stream = fopen(name,"a+t");
	if(!stream) {
		return -1;
	}
	return 0;
}

/**
@brief Appends a log entry into the log file

@param stream Stream/File to close
@param entry Contents of the logging entry
*/
signed char appendLogFile(FILE * stream, const char * entry) {
	if(!stream) {
		return -1;
	}
	fprintf(stream,"%s\n",entry);
	return 0;
}

/**
@brief Closes log file

@param stream Stream/File to close
*/
signed char closeLogFile(FILE * stream) {
	if(stream) {
		fclose(stream);
	}
	return 0;
}

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
	signed short * s[4000];
	
	for(i = 0; i < bsize; i++) {
		s[i] = (signed short)(32760*sin((3.14*freq)/rate*i));
	}
	alBufferData(b,AL_FORMAT_MONO8,s,bsize,rate);
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
	alSourcePlay(freq);
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

#ifdef __cplusplus
}
#endif /* __cplusplus */
