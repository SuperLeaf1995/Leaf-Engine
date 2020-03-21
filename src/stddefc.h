#ifndef STDDEFC_H
#define STDDEFC_H

#if defined(__MSDOS__) || defined(__DOS__) || defined(FREEDOS)
#if defined(__BORLANDC__)
#include <stdint.h> /*Borland C++ has STDINT.H*/
#endif
#if defined(__TURBOC__) && !defined(__BORLANDC__)
typedef unsigned char uint8_t; /*Turbo C lacks STDINT.H*/
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;
#define INT8_MAX 0x7F
#define INT8_MIN -128
#define UINT8_MAX 0xFFU
#define INT16_MAX 0x7FFF
#define INT16_MIN ((int)0x8000)
#define UINT16_MAX 0xFFFFU
#define INT32_MAX 0x7FFF
#define INT32_MIN ((int)0x8000)
#define UINT32_MAX 0xFFFFU
#define INT64_MAX 0x7FFFFFFFL
#define INT64_MIN ((long)0x80000000L)
#define UINT64_MAX 0xFFFFFFFFUL
#endif
#endif
#if defined(__linux) || defined(linux)
#include <stdint.h>
#endif

#endif
