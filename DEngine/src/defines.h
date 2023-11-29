#include <stdint.h>

#include "destring.h"

#define INLINE __forceinline
#define internal static
#define global static

#define EXIT_IF_NULL(x) if(!x){ return; }

#define KB(x) x * 1024
#define MB(x) KB(x) * 1024
#define GB(x) MB(x) * 1024

typedef unsigned char Byte;

typedef char    int8;
typedef int16_t int16;
typedef int     int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;

#define BIT(X) 1 << X

#define FOR_RANGE(var,range) for(int var = 0; var < range; var++)
#define FOR_RANGE2(var, min_range, max_range) for(int var = min_range; var < max_range; var++)
