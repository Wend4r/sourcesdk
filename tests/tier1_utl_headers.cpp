#include "common/assert.h"
#include "common/macros.h"

#include <tier0/utlsortvector.h>
#include <tier1/utlallocation.h>
#include <tier1/utlarray.h>
#include <tier1/utlbidirectionalset.h>
#include <tier0/utlblockmemory.h>
#include <tier0/utlbufferstrider.h>
#include <tier1/utlbufferutil.h>
#include <tier0/utlcommon.h>
#include <tier1/utldelegate.h>
#include <tier0/utldelegateimpl.h>
#include <tier0/utldict.h>
#include <tier1/utlencode.h>
#include <tier1/utlenvelope.h>
#include <tier0/utlfixedmemory.h>
#include <tier1/utlflags.h>
#include <tier0/utlhandletable.h>
#include <tier0/utlhash.h>
#include <tier0/utlhashdict.h>
#include <tier1/utlhashmaplarge.h>
#include <tier0/utlhashtable.h>
#include <tier0/utlincrementalvector.h>
#include <tier0/utlintrusivelist.h>
#include <tier1/utlintervaltree.h>
#include <tier1/utliterator.h>
#include <tier0/utlleanvector.h>
#include <tier0/utllinkedlist.h>
#include <tier0/utlmap.h>
#include <tier1/utlmultilist.h>
#include <tier1/utlntree.h>
#include <tier1/utlobjectattributetable.h>
#include <tier1/utlobjectreference.h>
#include <tier0/utlpair.h>
#include <tier0/utlpriorityqueue.h>
#include <tier0/utlqueue.h>
#include <tier0/utlrbtree.h>
#include <tier0/utlsoacontainer.h>
#include <tier0/utlstack.h>
#include <tier1/utlstringbuilder.h>
#include <tier0/utlstringmap.h>
#include <tier0/utlsymbollarge.h>
#include <tier1/utltscache.h>
#include <tier0/utltshash.h>
#include <tier0/utlvector.h>
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
