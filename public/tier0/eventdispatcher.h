#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#ifdef _WIN32
#pragma once
#endif

#include <tier1/utldelegate.h>
#include <tier0/utlmap.h>
#include <tier0/utlvector.h>
#include <schemasystem/schematypes.h>

#include <type_traits>

//-----------------------------------------------------------------------------
// Base of the event dispatchers: keeps the listeners of an event sorted by descending
// priority and lets the listeners be added and removed while they are iterated
//-----------------------------------------------------------------------------
class CEventDispatcher_Base
{
public:
	struct EventListenerInfo_t
	{
		EventListenerInfo_t() : m_pszName( nullptr ), m_nPriority( 0 ), m_nDelegateParamCount( 0 ), m_bDelegateReturnsVoid( false ) {}

		CUtlAbstractDelegate m_Delegate;
		const char *m_pszName;
		int32 m_nPriority;
		uint8 m_nDelegateParamCount;
		bool m_bDelegateReturnsVoid;
	};

	// Iteration over the listeners of one event. The dispatcher keeps its active iterators in a
	// list, so that registering or unregistering a listener moves their positions along
	struct DelegateIterator_Base_t
	{
		const CUtlVector< EventListenerInfo_t > *pListeners;

		// Listeners added behind the position of the iterator, which it passes over
		CUtlVectorFixedGrowable< int, 4 > skipListeners;

		int nCurrent;
		DelegateIterator_Base_t *pNext;
		bool bIteratingForward;
		bool bIsInListenerTelemetryScope;
	};

	// Scoped iteration: the destructor ends it, so a loop over Next() may break out early
	class CListenerIterator
	{
	public:
		CListenerIterator( CEventDispatcher_Base &dispatcher, const CUtlVector< EventListenerInfo_t > *pListeners, bool bForward ) : m_Dispatcher( dispatcher )
		{
			m_Iter.pListeners = pListeners;
			m_bHasListeners = dispatcher.IterateListeners_Start( m_Iter, bForward );
		}

		~CListenerIterator() { m_Dispatcher.IterateListeners_End( m_Iter ); }

		// Null after the last listener
		const EventListenerInfo_t *Next() { return m_bHasListeners ? m_Dispatcher.IterateListener( m_Iter ) : nullptr; }

	private:
		CListenerIterator( const CListenerIterator & ) = delete;
		CListenerIterator &operator=( const CListenerIterator & ) = delete;

		CEventDispatcher_Base &m_Dispatcher;
		DelegateIterator_Base_t m_Iter;
		bool m_bHasListeners;
	};

protected:
	DLL_CLASS_IMPORT CEventDispatcher_Base();

	// Inserts the listener after the ones with the same or a higher priority
	DLL_CLASS_IMPORT void RegisterEventListener_Base( const CUtlAbstractDelegate &delegate, uint32 nDelegateParamCount, bool bDelegateReturnsVoid, CUtlVector< EventListenerInfo_t > &listeners, int nPriority, const char *pszName );

	// False when the delegate was not registered
	DLL_CLASS_IMPORT bool UnregisterEventListener_Base( const CUtlAbstractDelegate &delegate, CUtlVector< EventListenerInfo_t > &listeners );

	// Appends the names of the listeners
	DLL_CLASS_IMPORT void GetListenerNames_Base( const CUtlVector< EventListenerInfo_t > &listeners, CUtlVector< CUtlString > *pNames );

	// Iterate over iter.pListeners from the first or the last listener: Start links the iterator
	// into the active ones and returns whether there is a listener, IterateListener returns the
	// next one or null after the last, and End unlinks the iterator
	DLL_CLASS_IMPORT bool IterateListeners_Start( DelegateIterator_Base_t &iter, bool bForward );
	DLL_CLASS_IMPORT const EventListenerInfo_t *IterateListener( DelegateIterator_Base_t &iter );
	DLL_CLASS_IMPORT void IterateListeners_End( DelegateIterator_Base_t &iter );

