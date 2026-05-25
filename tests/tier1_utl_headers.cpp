#include "common/assert.h"
#include "common/macros.h"

#include <tier1/UtlSortVector.h>
#include <tier1/utlallocation.h>
#include <tier1/utlarray.h>
#include <tier1/utlbidirectionalset.h>
#include <tier1/utlblockmemory.h>
#include <tier1/utlbufferstrider.h>
#include <tier1/utlbufferutil.h>
#include <tier1/utlcommon.h>
#include <tier1/utldelegate.h>
#include <tier1/utldelegateimpl.h>
#include <tier1/utldict.h>
#include <tier1/utlencode.h>
#include <tier1/utlenvelope.h>
#include <tier1/utlfixedmemory.h>
#include <tier1/utlflags.h>
#include <tier1/utlhandletable.h>
#include <tier1/utlhash.h>
#include <tier1/utlhashdict.h>
#include <tier1/utlhashmaplarge.h>
#include <tier1/utlhashtable.h>
#include <tier1/utlincrementalvector.h>
#include <tier1/utlintrusivelist.h>
#include <tier1/utlintervaltree.h>
#include <tier1/utlleanvector.h>
#include <tier1/utllinkedlist.h>
#include <tier1/utlmap.h>
#include <tier1/utlmultilist.h>
#include <tier1/utlntree.h>
#include <tier1/utlobjectattributetable.h>
#include <tier1/utlobjectreference.h>
#include <tier1/utlpair.h>
#include <tier1/utlpriorityqueue.h>
#include <tier1/utlqueue.h>
#include <tier1/utlrbtree.h>
#include <tier1/utlsoacontainer.h>
#include <tier1/utlstack.h>
#include <tier1/utlstringbuilder.h>
#include <tier1/utlstringmap.h>
#include <tier1/utlsymbollarge.h>
#include <tier1/utltscache.h>
#include <tier1/utltshash.h>
#include <tier1/utlvector.h>
#include <tier1/utlvectormemory.h>

REGISTER_NAMED_TEST( "Smoke.Tier1UtlHeaders.Include", Smoke_Tier1UtlHeaders_Include )
{
	// Tier1 utility headers should compile together and allow basic container use.
	CUtlVector< int > vec;

	vec.AddToTail( 1 );

	CUtlMap< int, int > map;

	map.Insert( 1, 2 );

	TEST_EQ( vec.Count(), 1 );
	TEST_EQ( map.Count(), 1u );
}
