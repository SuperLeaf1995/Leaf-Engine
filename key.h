#ifndef KEY_H
#define KEY_H

/*Keys*/
#if defined (__MSDOS__) || defined (_MSDOS) || defined (MSDOS) || defined (__DOS__) || defined (FREEDOS)
/*Function keys*/
#define KEY_F1 0x03b
#define KEY_F2 0x03c
#define KEY_F3 0x03d
#define KEY_F4 0x03e
#define KEY_F5 0x03f
#define KEY_F6 0x040
#define KEY_F7 0x041
#define KEY_F8 0x042
#define KEY_F9 0x043
#define KEY_F10 0x044
#define KEY_F11 0x0ff85
#define KEY_F12 0x0ff86

/*Right pad keys, not the numpad!*/
#define KEY_INS 0x052
#define KEY_DEL 0x053
#define KEY_HM 0x047
#define KEY_END 0x04f
#define KEY_PGUP 0x049
#define KEY_PGDWN 0x051

/*Arrow keys*/
#define KEY_UP 0x048
#define KEY_DOWN 0x050
#define KEY_RIGHT 0x04d
#define KEY_LEFT 0x04b
#endif

#ifdef __APPLE2__ /*Apple II has their own arrow keys, fuck off*/
#define KEY_UP 0xb
#define KEY_DOWN 0xa
#define KEY_RIGHT 0x15
#define KEY_LEFT 0x8
#endif

/*Shift, tab, etc*/
#define KEY_TAB 0x9
#define KEY_ENTER 0xd
#define KEY_ESC 0x1b

#endif
