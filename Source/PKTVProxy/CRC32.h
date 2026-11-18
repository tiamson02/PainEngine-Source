
#pragma once

#include "Engine.h"

extern DWORD  GCRCTable[256];

inline DWORD Crc32(const char *string)
{
	if( !string ) return 0;
#define COMPUTE(var, ch) (var) = ((var) << 8) ^ GCRCTable[(((var) >> 24) ^ (ch))&0xff]
    DWORD crc = 0,
          len = 0;

    for(const char *p = string; *p != 0; ++p) {
        COMPUTE (crc, *p);
        ++len;
    }
    // Include the length of the string.
    for(; len != 0; len >>= 8) {
        COMPUTE (crc, len & 0xff);
    }
    return ~crc;
#undef COMPUTE
}

inline DWORD Crc32( const char *mem, int size )
{
#define COMPUTE(var, ch) (var) = ((var) << 8) ^ GCRCTable[(((var) >> 24) ^ (ch))&0xff]
    DWORD crc = 0,
          len = 0;

    for( const char *p = mem; p-mem < size; ++p ) {
        COMPUTE (crc, *p);
        ++len;
    }
    // Include the length of the string.
    for(; len != 0; len >>= 8) {
        COMPUTE (crc, len & 0xff);
    }
    return ~crc;
#undef COMPUTE
}