	// For FOR_EACH_EVENT_LISTENER
	CListenerIterator IterateListeners( const CUtlVector< EventListenerInfo_t > &listeners, bool bForward ) { return CListenerIterator( *this, &listeners, bForward ); }

	// Index where a listener with nPriority goes
	DLL_CLASS_IMPORT int FindInsertionPoint( const CUtlVector< EventListenerInfo_t > &listeners, int nPriority );

	CThreadFastMutex m_Lock;
	DelegateIterator_Base_t *m_pActiveIterators;

private:
	CEventDispatcher_Base( const CEventDispatcher_Base & ) = delete;
	CEventDispatcher_Base &operator=( const CEventDispatcher_Base & ) = delete;
};

enum EventMapRegistrationType_t : int32
{
	EVENT_MAP_REGISTER = 0,
	EVENT_MAP_UNREGISTER,
};

//-----------------------------------------------------------------------------
// Events identified by the schema class of their message. An event ID is valid when the class
// is in the global type scope, and the ID of an event type comes from the static binding of
// the class that the schema compiler declares in it
//-----------------------------------------------------------------------------
class CEventID_SchemaBinding
{
public:
	typedef SchemaMetaInfoHandle_t< CSchemaClassInfo > EventID_t;

	template < typename EVENT_DESCRIPTOR >
	class EventIDGenerator
	{
	public:
		static EventID_t GetEventID() { return EventID_t( const_cast< CSchemaClassInfo * >( EVENT_DESCRIPTOR::Schema_StaticBinding() ) ); }
	};

	static bool IsValidEventID( EventID_t eventID )
	{
		return eventID && ( eventID->m_nFlags1 & ( 1 << 7 ) ) != 0; // SCHEMA_CF1_GLOBAL_TYPE_SCOPE
	}

	// Copies the class name of the event to pszBuffer and returns it
	static const char *GetEventName( EventID_t eventID, char *pszBuffer, size_t nBufferSize )
	{
		V_strncpy( pszBuffer, eventID && eventID->m_pszName ? eventID->m_pszName : "<null>", static_cast< int >( nBufferSize ) );

		return pszBuffer;
	}
};

//-----------------------------------------------------------------------------
// Signature of the events with one parameter, the event message by const reference. The
// listeners may return void or bool, and a listener returning false stops the dispatch
//-----------------------------------------------------------------------------
class CEventSignature_Bool_ConstStructRef
{
public:
	class EventSignature_Base
	{
	public:
		enum
		{
			PARAMETER_COUNT = 1
		};

		typedef bool ReturnType_t;

		static ReturnType_t GetDefaultReturnValue() { return true; }

		// Takes the value of the last listener, false ends the iteration
		static bool MergeReturnValueAndContinueIterating( ReturnType_t &result, const ReturnType_t &value )
		{
			result = value;

			return value;
		}
	};

	template < typename EVENT_DESCRIPTOR >
	class EventSignature : public EventSignature_Base
	{
	public:
		typedef const EVENT_DESCRIPTOR &Param1Type_t;
	};
};

class CEventIDManager_SchemaBinding : public CEventID_SchemaBinding
{
public:
	template < typename EVENT_DESCRIPTOR >
	class EventDescription : public EventIDGenerator< EVENT_DESCRIPTOR >, public CEventSignature_Bool_ConstStructRef::EventSignature< EVENT_DESCRIPTOR >
	{
	};
};

class CEventIDManager_Default : public CEventIDManager_SchemaBinding
{
};

// Result of a dispatch, merged from the return values of the listeners
template < typename EVENT_DESCRIPTION, typename RETURN_TYPE >
class CEventDispatcher_WrappedReturnType
{
public:
	typedef RETURN_TYPE ReturnType_t;

	CEventDispatcher_WrappedReturnType() : m_Value( EVENT_DESCRIPTION::GetDefaultReturnValue() ) {}

