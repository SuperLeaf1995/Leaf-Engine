#ifndef PCSPEAK_H
#define PCSPEAK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dos.h>

#include "stddefc.h"

void soundPlay(uint32_t x);
void soundStop(void);
int8_t soundPlayRawSoundFile(FILE *stream);

#endif
