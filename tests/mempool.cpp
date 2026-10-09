#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/mempool.h>

struct MemoryPoolTracked_t
{
	static inline int s_nDestructed = 0;

	~MemoryPoolTracked_t() { ++s_nDestructed; }

	int m_nValue = 0;
};

REGISTER_NAMED_TEST( "CUtlMemoryPool.AllocFreeClear", CUtlMemoryPool_AllocFreeClear )
{
	// Typed memory pools should construct on allocation and destruct on free and clear.
	MemoryPoolTracked_t::s_nDestructed = 0;

	CUtlMemoryPool< MemoryPoolTracked_t > pool( 4 );

	MemoryPoolTracked_t *pFirst = pool.Alloc();
	MemoryPoolTracked_t *pSecond = pool.Alloc();

	TEST_NOT_NULL( pFirst );
	TEST_NOT_NULL( pSecond );
	TEST_EQ( pool.Count(), 2 );

	pool.Free( pFirst );
	TEST_EQ( MemoryPoolTracked_t::s_nDestructed, 1 );
	TEST_EQ( pool.Count(), 1 );

	pool.Clear();
	TEST_EQ( pool.Count(), 0 );
}
