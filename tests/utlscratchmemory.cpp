#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/utlscratchmemory.h>

REGISTER_NAMED_TEST( "CUtlScratchMemoryPool.AllocFree", CUtlScratchMemoryPool_AllocFree )
{
	// Scratch memory pools should allocate aligned blocks and rewind to saved marks.
	CUtlScratchMemoryPoolFixedGrowable< 256 > pool;

	void *pFirst = pool.AllocAligned( 32, 16 );
	void *pSecond = pool.AllocAligned( 48, 16 );

	TEST_NOT_NULL( pFirst );
	TEST_NOT_NULL( pSecond );
	TEST_TRUE( pool.AllocSize() >= 256 );
	TEST_TRUE( pool.AllocationCount() >= 1 );

	UtlScratchMemoryPoolMark_t mark = pool.GetCurrentAllocPoint();
	void *pThird = pool.AllocAligned( 64, 16 );

	TEST_NOT_NULL( pThird );
	pool.FreeToAllocPoint( mark );
}
