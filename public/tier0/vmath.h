#ifndef VMATH_H
#define VMATH_H

#pragma once

/*
 * Math Extension Sublibrary (V_Math)
 *
 * This library provides a collection of extended mathematical functions
 * offering enhanced portability, degree-based trigonometric functions,
 * fast approximations, and utility functions commonly used in scientific
 * and engineering applications.
 *
 * It includes:
 *   - Trigonometric functions with degree input/output (e.g., V_sind, V_cosd)
 *   - Hyperbolic and exponential functions
 *   - Logarithmic and power functions
 *   - Rounding, modulus, and decomposition operations
 *   - IEEE-compatible remainder and classification utilities
 *   - Extended compatibility layer for platform-specific math handling
 *
 * The implementation optionally utilizes AVX and AVX2 SIMD instruction sets,
 * where available, to improve performance on supported CPUs.
 *
 * Internally, the library also leverages standard C runtime math libraries
 * (libc/libm or CRT) to ensure accuracy and compatibility with established
 * mathematical conventions.
 *
 * This library complements standard C math facilities and can be used
 * in performance-critical applications or when degree-based operations
 * are required.
 */

#include "basetypes.h"
#include "platform.h"

/// Trigonometric functions (degrees and radians).
PLATFORM_INTERFACE double V_acosd( double x );       // Returns the arc cosine of x in degrees.
PLATFORM_INTERFACE float  V_acosf( float x );        // Returns the arc cosine of x in radians.

PLATFORM_INTERFACE double V_asind( double x );       // Returns the arc sine of x in degrees.
PLATFORM_INTERFACE float  V_asinf( float x );        // Returns the arc sine of x in radians.

PLATFORM_INTERFACE double V_atand( double x );       // Returns the arc tangent of x in degrees.
PLATFORM_INTERFACE float  V_atanf( float x );        // Returns the arc tangent of x in radians.

PLATFORM_INTERFACE double V_atan2d( double y, double x ); // Returns the arc tangent of y/x in degrees, considering quadrant.
PLATFORM_INTERFACE float  V_atan2f( float y, float x );   // Returns the arc tangent of y/x in radians, considering quadrant.

PLATFORM_INTERFACE double V_cosd( double x );        // Returns the cosine of angle x in degrees.
PLATFORM_INTERFACE float  V_cosf( float x );         // Returns the cosine of x in radians.

PLATFORM_INTERFACE double V_sind( double x );        // Returns the sine of angle x in degrees.
PLATFORM_INTERFACE float  V_sinf( float x );         // Returns the sine of x in radians.

PLATFORM_INTERFACE void   V_sincosd( double x, double *s, double *c ); // Computes sine and cosine of angle x in degrees.
PLATFORM_INTERFACE void   V_sincosf( float x, float *s, float *c );    // Computes sine and cosine of x in radians.

PLATFORM_INTERFACE double V_tand( double x );        // Returns the tangent of angle x in degrees.
PLATFORM_INTERFACE float  V_tanf( float x );         // Returns the tangent of x in radians.

/// Hyperbolic functions.
PLATFORM_INTERFACE double V_coshd( double x );       // Returns the hyperbolic cosine of x.
PLATFORM_INTERFACE float  V_coshf( float x );        // Returns the hyperbolic cosine of x (float).

PLATFORM_INTERFACE double V_sinhd( double x );       // Returns the hyperbolic sine of x.
PLATFORM_INTERFACE float  V_sinhf( float x );        // Returns the hyperbolic sine of x (float).

PLATFORM_INTERFACE double V_tanhd( double x );       // Returns the hyperbolic tangent of x.
PLATFORM_INTERFACE float  V_tanhf( float x );        // Returns the hyperbolic tangent of x (float).

/// Exponential functions.
PLATFORM_INTERFACE double V_expd( double x );        // Returns the exponential value of e^x.
PLATFORM_INTERFACE float  V_expf( float x );         // Returns the exponential value of e^x (float).

PLATFORM_INTERFACE double V_exp2d( double x );       // Returns 2 raised to the power x.
PLATFORM_INTERFACE float  V_exp2f( float x );        // Returns 2 raised to the power x (float).

/// Logarithmic functions.
PLATFORM_INTERFACE double V_logd( double x );        // Returns the natural logarithm (base e) of x.
PLATFORM_INTERFACE float  V_logf( float x );         // Returns the natural logarithm of x (float).

