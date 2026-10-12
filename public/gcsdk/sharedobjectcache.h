//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for objects that are kept in synch between client and server
//
//=============================================================================

#ifndef SHAREDOBJECTCACHE_H
#define SHAREDOBJECTCACHE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlcommon.h"
#include "tier1/utlhashmaplarge.h"

#include "soid.h"
#include "sharedobject.h"

namespace GCSDK
{

//----------------------------------------------------------------------------
// Purpose: The part of a shared object cache that handles all objects of a 
//			single type.
//----------------------------------------------------------------------------
class CSharedObjectTypeCache
{
public:
	CSharedObjectTypeCache( int nTypeID );
	virtual ~CSharedObjectTypeCache();

	int GetTypeID() const { return m_nTypeID; }
	uint32 GetCount() const { return m_vecObjects.Count(); }
	CSharedObject *GetObject( uint32 nObj ) { return m_vecObjects[nObj]; }
	const CSharedObject *GetObject( uint32 nObj ) const { return m_vecObjects[nObj]; }

	virtual bool AddObject( CSharedObject *pObject );
	virtual bool AddObjectClean( CSharedObject *pObject );
	virtual CSharedObject *RemoveObject( const CSharedObject & soIndex );
	virtual void RemoveAllObjectsWithoutDeleting();
	virtual void EnsureCapacity( uint32 nItems );

	CSharedObject *RemoveObjectByIndex( uint32 nObj );
	void DestroyAllObjects();

	CSharedObject *FindSharedObject( const CSharedObject & soIndex );

	virtual void Dump() const;

#ifdef DBGFLAG_VALIDATE
	virtual void Validate( CValidator &validator, const char *pchName );
#endif

public:
	struct SharedObjectAndIndex_t
	{
		CUtlVector< int > m_vecIndices;
		CUtlVector< CSharedObject * > m_vecObjects;
	};

private:
	int FindSharedObjectIndex( const CSharedObject & soIndex ) const;
	void AddObjectInternal( CSharedObject *pObject );

	CSharedObjectVec m_vecObjects;
	CUtlHashMap< int, SharedObjectAndIndex_t, CDefEquals< int >, DefaultHashFunctor< int >, int > m_mapObjects;
	int m_nTypeID;
};


//----------------------------------------------------------------------------
// Purpose: A cache of a bunch of shared objects of different types. This class
//			is shared between clients, gameservers, and the GC and is 
//			responsible for sending messages from the GC to cause object 
//			creation/destruction/updating on the clients/gameservers.
//----------------------------------------------------------------------------
class CSharedObjectCache
{
public:
	CSharedObjectCache();
	virtual ~CSharedObjectCache();

	virtual SOID_t GetOwner() const = 0;

	virtual bool AddObject( CSharedObject *pSharedObject );
	virtual bool AddObjectClean( CSharedObject *pSharedObject );
	virtual CSharedObject *RemoveObject( const CSharedObject & soIndex );
	virtual bool RemoveAllObjectsWithoutDeleting();

	bool BDestroyObject( const CSharedObject & soIndex, bool bRemoveFromDatabase );

	//called to find the type cache for the specified class ID. This will return NULL if one does not exist
	CSharedObjectTypeCache *FindBaseTypeCache( int nClassID ) const;
	//called to create the specified class ID. If one exists, this is the same as find, otherwise one will be constructed
	CSharedObjectTypeCache *CreateBaseTypeCache( int nClassID );

	CSharedObject *FindSharedObject( const CSharedObject & soIndex );

	template < class T >
	T *FindTypedSharedObject( const CSharedObject &soIndex )
	{
		return assert_cast<T *>( FindSharedObject( soIndex ) );
	}

	// returns various singleton objects
	template< typename SOClass_t >
	SOClass_t *GetSingleton() const
	{
		CSharedObjectTypeCache *pTypeCache = FindBaseTypeCache( SOClass_t::k_nTypeID );
		if ( pTypeCache )
		{
			AssertMsg2( pTypeCache->GetCount() == 0 || pTypeCache->GetCount() == 1, "GetSingleton() called on type %u that has invalid number of items %u.", SOClass_t::k_nTypeID, pTypeCache->GetCount() );

			if ( pTypeCache->GetCount() == 1 )
			{
				return (SOClass_t *)pTypeCache->GetObject( 0 );
			}
		}
		return NULL;
	}

	void SetVersion( uint64 ulVersion ) { m_ulVersion = ulVersion; }
	uint64 GetVersion() const { return m_ulVersion; }
	virtual void MarkDirty() {}

	virtual void Dump() const;
#ifdef DBGFLAG_VALIDATE
	virtual void Validate( CValidator &validator, const char *pchName );
#endif

protected:
	virtual CSharedObjectTypeCache *AllocateTypeCache( int nClassID ) const = 0;
	CSharedObjectTypeCache *GetTypeCacheByIndex( int nIndex ) { return ( nIndex >= 0 && nIndex < m_vecTypeCaches.Count() ) ? m_vecTypeCaches[ nIndex ] : NULL; }
	int GetTypeCacheCount() const { return m_vecTypeCaches.Count(); }

	int FirstTypeCacheIndex() { return 0; }
	int NextTypeCacheIndex( int iCurrent ) { return iCurrent + 1; }
	int InvalidTypeCacheIndex() { return m_vecTypeCaches.Count(); }

	uint64 m_ulVersion;
private:
	CUtlVector< CSharedObjectTypeCache * > m_vecTypeCaches;
};



} // namespace GCSDK


#endif //SHAREDOBJECTCACHE_H
