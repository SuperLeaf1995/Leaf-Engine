#include "graphic.h"

unsigned int setVideo(unsigned char v) {
	poke(0xC050,0); /*Enter graphics mode*/
	poke(0xC052,0); /*Enter fullscreen*/
	poke(0xC057,0); /*Enter highres mode*/
	poke(0xC054,0); /*Select PAGE1*/
	return (unsigned int)v;
}
