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

// The message of a test event; its schema binding is the class info a test sets.
struct TestEvent_t
{
	static const CSchemaClassInfo *Schema_StaticBinding() { return s_pBinding; }

	static CSchemaClassInfo *s_pBinding;

	int m_nValue;
};

CSchemaClassInfo *TestEvent_t::s_pBinding = nullptr;

typedef CEvent_Manager< CEventIDManager_Default, TestEvent_t > TestEventManager_t;

// Records the calls of the test event listeners.
static CUtlString s_Calls;
static int s_nLastValue;

static void OnEventA( const TestEvent_t &msg ) { s_Calls.Append( "A" ); s_nLastValue = msg.m_nValue; }
static void OnEventB( const TestEvent_t &msg ) { s_Calls.Append( "B" ); s_nLastValue = msg.m_nValue; }
static void OnEventC( const TestEvent_t &msg ) { s_Calls.Append( "C" ); s_nLastValue = msg.m_nValue; }
static void OnEventNoParam() { s_Calls.Append( "N" ); }
static bool OnEventContinue( const TestEvent_t & ) { s_Calls.Append( "T" ); return true; }
static bool OnEventStop() { s_Calls.Append( "F" ); return false; }

static CUtlDelegate< void ( const TestEvent_t & ) > s_EventA( &OnEventA );
static CUtlDelegate< void ( const TestEvent_t & ) > s_EventB( &OnEventB );
static CUtlDelegate< void ( const TestEvent_t & ) > s_EventC( &OnEventC );
static CUtlDelegate< void () > s_EventNoParam( &OnEventNoParam );
static CUtlDelegate< bool ( const TestEvent_t & ) > s_EventContinue( &OnEventContinue );
static CUtlDelegate< bool () > s_EventStop( &OnEventStop );

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
	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	event.RegisterEventListener( s_EventA, 0, "A", startEvent.ID() );
	event.RegisterEventListener( s_EventB, 10, "B", startEvent.ID() );
	event.RegisterEventListener( s_EventC, 0, "C", stopEvent.ID() );

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
	TEST_TRUE( dispatcher.HasListeners( startEvent.ID() ) );

	CUtlVector< CUtlString > names;
	dispatcher.GetListenerNames( startEvent.ID(), &names );
	TEST_EQ( names.Count(), 2 );
	TEST_EQ( strcmp( names[ 0 ].Get(), "B" ), 0 );
}

REGISTER_NAMED_TEST( "EventDispatcher.OnlyGlobalScopeClassesIdentifyEvents", EventDispatcher_OnlyGlobalScopeClassesIdentifyEvents )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass localEvent( false );
	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	TEST_FALSE( CEventIDManager_Default::IsValidEventID( localEvent.ID() ) );
	TEST_FALSE( CEventIDManager_Default::IsValidEventID( EventID_t() ) );

	event.RegisterEventListener( s_EventA, 0, "A", localEvent.ID() );
	event.RegisterEventListener( s_EventA, 0, "A", EventID_t() );

	TEST_EQ( dispatcher.GetEventListenerMap().Count(), 0 );
	TEST_FALSE( dispatcher.HasListeners( localEvent.ID() ) );

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
	CTestEventClass startEvent( true ), stopEvent( true );
	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	event.RegisterEventListener( s_EventA, 0, "A", startEvent.ID() );
	event.RegisterEventListener( s_EventA, 0, "A", stopEvent.ID() );

	event.UnregisterEventListener( s_EventA, startEvent.ID() );

	TEST_FALSE( dispatcher.HasListeners( startEvent.ID() ) );
	TEST_TRUE( dispatcher.HasListeners( stopEvent.ID() ) );
}

