/*
 * Copyright 2016 Bruno Ribeiro
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "util.h"
#include <stdlib.h>

static const uint32_t AMASK = 0xFF000000;
static const uint32_t YMASK = 0x00FF0000;
static const uint32_t UMASK = 0x0000FF00;
static const uint32_t VMASK = 0x000000FF;

uint32_t ARGBtoAYUV( uint32_t value )
{
    uint32_t A, R, G, B, Y, U, V;

    A = value >> 24;
    R = (value >> 16) & 0xFF;
    G = (value >> 8) & 0xFF;
    B = value & 0xFF;

    Y = (uint32_t) ( 0.299 * R + 0.587 * G + 0.114 * B);
    U = (uint32_t) (-0.169 * R - 0.331 * G +   0.5 * B) + 128;
    V = (uint32_t) (   0.5 * R - 0.419 * G - 0.081 * B) + 128;
    return (A << 24) + (Y << 16) + (U << 8) + V;
}

#if 1
int is_different(
	uint32_t color1,
	uint32_t color2,
	uint32_t trY,
	uint32_t trU,
	uint32_t trV,
	uint32_t trA )
{
	uint32_t yuv1 = ARGBtoAYUV(color1);
	uint32_t yuv2 = ARGBtoAYUV(color2);

	return abs((int)(yuv1 & YMASK) - (int)(yuv2 & YMASK)) > trY ||
		   abs((int)(yuv1 & UMASK) - (int)(yuv2 & UMASK)) > trU ||
		   abs((int)(yuv1 & VMASK) - (int)(yuv2 & VMASK)) > trV ||
		   abs((int)(yuv1 & AMASK) - (int)(yuv2 & AMASK)) > trA;
}
#else
/*
 * Use this function for smoothed images (good for complex graphics)
 */
int is_different(
	uint32_t color1,
	uint32_t color2,
	uint32_t trY,
	uint32_t trU,
	uint32_t trV,
	uint32_t trA )
{
	color1 = ARGBtoAYUV(color1);
	color2 = ARGBtoAYUV(color2);

	return
		abs( (int) ((color1 >> 16) & 0xFF) - (int) ((color2 >> 16) & 0xFF) ) > trY ||
		abs( (int) ((color1 >> 8) & 0xFF) - (int) ((color2 >> 8) & 0xFF) ) > trU ||
		abs( (int) (color1 & 0xFF) - (int) (color2 & 0xFF) ) > trV ||
		abs( (int) (color1 >> 24) - (int) (color2 >> 24) ) > trA;
}
#endif
