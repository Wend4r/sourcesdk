#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#ifdef _WIN32
#pragma once
#endif

#include <tier1/utldelegate.h>
#include <tier0/utlmap.h>
#include <tier0/utlvector.h>

struct SchemaClassInfoData_t;

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

	// Index where a listener with nPriority goes
	DLL_CLASS_IMPORT int FindInsertionPoint( const CUtlVector< EventListenerInfo_t > &listeners, int nPriority );

	CThreadFastMutex m_Lock;
	DelegateIterator_Base_t *m_pActiveIterators;

private:
	CEventDispatcher_Base( const CEventDispatcher_Base & ) = delete;
	CEventDispatcher_Base &operator=( const CEventDispatcher_Base & ) = delete;
};

struct CEventID_SchemaBinding
{
	int8 unused;
};

struct CEventIDManager_SchemaBinding : CEventID_SchemaBinding
{
};

struct CEventIDManager_Default : CEventIDManager_SchemaBinding
{
};

template <typename T>
struct CEventDispatcher_Identified : CEventDispatcher_Base
{
	CUtlMap<const SchemaClassInfoData_t*, CUtlVector<EventListenerInfo_t>, unsigned int> m_EventListenerMap;
};

template <typename T>
struct CEventDispatcher : CEventDispatcher_Identified<T>
{
};

enum EventMapRegistrationType_t : int32
{
	EVENT_MAP_REGISTER = 0,
	EVENT_MAP_UNREGISTER,
};

#endif // EVENTDISPATCHER_H