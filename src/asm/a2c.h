/*A2C.H
Provides header to access assembly functions of ALL assembly files in C
*/

#if defined(__GNUC__)
#define _Cdecl __attribute__((cdecl))
#endif

extern void _Cdecl setVideoPixel(unsigned short x, unsigned short y, unsigned char color);
extern void _Cdecl getVideoPixel(unsigned short x, unsigned short y);

extern void _Cdecl setVideoMode(unsigned char video);
extern unsigned char _Cdecl getVideoMode(void);
extern void _Cdecl setVideoPalette(unsigned char *ptr, unsigned short n);

extern void _Cdecl setCursorStatus(unsigned char status);
extern void _Cdecl setCursorPosition(unsigned short x, unsigned short y);

/*deprecated*/
extern void _Cdecl vgaPlotPixel(unsigned short x, unsigned short y, unsigned char color);
extern void _Cdecl plotPixel(unsigned short x, unsigned short y, unsigned char color);
extern void _Cdecl plotVideoPixel(unsigned short x, unsigned short y, unsigned char color);
extern void _Cdecl setPixel(unsigned short x, unsigned short y, unsigned char color);

extern void _Cdecl vgaFetchPixel(unsigned short x, unsigned short y);
extern void _Cdecl vgaGetPixel(unsigned short x, unsigned short y);
extern void _Cdecl getPixel(unsigned short x, unsigned short y);

extern void _Cdecl setVideo(unsigned char video);
extern unsigned char _Cdecl getVideo(void);
extern void _Cdecl setPalette(unsigned char *ptr, unsigned short n);

extern void _Cdecl setMouseStatus(unsigned char status);
extern void _Cdecl setMousePosition(unsigned short x, unsigned short y);
