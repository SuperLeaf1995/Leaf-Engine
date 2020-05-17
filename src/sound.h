/*
 * Leaf-Engine sound Header
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

#ifndef __LEAF_SOUND_H__
#define __LEAF_SOUND_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*These should be supported on all targets*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>

#if defined(__linux) || defined(linux)
#include <AL/al.h>
#include <AL/alc.h>
#include <math.h>
#endif

#include "context.h"

/*
The main game context we are working on
*/
extern leafContext * leafCurrentCtx;

/*
Sound functions
*/
signed char initSound(void);
void playSound(unsigned long freq);
void stopSound(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
