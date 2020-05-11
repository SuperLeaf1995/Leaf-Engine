#include "context.h"

#if defined(__MSDOS__) || defined(__DOS__) || defined(_MSDOS) || defined(MSDOS) || defined(FREEDOS)
#include "dos\context.c"
#elif defined(__linux) || defined(linux)
#include "linux/context.c"
#elif defined(__GBA__)
#include "gba/context.c"
#endif
