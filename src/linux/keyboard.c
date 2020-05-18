/*
 * Leaf-Engine keyboard
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

#include "keyboard.h"

int kbhit(void) {
	while(XPending(leafCurrentCtx->xdisplay)) {
		XNextEvent(leafCurrentCtx->xdisplay,&leafCurrentCtx->xevent);
		if(leafCurrentCtx->xevent.type == KeyPress) {
			return 1;
		} else if(leafCurrentCtx->xevent.type == KeyRelease) {
			return 0;
		}
	}
	return 0;
}

int getch(void) {
	getEvent:
	XNextEvent(leafCurrentCtx->xdisplay,&leafCurrentCtx->xevent);
	if(leafCurrentCtx->xevent.type == KeyPress) {
		return XLookupKeysym(&leafCurrentCtx->xevent.xkey,0);
	}
	goto getEvent;
}
