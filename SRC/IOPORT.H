#ifndef __IOPORT_H__
#define __IOPORT_H__

/* DJGPP uses inportb and outportb for writes to IO ports */
#if defined(__DJGPP__) || defined(__GO32__)
#include <pc.h>
#elif !defined(__BORLANDC__) && defined(__TURBOC__)
#define inpb(p) inportb(p)
#define outb(p,v) outportb(p,v)
#endif

#endif
