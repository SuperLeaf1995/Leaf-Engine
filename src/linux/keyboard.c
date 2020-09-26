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

static unsigned char kbuf[0xFFFF];
static int khit = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Leaf_EventUpdate(void);
void Leaf_EventUpdate(void) {
	int c;
	while(XPending(Leaf_CurrentContext->xdisplay)) {
		XNextEvent(Leaf_CurrentContext->xdisplay,&Leaf_CurrentContext->xevent);
		
		if(Leaf_CurrentContext->xevent.type == KeyPress) {
			c = XLookupKeysym(&Leaf_CurrentContext->xevent.xkey,0);
			kbuf[c] = 1; khit = 1;
		} else if(Leaf_CurrentContext->xevent.type == KeyRelease) {
			c = XLookupKeysym(&Leaf_CurrentContext->xevent.xkey,0);
			kbuf[c] = 0; khit = 0;
		} else if(Leaf_CurrentContext->xevent.type == ClientMessage
		&& (unsigned int)Leaf_CurrentContext->xevent.xclient.data.l[0] == Leaf_CurrentContext->WM_DELETE_WINDOW) {
			Leaf_CurrentContext->ui = __LEAF_UI_EXIT_CODE;
		} else {
			
		}
	}
}
#pragma GCC diagnostic pop

int Leaf_Kbhit(void) {
	Leaf_EventUpdate();
	return khit;
}

int Leaf_Getch(void) {
	size_t i;
	while(1) {
		Leaf_EventUpdate();
		for(i = 0; i < 0xFF; i++) {
			if(kbuf[i] == 1) {
				return i;
			}
		}
	}
}
