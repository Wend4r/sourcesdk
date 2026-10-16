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

// Events identified by the schema class of their message
class CEventID_SchemaBinding
{
public:
	typedef SchemaMetaInfoHandle_t< CSchemaClassInfo > EventID_t;

	int8 unused;
};

class CEventIDManager_SchemaBinding : public CEventID_SchemaBinding
{
};

class CEventIDManager_Default : public CEventIDManager_SchemaBinding
{
};

//-----------------------------------------------------------------------------
// Dispatcher that keeps a listener list per event ID. Only the schema classes in the global
// type scope identify an event, the other IDs are ignored. The lock is held between
// IterateListeners_Start returning true and IterateListeners_End
//-----------------------------------------------------------------------------
template < typename T >
class CEventDispatcher_Identified : public CEventDispatcher_Base
{
public:
	typedef typename T::EventID_t EventID_t;
	typedef CUtlVector< EventListenerInfo_t > ListenerVector_t;
	typedef CUtlOrderedMap< EventID_t, ListenerVector_t, CDefLess< EventID_t >, unsigned int > EventListenerMap_t;

	struct DelegateIterator_t : DelegateIterator_Base_t
	{
		EventID_t EventID;
	};

	CEventDispatcher_Identified() = default;

	static bool IDLessFunc( const EventID_t &lhs, const EventID_t &rhs ) { return lhs < rhs; }

	static bool IsValidEventID( const EventID_t &eventID )
	{
		return eventID && ( eventID->m_nFlags1 & ( 1 << 7 ) ) != 0; // SCHEMA_CF1_GLOBAL_TYPE_SCOPE
	}

	void RegisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, uint32 nDelegateParamCount, bool bDelegateReturnsVoid, EventID_t eventID, int nPriority, const char *pszName )
	{
		if ( !IsValidEventID( eventID ) )
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

	template < typename R, typename... ARGS >
	void RegisterEventListener( const CUtlDelegate< R ( ARGS... ) > &delegate, EventID_t eventID, int nPriority = 0, const char *pszName = nullptr )
	{
		RegisterEventListener_Abstract( delegate.GetAbstractDelegate(), sizeof...( ARGS ), std::is_void_v< R >, eventID, nPriority, pszName );
	}

	// False when the delegate was not registered for the event
	bool UnregisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, EventID_t eventID )
	{
		if ( !IsValidEventID( eventID ) )
			return false;

		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );
		bool bResult = m_EventListenerMap.IsValidIndex( idx ) && UnregisterEventListener_Base( delegate, m_EventListenerMap.Element( idx ) );

		m_Lock.Unlock();

		return bResult;
	}

	template < typename R, typename... ARGS >
	bool UnregisterEventListener( const CUtlDelegate< R ( ARGS... ) > &delegate, EventID_t eventID )
	{
		return UnregisterEventListener_Abstract( delegate.GetAbstractDelegate(), eventID );
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
			Assert( 0 );
			return false;
		}

		iter.EventID = eventID;

		m_Lock.Lock();

		auto idx = m_EventListenerMap.Find( eventID );

		iter.pListeners = m_EventListenerMap.IsValidIndex( idx ) ? &m_EventListenerMap.Element( idx ) : nullptr;

		if ( !CEventDispatcher_Base::IterateListeners_Start( iter, bForward ) )
		{
			CEventDispatcher_Base::IterateListeners_End( iter );
			m_Lock.Unlock();

			return false;
		}

		return true;
	}

	const EventListenerInfo_t *IterateListener( DelegateIterator_t &iter ) { return CEventDispatcher_Base::IterateListener( iter ); }

	void IterateListeners_End( DelegateIterator_t &iter )
	{
		CEventDispatcher_Base::IterateListeners_End( iter );
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
	EventListenerMap_t m_EventListenerMap;
};

template < typename T >
class CEventDispatcher : public CEventDispatcher_Identified< T >
{
};

// Visits the listeners from the highest priority (or from the lowest with _BACK). The key is
// the listener vector for CEventDispatcher_Base and the event ID for the identified dispatchers
#define FOR_EACH_EVENT_LISTENER( dispatcher, key, pInfo ) \
	for ( auto pInfo##_iter = ( dispatcher ).IterateListeners( key, true ); auto pInfo = pInfo##_iter.Next(); )

#define FOR_EACH_EVENT_LISTENER_BACK( dispatcher, key, pInfo ) \
	for ( auto pInfo##_iter = ( dispatcher ).IterateListeners( key, false ); auto pInfo = pInfo##_iter.Next(); )

enum EventMapRegistrationType_t : int32
{
	EVENT_MAP_REGISTER = 0,
	EVENT_MAP_UNREGISTER,
};

#endif // EVENTDISPATCHER_H