# Leaf Engine
A small game engine for MSDOS, FreeDOS, Apple II and Apple II+, with full video support, and decoding of bitmaps.

# Compatibility
The game engine is compatible with the following platforms:
* Microsoft DOS
* FreeDOS
* Apple II (Partialy, no mouse functions and partialy no video functions)
* Apple II+ (Partialy, no mouse functions and partialy no video functions)
* Windows (Partialy, no video and mouse functions)
* Unix (Partialy, no video and mouse functions)
* Linux (Partialy, no video and mouse functions)
While Windows, Unix and Linux supports only the file operation functions. Microsoft DOS and FreeDOS supports all functions.

# Code style
* Free all allocated memory, even if its NULL, no one gets hurt for NULL de-allocation
* Functions must not return pointers
* "#elseif" or "#else if" will never be used, due to portability issues.
* Aritmetical operations that dosent need 64-bit integers should not be divided into "#ifdef [platform]" macros
* Floats should be avoided as must as posible
* Static variables should be used in functions
* Only one "mouse" struct per program. There is no need for more than one
* If your target OS dosent supports multiple files, include first the header (key.h), (leaf.h) then (leaf.c), (image.c) in that order, remember
to remove the "#include" macros