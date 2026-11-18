#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/vertexkdtree.h>

REGISTER_NAMED_TEST( "VertexKDTree.FindsVertices", VertexKDTree_FindsVertices )
{
	// A line of vertices at x = 0..9.
	float flVerts[ 10 ][ 3 ];

	for ( int i = 0; i < 10; i++ )
	{
		flVerts[ i ][ 0 ] = ( float )i;
		flVerts[ i ][ 1 ] = 0.0f;
		flVerts[ i ][ 2 ] = 0.0f;
	}

	CVertexKDTree tree;
	tree.BuildMidpoint( &flVerts[ 0 ][ 0 ], 10, 3 );

	Vec3D< float > vCenter( 4.0f, 0.0f, 0.0f );

	CUtlVectorFixedGrowable< const float *, 64 > sphere;
	tree.FindVertsInSphere( sphere, vCenter, 1.5f, 0 );
	TEST_EQ( sphere.Count(), 3 );

	// The shell leaves out the vertices nearer than its inner radius.
	CUtlVectorFixedGrowable< const float *, 64 > shell;
	tree.FindVertsInSphericalShell( shell, vCenter, 1.5f, 2.5f, 0 );
	TEST_EQ( shell.Count(), 2 );

	CUtlVectorFixed< const float *, 64 > box;
	tree.FindVertsInBox( box, Vec3D< float >( 1.5f, -1.0f, -1.0f ), Vec3D< float >( 5.5f, 1.0f, 1.0f ), 0 );
	TEST_EQ( box.Count(), 4 );

	CUtlVectorFixedGrowable< const float *, 64 > nearest;
	tree.Nearest( Vec3D< float >( 6.9f, 0.0f, 0.0f ), nearest );
	TEST_EQ( nearest.Count(), 1 );
	TEST_TRUE( nearest[ 0 ] == &flVerts[ 7 ][ 0 ] );
	TEST_TRUE( tree.Nearest( Vec3D< float >( 6.9f, 0.0f, 0.0f ) ) == &flVerts[ 7 ][ 0 ] );
}
