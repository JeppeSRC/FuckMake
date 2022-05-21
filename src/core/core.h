#pragma once

#include <stdio.h>

#ifdef _DEBUG
#define ASSERT(x) if (!(x)) {uint8* _abcd = 0; (*_abcd) = 0; printf("Assertion Failed: %s\n", #x);}
#else
#define ASSERT(x)
#endif

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
typedef unsigned long long uint64;
typedef signed long long int64;