	bool MergeReturnValueAndContinueIterating( const ReturnType_t &value ) { return EVENT_DESCRIPTION::MergeReturnValueAndContinueIterating( m_Value, value ); }
	ReturnType_t &Get() { return m_Value; }

private:
	ReturnType_t m_Value;
};

//-----------------------------------------------------------------------------
// Dispatcher that keeps a listener list per event ID. Only the valid IDs of EVENT_ID_MANAGER
// identify an event, the other ones are ignored. The lock is held between
// IterateListeners_Start returning true and IterateListeners_End
//-----------------------------------------------------------------------------
template < typename EVENT_ID_MANAGER >
class CEventDispatcher_Identified : public CEventDispatcher_Base
{
public:
	typedef CEventDispatcher_Base BaseClass;
	typedef typename EVENT_ID_MANAGER::EventID_t EventID_t;
	typedef CUtlVector< EventListenerInfo_t > ListenerVector_t;
	typedef CUtlOrderedMap< EventID_t, ListenerVector_t, CDefLess< EventID_t >, unsigned int > EventListenerMap_t;

	struct DelegateIterator_t : DelegateIterator_Base_t
	{
		EventID_t EventID;
	};

	explicit CEventDispatcher_Identified( int nGrowSize = 0 ) : m_EventListenerMap( nGrowSize, 0 ) {}

	static const char *GetEventName( EventID_t eventID, char *pszBuffer, size_t nBufferSize ) { return EVENT_ID_MANAGER::GetEventName( eventID, pszBuffer, nBufferSize ); }
	static bool IDLessFunc( const EventID_t &lhs, const EventID_t &rhs ) { return lhs < rhs; }

	bool HasListeners( EventID_t eventID )
	{
		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );
		bool bResult = m_EventListenerMap.IsValidIndex( idx ) && m_EventListenerMap.Element( idx ).Count() > 0;

		m_Lock.Unlock();

		return bResult;
	}

	// Appends the names of the listeners of the event
	void GetListenerNames( EventID_t eventID, CUtlVector< CUtlString > *pNames )
	{
		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );

		if ( m_EventListenerMap.IsValidIndex( idx ) )
			GetListenerNames_Base( m_EventListenerMap.Element( idx ), pNames );

		m_Lock.Unlock();
	}

	// Returns true with the lock held when the event has a listener, call IterateListeners_End
	// after the last IterateListener then
	bool IterateListeners_Start( DelegateIterator_t &iter, EventID_t eventID, bool bForward )
	{
		if ( !eventID )
		{
			Assert( eventID );

			return false;
		}

		iter.EventID = eventID;

		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );

		iter.pListeners = m_EventListenerMap.IsValidIndex( idx ) ? &m_EventListenerMap.Element( idx ) : nullptr;

		if ( !BaseClass::IterateListeners_Start( iter, bForward ) )
		{
			BaseClass::IterateListeners_End( iter );
			m_Lock.Unlock();

			return false;
		}

		return true;
	}

	const EventListenerInfo_t *IterateListener( DelegateIterator_t &iter ) { return BaseClass::IterateListener( iter ); }

	void IterateListeners_End( DelegateIterator_t &iter )
	{
		BaseClass::IterateListeners_End( iter );
		m_Lock.Unlock();
	}

	void RegisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, uint32 nDelegateParamCount, bool bDelegateReturnsVoid, EventID_t eventID, int nPriority, const char *pszName )
	{
		if ( !EVENT_ID_MANAGER::IsValidEventID( eventID ) )
			return;

		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );

		if ( !m_EventListenerMap.IsValidIndex( idx ) )
		{
			idx = m_EventListenerMap.Insert( eventID, ListenerVector_t() );

			// The insertion may move the listener lists of the active iterators
			for ( DelegateIterator_Base_t *pIter = m_pActiveIterators; pIter; pIter = pIter->pNext )
			{
				if ( !pIter->pListeners )
					continue;

				auto iterIdx = m_EventListenerMap.Find( static_cast< DelegateIterator_t * >( pIter )->EventID );

				pIter->pListeners = m_EventListenerMap.IsValidIndex( iterIdx ) ? &m_EventListenerMap.Element( iterIdx ) : nullptr;
			}
		}

		RegisterEventListener_Base( delegate, nDelegateParamCount, bDelegateReturnsVoid, m_EventListenerMap.Element( idx ), nPriority, pszName );

		m_Lock.Unlock();
	}

	void UnregisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, EventID_t eventID )
	{
		if ( !EVENT_ID_MANAGER::IsValidEventID( eventID ) )
			return;

		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );

		if ( m_EventListenerMap.IsValidIndex( idx ) )
			UnregisterEventListener_Base( delegate, m_EventListenerMap.Element( idx ) );

		m_Lock.Unlock();
	}

	// Scoped iteration over the listeners of the event, holding the lock while there are any
	class CListenerIterator
	{
	public:
		CListenerIterator( CEventDispatcher_Identified &dispatcher, EventID_t eventID, bool bForward ) : m_Dispatcher( dispatcher )
		{
			m_bHasListeners = dispatcher.IterateListeners_Start( m_Iter, eventID, bForward );
		}

		~CListenerIterator()
		{
			if ( m_bHasListeners )
				m_Dispatcher.IterateListeners_End( m_Iter );
		}

		// Null after the last listener
		const EventListenerInfo_t *Next() { return m_bHasListeners ? m_Dispatcher.IterateListener( m_Iter ) : nullptr; }

	private:
		CListenerIterator( const CListenerIterator & ) = delete;
		CListenerIterator &operator=( const CListenerIterator & ) = delete;

		CEventDispatcher_Identified &m_Dispatcher;
		DelegateIterator_t m_Iter;
		bool m_bHasListeners;
	};

	// For FOR_EACH_EVENT_LISTENER
	CListenerIterator IterateListeners( EventID_t eventID, bool bForward ) { return CListenerIterator( *this, eventID, bForward ); }

	const EventListenerMap_t &GetEventListenerMap() const { return m_EventListenerMap; }

