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

REGISTER_NAMED_TEST( "mathlib.MatrixScaleAndTranslation", mathlib_MatrixScaleAndTranslation )
{
	// Scaling touches only the 3x3 part; translation lives in the fourth column.
	MathLib_Init();

	matrix3x4_t mat;
	SetIdentityMatrix( mat );
	mat.SetTranslation( Vector( 1.0f, 2.0f, 3.0f ) );
	TEST_TRUE( mat.GetTranslation() == Vector( 1.0f, 2.0f, 3.0f ) );

	Vector vTranslation;
	MatrixGetTranslation( mat, vTranslation );
	TEST_TRUE( vTranslation == Vector( 1.0f, 2.0f, 3.0f ) );
	MatrixSetTranslation( Vector( 4.0f, 5.0f, 6.0f ), mat );
	TEST_TRUE( mat.GetTranslation() == Vector( 4.0f, 5.0f, 6.0f ) );

	matrix3x4_t scaled = mat * 2.0f;
	TEST_EQ( scaled[ 0 ][ 0 ], 2.0f );
	TEST_EQ( scaled[ 1 ][ 1 ], 2.0f );
	TEST_EQ( scaled[ 2 ][ 2 ], 2.0f );
	TEST_EQ( scaled[ 0 ][ 3 ], 4.0f );
	TEST_EQ( mat[ 0 ][ 0 ], 1.0f );

	mat *= 3.0f;
	TEST_EQ( mat[ 1 ][ 1 ], 3.0f );
	TEST_EQ( mat[ 1 ][ 3 ], 5.0f );

	mat.ScaleBy( 0.5f );
	TEST_EQ( mat[ 2 ][ 2 ], 1.5f );

	matrix3x4_t translate;
	SetIdentityMatrix( translate );
	translate.SetTranslation( Vector( 1.0f, 0.0f, 0.0f ) );
	matrix3x4_t combined;
	SetIdentityMatrix( combined );
	combined *= translate;
	combined *= translate;
	TEST_TRUE( combined.GetTranslation() == Vector( 2.0f, 0.0f, 0.0f ) );

	mat.ScaleByZero();
	TEST_EQ( mat[ 0 ][ 0 ], 0.0f );
	TEST_EQ( mat[ 2 ][ 2 ], 0.0f );
	TEST_EQ( mat[ 2 ][ 3 ], 6.0f );
}

REGISTER_NAMED_TEST( "mathlib.FloatBits", mathlib_FloatBits )
{
	// Bit casts should be exactly 32 bits wide in both directions.
	const float flOne = 1.0f;
	float flValue = -2.0f;

	TEST_EQ( FloatBits( flOne ), 0x3F800000u );
	TEST_EQ( BitsToFloat( 0x3F800000u ), 1.0f );
	TEST_EQ( FloatBits( flValue ), 0xC0000000u );
	FloatBits( flValue ) = 0x40400000u;
	TEST_EQ( flValue, 3.0f );
	TEST_TRUE( IsFinite( flOne ) );
	TEST_FALSE( IsFinite( BitsToFloat( 0x7F800000u ) ) );

	TEST_TRUE( Vector( 0.0f, 0.0f, 0.0f ).IsZeroFast() );
	TEST_FALSE( Vector( 0.0f, -0.0f, 0.0f ).IsZeroFast() );
}

REGISTER_NAMED_TEST( "mathlib.FloatHelpers", mathlib_FloatHelpers )
{
	// Single-precision helpers should keep their exact endpoint values.
	TEST_EQ( QuinticInterpolatingPolynomial( 0.0f ), 0.0f );
	TEST_EQ( QuinticInterpolatingPolynomial( 1.0f ), 1.0f );
	TEST_EQ( QuinticInterpolatingPolynomial( 0.5f ), 0.5f );
	TEST_EQ( InvRSquared( Vector( 0.5f, 0.0f, 0.0f ) ), 1.0f );
	TEST_EQ( InvRSquared( Vector( 2.0f, 0.0f, 0.0f ) ), 0.25f );
}
