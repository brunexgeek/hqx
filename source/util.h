#ifndef HQX_UTIL_H
#define HQX_UTIL_H

#include <stdint.h>

uint32_t ARGBtoAYUV( uint32_t value );
int is_different( uint32_t color1, uint32_t color2, uint32_t trY, uint32_t trU,
        uint32_t trV, uint32_t trA );

#endif // HQX_UTIL_H