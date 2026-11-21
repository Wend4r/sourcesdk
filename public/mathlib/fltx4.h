//===== Copyright 1996-2010, Valve Corporation, All rights reserved. ======//
//
// Purpose: - defines the type fltx4 - Avoid cyclic includion.
//
//===========================================================================//

#ifndef FLTX4_H
#define FLTX4_H

#if defined(GNUC)
#define USE_STDC_FOR_SIMD 0
#else
#define USE_STDC_FOR_SIMD 0
#endif

#if (USE_STDC_FOR_SIMD == 0)
#define _SSE1 1
#endif

// I thought about defining a class/union for the SIMD packed floats instead of using fltx4,
// but decided against it because (a) the nature of SIMD code which includes comparisons is to blur
// the relationship between packed floats and packed integer types and (b) not sure that the
// compiler would handle generating good code for the intrinsics.

#if USE_STDC_FOR_SIMD
#error "hello"
typedef union
{
	float  m128_f32[4];
	uint32 m128_u32[4];
} fltx4;

typedef fltx4 i32x4;
typedef fltx4 u32x4;

typedef fltx4 bi32x4;

#else

#include <tmmintrin.h>		// For __m128(i)

typedef __m128 fltx4;
typedef __m128 i32x4;
typedef __m128 u32x4;
typedef __m128i shortx8;
typedef fltx4 bi32x4;

#endif

#endif
