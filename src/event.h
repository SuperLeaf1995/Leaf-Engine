/*
 * Leaf-Engine event Header
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

#ifndef __LEAF_LEAF_H__
#define __LEAF_LEAF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*These should be supported on all targets*/
#include <stdio.h>
#include <stdlib.h>

/*
LeafEvent is a special structure wich handles all events in a program
this should be the main structure and there should be generally one per
program.

The structure aims to provide a simple way to get/set data for input.
*/
typedef struct leafEvent {
	signed int key;
	/** Change of the mouse X */
	signed short cx;
	/** Change of the mouse Y */
	signed short cy;
	signed char eventStatus;
	/** UI code, can be used in UI-based/capable operating systems
	Always zero on DOS */
	signed char ui;
}leafEvent;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