REGISTER_NAMED_TEST( "EventDispatcher.NewEventsDoNotBreakActiveIterations", EventDispatcher_NewEventsDoNotBreakActiveIterations )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass startEvent( true );
	CTestEventClass newEvents[ 64 ];
	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	event.RegisterEventListener( s_EventA, 10, "A", startEvent.ID() );
	event.RegisterEventListener( s_EventB, 0, "B", startEvent.ID() );

	// A listener that registers for new events grows the map under the iteration, which then
	// has to follow its listener list to the new place.
	CUtlString order;
	FOR_EACH_EVENT_LISTENER( dispatcher, startEvent.ID(), pInfo )
	{
		order.Append( pInfo->m_pszName );

		for ( auto &newEvent : newEvents )
			event.RegisterEventListener( s_EventC, 0, "C", newEvent.ID() );
	}

	TEST_EQ( strcmp( order.Get(), "AB" ), 0 );
	TEST_EQ( dispatcher.GetEventListenerMap().Count(), 65 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );
}

REGISTER_NAMED_TEST( "EventDispatcher.DispatchCallsTheListenersByPriority", EventDispatcher_DispatchCallsTheListenersByPriority )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass eventClass( true );
	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	event.RegisterEventListener( s_EventA, 0, "A", eventClass.ID() );
	event.RegisterEventListener( s_EventB, 10, "B", eventClass.ID() );
	event.RegisterEventListener( s_EventNoParam, 5, "N", eventClass.ID() );

	TestEvent_t msg = { 42 };

	s_Calls.Clear();
	s_nLastValue = 0;
	TEST_TRUE( event.DispatchEvent( msg, true, eventClass.ID() ) );
	TEST_EQ( strcmp( s_Calls.Get(), "BNA" ), 0 );
	TEST_EQ( s_nLastValue, 42 );

	s_Calls.Clear();
	event.DispatchEvent( msg, false, eventClass.ID() );
	TEST_EQ( strcmp( s_Calls.Get(), "ANB" ), 0 );
	TEST_TRUE( dispatcher.m_pActiveIterators == nullptr );
}

REGISTER_NAMED_TEST( "EventDispatcher.ListenerReturningFalseStopsTheDispatch", EventDispatcher_ListenerReturningFalseStopsTheDispatch )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass eventClass( true );
	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	event.RegisterEventListener( s_EventContinue, 20, "T", eventClass.ID() );
	event.RegisterEventListener( s_EventStop, 10, "F", eventClass.ID() );
	event.RegisterEventListener( s_EventA, 0, "A", eventClass.ID() );

	TestEvent_t msg = { 1 };

	s_Calls.Clear();
	TEST_FALSE( event.DispatchEvent( msg, true, eventClass.ID() ) );
	TEST_EQ( strcmp( s_Calls.Get(), "TF" ), 0 );

	// Without listeners the dispatch returns the default value.
	event.UnregisterEventListener( s_EventStop, eventClass.ID() );
	event.UnregisterEventListener( s_EventContinue, eventClass.ID() );
	event.UnregisterEventListener( s_EventA, eventClass.ID() );

	s_Calls.Clear();
	TEST_TRUE( event.DispatchEvent( msg, true, eventClass.ID() ) );
	TEST_EQ( s_Calls.Length(), 0 );
}

REGISTER_NAMED_TEST( "EventDispatcher.EventIDComesFromTheSchemaBinding", EventDispatcher_EventIDComesFromTheSchemaBinding )
{
	CTestIdentifiedDispatcher dispatcher;
	CTestEventClass eventClass( true );
	eventClass.m_Info.m_pszName = "TestEvent_t";
	TestEvent_t::s_pBinding = &eventClass.m_Info;

	TestEventManager_t &event = dispatcher.Event< TestEvent_t >();

	// Without an explicit ID the listener goes to the event of the binding.
	event.RegisterEventListener( s_EventA, 0, "A" );
	TEST_TRUE( dispatcher.HasListeners( eventClass.ID() ) );

	TestEvent_t msg = { 7 };

	s_Calls.Clear();
	event.DispatchEvent( msg );
	TEST_EQ( strcmp( s_Calls.Get(), "A" ), 0 );

	char szName[ 32 ];
	TEST_EQ( strcmp( dispatcher.GetEventName( eventClass.ID(), szName, sizeof( szName ) ), "TestEvent_t" ), 0 );

	event.UnregisterEventListener( s_EventA );
	TEST_FALSE( dispatcher.HasListeners( eventClass.ID() ) );

	TestEvent_t::s_pBinding = nullptr;
}
