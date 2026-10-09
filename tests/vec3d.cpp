#include "common/assert.h"
#include "common/macros.h"

#include <mathlib/vec3d.h>

REGISTER_NAMED_TEST( "Vec3D.Arithmetic", Vec3D_Arithmetic )
{
	// Component-wise arithmetic should match the scalar math for every component.
	Vec3D< int > a( 1, 2, 3 );
	Vec3D< int > b( 4, 5, 6 );

	TEST_TRUE( a + b == Vec3D< int >( 5, 7, 9 ) );
	TEST_TRUE( b - a == Vec3D< int >( 3, 3, 3 ) );
	TEST_TRUE( a * 2 == Vec3D< int >( 2, 4, 6 ) );
	TEST_TRUE( b / 2 == Vec3D< int >( 2, 2, 3 ) );
	TEST_TRUE( -a == Vec3D< int >( -1, -2, -3 ) );
	TEST_TRUE( a != b );

	Vec3D< int > c = a;
	c += b;
	c -= a;
	TEST_TRUE( c == b );
	c *= 3;
	c /= 3;
	TEST_TRUE( c == b );
	c.Negate();
	TEST_TRUE( c == -b );

	TEST_EQ( a[ 0 ], 1 );
	TEST_EQ( a[ 2 ], 3 );
	a[ 1 ] = 7;
	TEST_EQ( a.y, 7 );
	TEST_EQ( a.Base(), &a.x );
}

REGISTER_NAMED_TEST( "Vec3D.InitAndMagnitude", Vec3D_InitAndMagnitude )
{
	// Init and Zero should reset components; Dot and Length should use all three axes.
	Vec3D< double > v;
	v.Init( 2.0, 3.0, 6.0 );

	TEST_EQ( v.LengthSqr(), 49.0 );
	TEST_EQ( v.Length(), 7.0 );
	TEST_EQ( v.Dot( Vec3D< double >( 1.0, 1.0, 1.0 ) ), 11.0 );

	v.Zero();
	TEST_TRUE( v == Vec3D< double >( 0.0, 0.0, 0.0 ) );

	Vec3D< float > f;
	f.Init();
	TEST_EQ( f.LengthSqr(), 0.0f );
}
