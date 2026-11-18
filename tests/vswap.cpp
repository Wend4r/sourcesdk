#include "common/assert.h"
#include "common/macros.h"

#include <mathlib/mathlib.h>

#include <memory>

struct SwapTracked_t
{
	static inline int s_nCopies = 0;

	int m_nValue;

	SwapTracked_t( int nValue ) : m_nValue( nValue ) {}
	SwapTracked_t( const SwapTracked_t &other ) : m_nValue( other.m_nValue ) { s_nCopies++; }
	SwapTracked_t( SwapTracked_t &&other ) : m_nValue( other.m_nValue ) {}
	SwapTracked_t &operator=( const SwapTracked_t &other ) { m_nValue = other.m_nValue; s_nCopies++; return *this; }
	SwapTracked_t &operator=( SwapTracked_t &&other ) { m_nValue = other.m_nValue; return *this; }
};

REGISTER_NAMED_TEST( "V_swap.MovesValues", V_swap_MovesValues )
{
	// V_swap should move the values rather than copy them.
	SwapTracked_t a( 1 ), b( 2 );

	SwapTracked_t::s_nCopies = 0;
	V_swap( a, b );
	TEST_EQ( a.m_nValue, 2 );
	TEST_EQ( b.m_nValue, 1 );
	TEST_EQ( SwapTracked_t::s_nCopies, 0 );

	// Move-only types can be swapped too.
	std::unique_ptr< int > pFirst( new int( 3 ) ), pSecond( new int( 4 ) );

	V_swap( pFirst, pSecond );
	TEST_EQ( *pFirst, 4 );
	TEST_EQ( *pSecond, 3 );
}
