//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Additional shared object cache functionality for the GC
//
//=============================================================================

#ifndef GCCLIENT_SHAREDOBJECTCACHE_H
#define GCCLIENT_SHAREDOBJECTCACHE_H
#ifdef _WIN32
#pragma once
#endif
#include "soid.h"
#include "sharedobjectcache.h"

class CMsgSOCacheSubscribed;
class CMsgSOCacheSubscribed_SubscribedType;

namespace GCSDK
{

class CGCClientSharedObjectCache;

/// Enumerate different events that might trigger a callback to an ISharedObjectListener
enum ESOCacheEvent
{

	/// Dummy sentinel value
	eSOCacheEvent_None = 0,

	/// We received a our first update from the GC and are subscribed
	eSOCacheEvent_Subscribed = 1,

	/// We lost connection to GC or GC notified us that we are no longer subscribed.
	/// Objects stay in the cache, but we no longer receive updates
	eSOCacheEvent_Unsubscribed = 2,

	/// We received a full update from the GC on a cache for which we were already subscribed.
	/// This can happen if connectivity is lost, and then restored before we realized it was lost.
	eSOCacheEvent_Resubscribed = 3,

	/// We received an incremental update from the GC about specific object(s) being
	/// added, updated, or removed from the cache
	eSOCacheEvent_Incremental = 4,

	/// A lister was added to the cache
	eSOCacheEvent_ListenerAdded = 5,

	/// A lister was removed from the cache
	eSOCacheEvent_ListenerRemoved = 6,
};

//----------------------------------------------------------------------------
// Purpose: Allow game components to register themselves to hear about inventory
//			changes when they are received from the server
//----------------------------------------------------------------------------
class ISharedObjectListener
{
public:

	/// Called when a new object is created in a cache we are currently subscribed to
	virtual void SOCreated( SOID_t owner, const CSharedObject *pObject, ESOCacheEvent eEvent ) = 0;

	/// Called when an object is updated in a cache we are currently subscribed to
	virtual void SOUpdated( SOID_t owner, const CSharedObject *pObject, ESOCacheEvent eEvent ) = 0;

	/// Called when an object is about to be deleted in a cache we are currently subscribed to.
	/// The object will have already been removed from the cache, but is still valid.
	virtual void SODestroyed( SOID_t owner, const CSharedObject *pObject, ESOCacheEvent eEvent ) = 0;

	/// Called to notify a listener that he is subscribed to the cache. A listener is guaranteed
	/// that it will not receive incremental updates (SOCreated, SOUpdated, SODestroyed) while not
	/// subscribed. However, note that it may be possible to receive an SOCacheSubscribed message
	/// while already subscribed. This can happen if the GC loses and restores connection, or
	/// otherwise decides that a full update is necessary.
	virtual void SOCacheSubscribed( SOID_t owner, CGCClientSharedObjectCache *pCache, ESOCacheEvent eEvent ) = 0;

	/// Called to notify a listener that he is no longer subscribed to the cache.
	virtual void SOCacheUnsubscribed( SOID_t owner, CGCClientSharedObjectCache *pCache, ESOCacheEvent eEvent ) = 0;
};


//----------------------------------------------------------------------------
// Purpose: The part of a shared object cache that handles all objects of a
//			single type.
//----------------------------------------------------------------------------
class CGCClientSharedObjectTypeCache : public CSharedObjectTypeCache
{
public:
	CGCClientSharedObjectTypeCache( int nTypeID );
	virtual ~CGCClientSharedObjectTypeCache();

	bool BParseCacheSubscribedMsg( const CMsgSOCacheSubscribed_SubscribedType & msg, CUtlVector<CSharedObject*> &vecCreatedObjects, CUtlVector<CSharedObject*> &vecUpdatedObjects, CUtlVector<CSharedObject*> &vecObjectsToDestroy );

	CSharedObject *BCreateFromMsg( const void *pvData, uint32 unSize, bool *bUpdatedExisting );
	bool BDestroyFromMsg( const void *pvData, uint32 unSize );
	bool BUpdateFromMsg( const void *pvData, uint32 unSize );

	void RemoveAllObjects( CUtlVector<CSharedObject*> &vecObjects );

private:
	uint32 m_nServiceID;
};


//----------------------------------------------------------------------------
// Purpose: A cache of a bunch of shared objects of different types. This class
//			is shared between clients, gameservers, and the GC and is
//			responsible for sending messages from the GC to cause object
//			creation/destruction/updating on the clients/gameservers.
//----------------------------------------------------------------------------
class CGCClientSharedObjectCache : public CSharedObjectCache
{
	friend class CGCSOUpdateMultipleJob;

public:
	CGCClientSharedObjectCache( const SOID_t &owner = SOID_t() );
	virtual ~CGCClientSharedObjectCache();

	/// Who owns this cache?
	virtual SOID_t GetOwner() const OVERRIDE { return m_owner; }

	/// Have we received at least one update from the GC?
	bool BIsInitialized() const { return m_bInitialized; }

	/// Are we currently subscribed to updates from the GC?
	bool BIsSubscribed() const { return m_bSubscribed; }

	CGCClientSharedObjectTypeCache *FindTypeCache( int nClassID ) { return (CGCClientSharedObjectTypeCache *)FindBaseTypeCache( nClassID ); }
	CGCClientSharedObjectTypeCache *CreateTypeCache( int nClassID ) { return (CGCClientSharedObjectTypeCache *)CreateBaseTypeCache( nClassID ); }

	bool BParseCacheSubscribedMsg( const CMsgSOCacheSubscribed & msg );
	void NotifyUnsubscribe();
	void NotifyResubscribedUpToDate();

	bool BCreateFromMsg( int nTypeID, const void *pvData, uint32 unSize );
	bool BDestroyFromMsg( int nTypeID, const void *pvData, uint32 unSize );
	bool BUpdateFromMsg( int nTypeID, const void *pvData, uint32 unSize );

private:
	virtual CSharedObjectTypeCache *AllocateTypeCache( int nClassID ) const OVERRIDE { return new CGCClientSharedObjectTypeCache( nClassID ); }
	CGCClientSharedObjectTypeCache *GetTypeCacheByIndex( int nIndex ) { return (CGCClientSharedObjectTypeCache *)CSharedObjectCache::GetTypeCacheByIndex( nIndex ); }

public:
	SOID_t m_owner;
	bool m_bInitialized;
	bool m_bSubscribed;
};



} // namespace GCSDK


#endif //GCCLIENT_SHAREDOBJECTCACHE_H