protected:
	template < typename R, typename... ARGS >
	void RegisterEventListener( const CUtlDelegate< R ( ARGS... ) > &delegate, int nPriority, const char *pszName, EventID_t eventID )
	{
		RegisterEventListener_Abstract( delegate.GetAbstractDelegate(), sizeof...( ARGS ), std::is_void_v< R >, eventID, nPriority, pszName );
	}

	template < typename R, typename... ARGS >
	void UnregisterEventListener( const CUtlDelegate< R ( ARGS... ) > &delegate, EventID_t eventID )
	{
		UnregisterEventListener_Abstract( delegate.GetAbstractDelegate(), eventID );
	}

	EventListenerMap_t m_EventListenerMap;
};

//-----------------------------------------------------------------------------
// Typed access to the listeners of one event, reached through CEventDispatcher::Event. Each
// level accepts the delegates of one signature: without and with the event parameter, and
// returning void or the return type of the event
//-----------------------------------------------------------------------------
template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR, typename RETURNTYPE, int PARAMCOUNT >
class CEvent_Register;

template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR >
class CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, void, 0 > : public CEventDispatcher_Identified< EVENT_ID_MANAGER >
{
public:
	typedef CEventDispatcher_Identified< EVENT_ID_MANAGER > BaseClass;
	typedef typename BaseClass::EventID_t EventID_t;
	typedef typename EVENT_ID_MANAGER::template EventDescription< EVENT_DESCRIPTOR > EventDescription_t;

	void RegisterEventListener( const CUtlDelegate< void () > &delegate, int nPriority = 0, const char *pszName = nullptr, EventID_t eventID = EventDescription_t::GetEventID() ) { BaseClass::RegisterEventListener( delegate, nPriority, pszName, eventID ); }
	void UnregisterEventListener( const CUtlDelegate< void () > &delegate, EventID_t eventID = EventDescription_t::GetEventID() ) { BaseClass::UnregisterEventListener( delegate, eventID ); }

private:
	CEvent_Register();
};

