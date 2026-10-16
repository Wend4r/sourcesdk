#include "common/assert.h"
#include "common/macros.h"

#include <tier0/platform.h>
#include <tier0/eventdispatcher.h>

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
	using CEventDispatcher_Base::IterateListeners;
	using CEventDispatcher_Base::m_pActiveIterators;

	CUtlVector< EventListenerInfo_t > m_Listeners;
};

// Exposes the active iterators of the identified dispatcher.
class CTestIdentifiedDispatcher : public CEventDispatcher< CEventIDManager_Default >
{
public:
	using CEventDispatcher_Base::m_pActiveIterators;
};

typedef CTestIdentifiedDispatcher::EventID_t EventID_t;

// A schema class standing for an event message; only the global type scope ones identify events.
struct CTestEventClass
{
	explicit CTestEventClass( bool bGlobalScope = true )
	{
		memset( &m_Info, 0, sizeof( m_Info ) );
		m_Info.m_nFlags1 = bGlobalScope ? SCHEMA_CF1_GLOBAL_TYPE_SCOPE : 0;
	}

	EventID_t ID() { return EventID_t( &m_Info ); }

	CSchemaClassInfo m_Info;
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

REGISTER_NAMED_TEST( "EventDispatcher.ForEachVisitsInPriorityOrder", EventDispatcher_ForEachVisitsInPriorityOrder )
{
	CTestEventDispatcher dispatcher;

	dispatcher.Register( s_DelegateA, 5, "A" );
	dispatcher.Register( s_DelegateB, 10, "B" );
	dispatcher.Register( s_DelegateC, 0, "C" );

	CUtlString order;
	FOR_EACH_EVENT_LISTENER( dispatcher, dispatcher.m_Listeners, pInfo )
	{
		order.Append( pInfo->m_pszName );
	}

	CUtlString reverse;
	FOR_EACH_EVENT_LISTENER_BACK( dispatcher, dispatcher.m_Listeners, pInfo )
	{
		reverse.Append( pInfo->m_pszName );
	}

	TEST_EQ( strcmp( order.Get(), "BAC" ), 0 );
	TEST_EQ( strcmp( reverse.Get(), "CAB" ), 0 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );
}

REGISTER_NAMED_TEST( "EventDispatcher.ForEachEndsTheIterationOnBreak", EventDispatcher_ForEachEndsTheIterationOnBreak )
{
	CTestEventDispatcher dispatcher;

	dispatcher.Register( s_DelegateA, 0, "A" );
	dispatcher.Register( s_DelegateB, 0, "B" );

	int nVisited = 0;
	FOR_EACH_EVENT_LISTENER( dispatcher, dispatcher.m_Listeners, pInfo )
	{
		TEST_TRUE( dispatcher.m_pActiveIterators != nullptr );
		nVisited++;
		break;
	}

	TEST_EQ( nVisited, 1 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );

	// Nothing to visit in an empty list.
	CUtlVector< CEventDispatcher_Base::EventListenerInfo_t > empty;
	FOR_EACH_EVENT_LISTENER( dispatcher, empty, pInfo )
	{
		nVisited++;
	}

	TEST_EQ( nVisited, 1 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );
}

REGISTER_NAMED_TEST( "EventDispatcher.ListenersAreKeptPerEvent", EventDispatcher_ListenersAreKeptPerEvent )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass startEvent( true ), stopEvent( true );

	dispatcher.RegisterEventListener( s_DelegateA, startEvent.ID(), 0, "A" );
	dispatcher.RegisterEventListener( s_DelegateB, startEvent.ID(), 10, "B" );
	dispatcher.RegisterEventListener( s_DelegateC, stopEvent.ID(), 0, "C" );

	CUtlString start;
	FOR_EACH_EVENT_LISTENER( dispatcher, startEvent.ID(), pInfo )
	{
		start.Append( pInfo->m_pszName );
	}

	CUtlString stopBack;
	FOR_EACH_EVENT_LISTENER_BACK( dispatcher, stopEvent.ID(), pInfo )
	{
		stopBack.Append( pInfo->m_pszName );
	}

	TEST_EQ( strcmp( start.Get(), "BA" ), 0 );
	TEST_EQ( strcmp( stopBack.Get(), "C" ), 0 );
	TEST_EQ( dispatcher.GetEventListenerMap().Count(), 2 );

	CUtlVector< CUtlString > names;
	dispatcher.GetListenerNames( startEvent.ID(), &names );
	TEST_EQ( names.Count(), 2 );
	TEST_EQ( strcmp( names[ 0 ].Get(), "B" ), 0 );
}

REGISTER_NAMED_TEST( "EventDispatcher.OnlyGlobalScopeClassesIdentifyEvents", EventDispatcher_OnlyGlobalScopeClassesIdentifyEvents )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass localEvent( false );

	dispatcher.RegisterEventListener( s_DelegateA, localEvent.ID(), 0, "A" );
	dispatcher.RegisterEventListener( s_DelegateA, EventID_t(), 0, "A" );

	TEST_EQ( dispatcher.GetEventListenerMap().Count(), 0 );
	TEST_FALSE( dispatcher.UnregisterEventListener( s_DelegateA, localEvent.ID() ) );

	int nVisited = 0;
	FOR_EACH_EVENT_LISTENER( dispatcher, localEvent.ID(), pInfo )
	{
		nVisited++;
	}

	TEST_EQ( nVisited, 0 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );
}

REGISTER_NAMED_TEST( "EventDispatcher.UnregisterRemovesOnlyFromItsEvent", EventDispatcher_UnregisterRemovesOnlyFromItsEvent )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass startEvent( true ), stopEvent( true ), otherEvent( true );

	dispatcher.RegisterEventListener( s_DelegateA, startEvent.ID(), 0, "A" );
	dispatcher.RegisterEventListener( s_DelegateA, stopEvent.ID(), 0, "A" );

	TEST_TRUE( dispatcher.UnregisterEventListener( s_DelegateA, startEvent.ID() ) );
	TEST_FALSE( dispatcher.UnregisterEventListener( s_DelegateA, startEvent.ID() ) );
	TEST_FALSE( dispatcher.UnregisterEventListener( s_DelegateA, otherEvent.ID() ) );

	CUtlVector< CUtlString > names;
	dispatcher.GetListenerNames( stopEvent.ID(), &names );
	TEST_EQ( names.Count(), 1 );
}

REGISTER_NAMED_TEST( "EventDispatcher.NewEventsDoNotBreakActiveIterations", EventDispatcher_NewEventsDoNotBreakActiveIterations )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass startEvent( true );
	CTestEventClass newEvents[ 64 ];

	dispatcher.RegisterEventListener( s_DelegateA, startEvent.ID(), 10, "A" );
	dispatcher.RegisterEventListener( s_DelegateB, startEvent.ID(), 0, "B" );

	// A listener that registers for new events grows the map under the iteration, which then
	// has to follow its listener list to the new place.
	CUtlString order;
	FOR_EACH_EVENT_LISTENER( dispatcher, startEvent.ID(), pInfo )
	{
		order.Append( pInfo->m_pszName );

		for ( auto &event : newEvents )
			dispatcher.RegisterEventListener( s_DelegateC, event.ID(), 0, "C" );
	}

	TEST_EQ( strcmp( order.Get(), "AB" ), 0 );
	TEST_EQ( dispatcher.GetEventListenerMap().Count(), 65 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );
}
