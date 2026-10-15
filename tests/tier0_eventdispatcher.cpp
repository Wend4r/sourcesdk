#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/eventdispatcher.h>

#include <cstddef>
#include <cstring>

// Exposes the protected listener operations of the dispatcher base for the tests.
class CTestEventDispatcher : public CEventDispatcher_Base
{
public:
	template < typename DELEGATE >
	void Register( const DELEGATE &delegate, int nPriority, const char *pszName )
	{
		RegisterEventListener_Base( delegate.GetAbstractDelegate(), 1, true, m_Listeners, nPriority, pszName );
	}

	template < typename DELEGATE >
	bool Unregister( const DELEGATE &delegate )
	{
		return UnregisterEventListener_Base( delegate.GetAbstractDelegate(), m_Listeners );
	}

	void GetNames( CUtlVector< CUtlString > *pNames ) { GetListenerNames_Base( m_Listeners, pNames ); }

	using CEventDispatcher_Base::DelegateIterator_Base_t;
	using CEventDispatcher_Base::IterateListeners_Start;
	using CEventDispatcher_Base::IterateListener;
	using CEventDispatcher_Base::IterateListeners_End;

	CUtlVector< EventListenerInfo_t > m_Listeners;
};

static void ListenerA( int ) {}
static void ListenerB( int ) {}
static void ListenerC( int ) {}

static CUtlDelegate< void ( int ) > s_DelegateA( &ListenerA );
static CUtlDelegate< void ( int ) > s_DelegateB( &ListenerB );
static CUtlDelegate< void ( int ) > s_DelegateC( &ListenerC );

// Joins the names of the listeners in the order the iterator returns them.
static void CollectNames( CTestEventDispatcher &dispatcher, bool bForward, CUtlString *pOut )
{
	CTestEventDispatcher::DelegateIterator_Base_t iter;
	iter.pListeners = &dispatcher.m_Listeners;

	if ( dispatcher.IterateListeners_Start( iter, bForward ) )
	{
		while ( const CEventDispatcher_Base::EventListenerInfo_t *pInfo = dispatcher.IterateListener( iter ) )
		{
			pOut->Append( pInfo->m_pszName );
		}
	}

	dispatcher.IterateListeners_End( iter );
}

REGISTER_NAMED_TEST( "EventDispatcher.LayoutMatchesTier0", EventDispatcher_LayoutMatchesTier0 )
{
	TEST_EQ( sizeof( CEventDispatcher_Base::EventListenerInfo_t ), static_cast< size_t >( 40 ) );
	TEST_EQ( offsetof( CEventDispatcher_Base::DelegateIterator_Base_t, nCurrent ), static_cast< size_t >( 48 ) );
	TEST_EQ( offsetof( CEventDispatcher_Base::DelegateIterator_Base_t, pNext ), static_cast< size_t >( 56 ) );
	TEST_EQ( offsetof( CEventDispatcher_Base::DelegateIterator_Base_t, bIteratingForward ), static_cast< size_t >( 64 ) );
}

REGISTER_NAMED_TEST( "EventDispatcher.ListenersAreSortedByPriority", EventDispatcher_ListenersAreSortedByPriority )
{
	CTestEventDispatcher dispatcher;

	dispatcher.Register( s_DelegateA, 0, "A" );
	dispatcher.Register( s_DelegateB, 10, "B" );
	dispatcher.Register( s_DelegateC, 0, "C" );

	// Higher priorities come first, equal ones keep the order they were added in.
	CUtlString order;
	CollectNames( dispatcher, true, &order );
	TEST_EQ( strcmp( order.Get(), "BAC" ), 0 );

	CUtlString reverse;
	CollectNames( dispatcher, false, &reverse );
	TEST_EQ( strcmp( reverse.Get(), "CAB" ), 0 );

	const CEventDispatcher_Base::EventListenerInfo_t &first = dispatcher.m_Listeners[ 0 ];
	TEST_EQ( first.m_nPriority, 10 );
	TEST_EQ( first.m_nDelegateParamCount, 1 );
	TEST_TRUE( first.m_bDelegateReturnsVoid );

	CUtlVector< CUtlString > names;
	dispatcher.GetNames( &names );
	TEST_EQ( names.Count(), 3 );
	TEST_EQ( strcmp( names[ 0 ].Get(), "B" ), 0 );
}

REGISTER_NAMED_TEST( "EventDispatcher.UnregisterRemovesTheDelegate", EventDispatcher_UnregisterRemovesTheDelegate )
{
	CTestEventDispatcher dispatcher;

	dispatcher.Register( s_DelegateA, 0, "A" );
	dispatcher.Register( s_DelegateB, 0, "B" );

	TEST_TRUE( dispatcher.Unregister( s_DelegateA ) );
	TEST_EQ( dispatcher.m_Listeners.Count(), 1 );

	// A delegate that is not registered is not found.
	TEST_FALSE( dispatcher.Unregister( s_DelegateA ) );
	TEST_FALSE( dispatcher.Unregister( s_DelegateC ) );
}

REGISTER_NAMED_TEST( "EventDispatcher.IterationSurvivesChangesToTheListeners", EventDispatcher_IterationSurvivesChangesToTheListeners )
{
	CTestEventDispatcher dispatcher;

	dispatcher.Register( s_DelegateA, 20, "A" );
	dispatcher.Register( s_DelegateB, 10, "B" );

	CTestEventDispatcher::DelegateIterator_Base_t iter;
	iter.pListeners = &dispatcher.m_Listeners;
	TEST_TRUE( dispatcher.IterateListeners_Start( iter, true ) );

	CUtlString order;
	order.Append( dispatcher.IterateListener( iter )->m_pszName );

	// A listener added in front of the current position is not visited by this pass,
	// and removing a visited one does not make the iterator skip the next.
	dispatcher.Register( s_DelegateC, 30, "C" );
	TEST_TRUE( dispatcher.Unregister( s_DelegateA ) );

	while ( const CEventDispatcher_Base::EventListenerInfo_t *pInfo = dispatcher.IterateListener( iter ) )
	{
		order.Append( pInfo->m_pszName );
	}

	dispatcher.IterateListeners_End( iter );

	TEST_EQ( strcmp( order.Get(), "AB" ), 0 );
}