PLATFORM_INTERFACE double V_log2d( double x );       // Returns the base-2 logarithm of x.
PLATFORM_INTERFACE float  V_log2f( float x );        // Returns the base-2 logarithm of x (float).

PLATFORM_INTERFACE uint V_log2_uint( uint x ); // Returns floor(log2(x)) for integer values.

PLATFORM_INTERFACE double V_log10d( double x );      // Returns the base-10 logarithm of x.
PLATFORM_INTERFACE float  V_log10f( float x );       // Returns the base-10 logarithm of x (float).

/// Power functions.
PLATFORM_INTERFACE double V_powd( double base, double exp ); // Returns base raised to the power exp.
PLATFORM_INTERFACE float  V_powf( float base, float exp );   // Returns base raised to the power exp (float).

/// Square root and hypotenuse.
PLATFORM_INTERFACE double V_tier0_sqrtd( double x ); // Returns the square root of x.
PLATFORM_INTERFACE float  V_tier0_sqrtf( float x );  // Returns the square root of x (float).
#define V_sqrtd V_tier0_sqrtd
#define V_sqrtf V_tier0_sqrtf

PLATFORM_INTERFACE double V_hypotd( double x, double y ); // Returns sqrt(x^2 + y^2).
PLATFORM_INTERFACE float  V_hypotf( float x, float y );   // Returns sqrt(x^2 + y^2) (float).

/// Rounding and integer conversion functions.
PLATFORM_INTERFACE double V_tier0_ceild( double x );   // Rounds x upward to the nearest integer value.
PLATFORM_INTERFACE float  V_tier0_ceilf( float x );    // Rounds x upward to the nearest integer value (float).
#define V_ceild V_tier0_ceild
#define V_ceilf V_tier0_ceilf

PLATFORM_INTERFACE double V_tier0_floord( double x );  // Rounds x downward to the nearest integer value.
PLATFORM_INTERFACE float  V_tier0_floorf( float x );   // Rounds x downward to the nearest integer value (float).
#define V_floord V_tier0_floord
#define V_floorf V_tier0_floorf

PLATFORM_INTERFACE double V_roundd( double x );        // Rounds x to the nearest integer (halfway cases away from zero).
PLATFORM_INTERFACE float  V_roundf( float x );         // Rounds x to the nearest integer (float).

/// Modulo and remainder functions.
PLATFORM_INTERFACE double V_fmodd( double x, double y );     // Returns the floating-point remainder of x/y (same sign as x).
PLATFORM_INTERFACE float  V_fmodf( float x, float y );       // Returns the floating-point remainder of x/y (float).

PLATFORM_INTERFACE double V_remainderd( double x, double y ); // Returns IEEE 754-style remainder of x/y.
PLATFORM_INTERFACE float  V_remainderf( float x, float y );   // Returns IEEE 754-style remainder of x/y (float).

PLATFORM_INTERFACE double V_modfd( double x, double *iptr ); // Splits x into integer and fractional parts.
PLATFORM_INTERFACE float  V_modff( float x, float *iptr );   // Splits x into integer and fractional parts (float).

/// Decomposition and scaling functions.
PLATFORM_INTERFACE double V_frexpd( double x, int *exp ); // Decomposes x into mantissa and exponent: x = mantissa * 2^exp.
PLATFORM_INTERFACE float  V_frexpf( float x, int *exp );  // Same as above for float.

PLATFORM_INTERFACE double V_ldexpd( double x, int exp );  // Returns x * 2^exp.
PLATFORM_INTERFACE float  V_ldexpf( float x, int exp );   // Returns x * 2^exp (float).

/// Absolute value.
PLATFORM_INTERFACE double V_tier0_fabsd( double x );      // Returns the absolute value of x.
PLATFORM_INTERFACE float  V_tier0_fabsf( float x );       // Returns the absolute value of x (float).
#define V_fabsd V_tier0_fabsd
#define V_fabsf V_tier0_fabsf

/// Classification and special number checks.
PLATFORM_INTERFACE int V_fpclassifyd( double x );         // Classifies the value of x (zero, subnormal, normal, infinity, NaN).
PLATFORM_INTERFACE int V_isfinited( double x );           // Returns non-zero if x is finite.
PLATFORM_INTERFACE int V_isinfd( double x );              // Returns non-zero if x is infinite.
PLATFORM_INTERFACE int V_isnand( double x );              // Returns non-zero if x is NaN.

#endif // VMATH_H
