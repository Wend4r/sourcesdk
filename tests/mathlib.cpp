#include "common/assert.h"
#include "common/macros.h"

#include <mathlib/mathlib.h>
#include <mathlib/spherical_geometry.h>

#include <float.h>
#include <math.h>

REGISTER_NAMED_TEST( "mathlib.FastPow", mathlib_FastPow )
{
	// The fast approximations should stay close to the exact results for representative inputs.
	TEST_TRUE( fabsf( FastLog2( 8.0f ) - 3.0f ) < 0.01f );
	TEST_TRUE( fabsf( FastLog2( 0.5f ) + 1.0f ) < 0.01f );
	TEST_TRUE( fabsf( FastPow2( 3.0f ) - 8.0f ) < 0.05f );
	TEST_TRUE( fabsf( FastPow( 3.0f, 2.0f ) - 9.0f ) < 0.2f );
	TEST_TRUE( fabsf( FastPow10( 2.0f ) - 100.0f ) < 2.0f );
	TEST_EQ( FastPow( 0.0f, 2.0f ), 0.0f );
}

REGISTER_NAMED_TEST( "mathlib.AlmostEqual", mathlib_AlmostEqual )
{
	// Comparisons should be measured in ULPs and handle signed zero, infinities and NaNs.
	const float flOne = 1.0f;
	const float flNext = nextafterf( flOne, 2.0f );
	const float flFar = 1.001f;

	TEST_TRUE( AlmostEqual( flOne, flOne ) );
	TEST_TRUE( AlmostEqual( flOne, flNext ) );
	TEST_FALSE( AlmostEqual( flOne, flFar ) );
	TEST_TRUE( AlmostEqual( 0.0f, -0.0f ) );
	TEST_FALSE( AlmostEqual( flOne, -flOne ) );
	TEST_TRUE( AlmostEqual( INFINITY, INFINITY ) );
	TEST_FALSE( AlmostEqual( FLT_MAX, INFINITY ) );
	TEST_FALSE( AlmostEqual( NAN, NAN ) );
	TEST_TRUE( AlmostEqual( Vector( 1.0f, 2.0f, 3.0f ), Vector( 1.0f, 2.0f, 3.0f ) ) );
}

REGISTER_NAMED_TEST( "mathlib.SphericalHarmonic", mathlib_SphericalHarmonic )
{
	// Band 0 is the constant 1 / ( 2 * sqrt( pi ) ) and P( 1, 0, x ) is x.
	const float flY00 = 0.5f / sqrtf( M_PI );

	TEST_TRUE( fabsf( AssociatedLegendrePolynomial( 0, 0, 0.3f ) - 1.0f ) < 1.0e-6f );
	TEST_TRUE( fabsf( AssociatedLegendrePolynomial( 1, 0, 0.3f ) - 0.3f ) < 1.0e-6f );
	TEST_TRUE( fabsf( AssociatedLegendrePolynomial( 2, 0, 0.5f ) + 0.125f ) < 1.0e-6f );
	TEST_TRUE( fabsf( SphericalHarmonic( 0, 0, 0.7f, 1.3f ) - flY00 ) < 1.0e-5f );
	TEST_TRUE( fabsf( SphericalHarmonic( 0, 0, Vector( 0.0f, 0.0f, 1.0f ) ) - flY00 ) < 1.0e-5f );
}

// Declared by the private mathlib/sse.h.
float _SSE_RSqrtFast( float x );

REGISTER_NAMED_TEST( "mathlib.SSERSqrtFast", mathlib_SSERSqrtFast )
{
	// The estimate is returned, not written back over the argument.
	MathLib_Init();

	TEST_TRUE( fabsf( _SSE_RSqrtFast( 4.0f ) - 0.5f ) < 0.001f );
	TEST_TRUE( fabsf( _SSE_RSqrtFast( 0.25f ) - 2.0f ) < 0.005f );
}

REGISTER_NAMED_TEST( "mathlib.ConcatRotations", mathlib_ConcatRotations )
{
	// Rotating 90 degrees about Z twice should give a 180 degree rotation about Z.
	MathLib_Init();

	matrix3x4_t rot90;
	matrix3x4_t rot180;
	AngleMatrix( QAngle( 0.0f, 90.0f, 0.0f ), rot90 );
	SetIdentityMatrix( rot180 );
	ConcatRotations( rot90, rot90, rot180 );

	TEST_TRUE( fabsf( rot180[ 0 ][ 0 ] + 1.0f ) < 1.0e-5f );
	TEST_TRUE( fabsf( rot180[ 1 ][ 1 ] + 1.0f ) < 1.0e-5f );
	TEST_TRUE( fabsf( rot180[ 2 ][ 2 ] - 1.0f ) < 1.0e-5f );
	TEST_TRUE( fabsf( rot180[ 0 ][ 1 ] ) < 1.0e-5f );
	TEST_TRUE( fabsf( rot180[ 1 ][ 0 ] ) < 1.0e-5f );
}
