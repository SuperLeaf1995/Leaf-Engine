/*
 * Leaf-Engine Apple II graphic
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

#include "graphic.h"

signed char Leaf_setVideo(unsigned char v)
{
	POKE(0xC050,0); /*Enter graphics mode*/
	POKE(0xC052,0); /*Enter fullscreen*/
	POKE(0xC057,0); /*Enter highres mode*/
	POKE(0xC054,0); /*Select PAGE1*/
	return (unsigned int)v;
}