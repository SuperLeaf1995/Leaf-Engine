#ifndef __LEAF_PALETTE_H__
#define __LEAF_PALETTE_H__

/*
PaletteEntry is used for storing palette information of pictures, works
in coordinance with setPalette.
*/
typedef struct paletteEntry {
	/** Red component of the palette */
	unsigned char r;
	/** Green component of the palette */
	unsigned char g;
	/** Blue component of the palette */
	unsigned char b;
}paletteEntry;

#endif
