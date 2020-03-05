#if defined(__GNUC__)
#define _Cdecl __attribute__((cdecl))
#endif

extern void _Cdecl plotPixel(unsigned short x, unsigned short y, unsigned char color);
extern void _Cdecl setVideo(unsigned char video);
extern void _Cdecl soundStart(unsigned short freq);
extern void _Cdecl soundStop(void);