template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR, typename RETURNTYPE >
class CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, RETURNTYPE, 0 > : public CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, void, EVENT_ID_MANAGER::template EventDescription< EVENT_DESCRIPTOR >::PARAMETER_COUNT >
{
public:
	typedef CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, void, EVENT_ID_MANAGER::template EventDescription< EVENT_DESCRIPTOR >::PARAMETER_COUNT > BaseClass;
	typedef typename BaseClass::EventID_t EventID_t;
	typedef typename BaseClass::EventDescription_t EventDescription_t;

	using BaseClass::RegisterEventListener;
	using BaseClass::UnregisterEventListener;

	void RegisterEventListener( const CUtlDelegate< RETURNTYPE () > &delegate, int nPriority = 0, const char *pszName = nullptr, EventID_t eventID = EventDescription_t::GetEventID() ) { CEventDispatcher_Identified< EVENT_ID_MANAGER >::RegisterEventListener( delegate, nPriority, pszName, eventID ); }
	void UnregisterEventListener( const CUtlDelegate< RETURNTYPE () > &delegate, EventID_t eventID = EventDescription_t::GetEventID() ) { CEventDispatcher_Identified< EVENT_ID_MANAGER >::UnregisterEventListener( delegate, eventID ); }

private:
	CEvent_Register();
};

template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR, typename RETURNTYPE >
class CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, RETURNTYPE, 1 > : public CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, RETURNTYPE, 0 >
{
public:
	typedef CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, RETURNTYPE, 0 > BaseClass;
	typedef typename BaseClass::EventID_t EventID_t;
	typedef typename BaseClass::EventDescription_t EventDescription_t;
	typedef typename EventDescription_t::Param1Type_t Param1Type_t;

	using BaseClass::RegisterEventListener;
	using BaseClass::UnregisterEventListener;

	void RegisterEventListener( const CUtlDelegate< RETURNTYPE ( Param1Type_t ) > &delegate, int nPriority = 0, const char *pszName = nullptr, EventID_t eventID = EventDescription_t::GetEventID() ) { CEventDispatcher_Identified< EVENT_ID_MANAGER >::RegisterEventListener( delegate, nPriority, pszName, eventID ); }
	void UnregisterEventListener( const CUtlDelegate< RETURNTYPE ( Param1Type_t ) > &delegate, EventID_t eventID = EventDescription_t::GetEventID() ) { CEventDispatcher_Identified< EVENT_ID_MANAGER >::UnregisterEventListener( delegate, eventID ); }

private:
	CEvent_Register();
};

//-----------------------------------------------------------------------------
// Calls the listeners of an event in the order of their priorities, or in the reverse order
// when bForward is false, until a listener returning a value asks to stop
//-----------------------------------------------------------------------------
template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR, int PARAMCOUNT >
class CEvent_Dispatch;

template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR >
class CEvent_Dispatch< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, 1 > : public CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, typename EVENT_ID_MANAGER::template EventDescription< EVENT_DESCRIPTOR >::ReturnType_t, 1 >
{
public:
	typedef CEvent_Register< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, typename EVENT_ID_MANAGER::template EventDescription< EVENT_DESCRIPTOR >::ReturnType_t, 1 > BaseClass;
	typedef typename BaseClass::EventID_t EventID_t;
	typedef typename BaseClass::EventDescription_t EventDescription_t;
	typedef typename EventDescription_t::ReturnType_t ReturnType_t;
	typedef typename EventDescription_t::Param1Type_t Param1Type_t;

