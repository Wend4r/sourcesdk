#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>

#include <memory>
#include <utility>

struct ConstructMoveOnly_t
{
	ConstructMoveOnly_t( std::unique_ptr< int > pValue ) : m_pValue( std::move( pValue ) ) {}

	std::unique_ptr< int > m_pValue;
};

struct ConstructLvalueRef_t
{
	ConstructLvalueRef_t( int &nOut ) { nOut = 42; }
};

struct ConstructSixArgs_t
{
	ConstructSixArgs_t( int a, int b, int c, int d, int e, int f ) : m_nSum( a + b + c + d + e + f ) {}

	int m_nSum;
};

REGISTER_NAMED_TEST( "Construct.ForwardsMoveOnlyArguments", Construct_ForwardsMoveOnlyArguments )
{
	// Move-only arguments must reach the constructor without being copied.
	alignas( ConstructMoveOnly_t ) unsigned char storage[ sizeof( ConstructMoveOnly_t ) ];

	std::unique_ptr< int > pValue( new int( 7 ) );
	ConstructMoveOnly_t *pObject = Construct( reinterpret_cast< ConstructMoveOnly_t * >( storage ), std::move( pValue ) );

	TEST_NULL( pValue.get() );
	TEST_NOT_NULL( pObject->m_pValue.get() );
	TEST_EQ( *pObject->m_pValue, 7 );

	pObject->~ConstructMoveOnly_t();
}

REGISTER_NAMED_TEST( "Construct.ForwardsLvalueReferences", Construct_ForwardsLvalueReferences )
{
	// An lvalue argument must bind to a reference parameter, not to a copy.
	alignas( ConstructLvalueRef_t ) unsigned char storage[ sizeof( ConstructLvalueRef_t ) ];

	int nOut = 0;
	Construct( reinterpret_cast< ConstructLvalueRef_t * >( storage ), nOut );

	TEST_EQ( nOut, 42 );
}

REGISTER_NAMED_TEST( "Construct.MoreThanFiveArguments", Construct_MoreThanFiveArguments )
{
	// The argument count is not limited.
	alignas( ConstructSixArgs_t ) unsigned char storage[ sizeof( ConstructSixArgs_t ) ];

	ConstructSixArgs_t *pObject = Construct( reinterpret_cast< ConstructSixArgs_t * >( storage ), 1, 2, 3, 4, 5, 6 );

	TEST_EQ( pObject->m_nSum, 21 );
}