	ReturnType_t DispatchEvent( Param1Type_t param1, bool bForward = true, EventID_t eventID = EventDescription_t::GetEventID() )
	{
		CEventDispatcher_WrappedReturnType< EventDescription_t, ReturnType_t > result;
		typename BaseClass::DelegateIterator_t iter;

		if ( !this->IterateListeners_Start( iter, eventID, bForward ) )
			return result.Get();

		while ( const CEventDispatcher_Base::EventListenerInfo_t *pInfo = this->IterateListener( iter ) )
		{
			if ( pInfo->m_bDelegateReturnsVoid )
			{
				if ( pInfo->m_nDelegateParamCount == 0 )
				{
					CUtlDelegate< void () > delegate;
					delegate.SetAbstractDelegate( pInfo->m_Delegate );
					delegate();
				}
				else
				{
					Assert( pInfo->m_nDelegateParamCount == 1 );

					CUtlDelegate< void ( Param1Type_t ) > delegate;
					delegate.SetAbstractDelegate( pInfo->m_Delegate );
					delegate( param1 );
				}

				continue;
			}

			ReturnType_t value;

			if ( pInfo->m_nDelegateParamCount == 0 )
			{
				CUtlDelegate< ReturnType_t () > delegate;
				delegate.SetAbstractDelegate( pInfo->m_Delegate );
				value = delegate();
			}
			else
			{
				Assert( pInfo->m_nDelegateParamCount == 1 );

				CUtlDelegate< ReturnType_t ( Param1Type_t ) > delegate;
				delegate.SetAbstractDelegate( pInfo->m_Delegate );
				value = delegate( param1 );
			}

			if ( !result.MergeReturnValueAndContinueIterating( value ) )
				break;
		}

		this->IterateListeners_End( iter );

		return result.Get();
	}

private:
	CEvent_Dispatch();
};

template < typename EVENT_ID_MANAGER, typename EVENT_DESCRIPTOR >
class CEvent_Manager : public CEvent_Dispatch< EVENT_ID_MANAGER, EVENT_DESCRIPTOR, EVENT_ID_MANAGER::template EventDescription< EVENT_DESCRIPTOR >::PARAMETER_COUNT >
{
private:
	CEvent_Manager();
};

//-----------------------------------------------------------------------------
// The dispatcher the engine passes to the services and loop modes:
//
//	pEventDispatcher->Event< EventServerPollNetworking_t >().RegisterEventListener( UtlMakeDelegate( this, &CMyService::OnPollNetworking ), 0, "CMyService::OnPollNetworking" );
//	pEventDispatcher->Event< EventServerPollNetworking_t >().DispatchEvent( msg );
//-----------------------------------------------------------------------------
template < typename EVENT_ID_MANAGER >
class CEventDispatcher : public CEventDispatcher_Identified< EVENT_ID_MANAGER >
{
public:
	typedef CEventDispatcher_Identified< EVENT_ID_MANAGER > BaseClass;

	explicit CEventDispatcher( int nGrowSize = 0 ) : BaseClass( nGrowSize ) {}

	// The event classes add no members, so the dispatcher is used as the one of the event
	template < typename EVENT_DESCRIPTOR >
	CEvent_Manager< EVENT_ID_MANAGER, EVENT_DESCRIPTOR > &Event() { return *reinterpret_cast< CEvent_Manager< EVENT_ID_MANAGER, EVENT_DESCRIPTOR > * >( this ); }
};

// Registrar of the services and loop modes without events
template < typename EVENT_ID_MANAGER >
class CEventMapEmptyRegistrar
{
public:
	static void OnEventMapRegistration( CEventDispatcher< EVENT_ID_MANAGER > *pEventDispatcher, EventMapRegistrationType_t nRegistrationType ) {}
};

// Visits the listeners from the highest priority (or from the lowest with _BACK). The key is
// the listener vector for CEventDispatcher_Base and the event ID for the identified dispatchers
#define FOR_EACH_EVENT_LISTENER( dispatcher, key, pInfo ) \
	for ( auto pInfo##_iter = ( dispatcher ).IterateListeners( key, true ); auto pInfo = pInfo##_iter.Next(); )

#define FOR_EACH_EVENT_LISTENER_BACK( dispatcher, key, pInfo ) \
	for ( auto pInfo##_iter = ( dispatcher ).IterateListeners( key, false ); auto pInfo = pInfo##_iter.Next(); )

#endif // EVENTDISPATCHER_H