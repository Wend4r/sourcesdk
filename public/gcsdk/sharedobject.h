//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Base class for objects that are kept in synch between client and server
//
//=============================================================================

#ifndef SHAREDOBJECT_H
#define SHAREDOBJECT_H
#ifdef _WIN32
#pragma once
#endif

// ENABLE_SO_OVERWRITE_PARANOIA can be set to either 0 or 1. If enabled, it will add
// extra fields to every CSharedObject instance to try and detect overwrites at the
// cost of additional runtime memory.
#define ENABLE_SO_OVERWRITE_PARANOIA				0

// ENABLE_SO_CONSTRUCT_DESTRUCT_PARANOIA can be set to either 0 or 1. If enabled, it
// will add extra fields to every CSharedObject instance to try and detect issues with
// constructions/destruction (ie., double-deletes, etc.), including reference counting.
#define ENABLE_SO_CONSTRUCT_DESTRUCT_PARANOIA		0

#include "tier0/utlbuffer.h"
#include "tier0/utlsortvector.h"
#include "soid.h"

namespace google { namespace protobuf { class Message; } }

#include "tier0/memdbgon.h"

namespace GCSDK
{

class CSQLAccess;
class CSharedObject;
typedef CSharedObject *(*SOCreationFunc_t)( );
class CSharedObjectCache;


//----------------------------------------------------------------------------
// Purpose: Abstract base class for objects that are shared between the GC and
//			a gameserver/client. These can also be stored in the database.
//----------------------------------------------------------------------------
class CSharedObject
{
	friend class CGCSharedObjectCache;
	friend class CSharedObjectCache;
public:

	virtual ~CSharedObject() {}

	virtual int GetTypeID() const = 0;
	virtual bool BParseFromMessage( SOID_t ID, const CUtlBuffer & buffer ) = 0;
	virtual bool BUpdateFromNetwork( const CSharedObject & objUpdate ) = 0;
	virtual bool BIsKeyLess( const CSharedObject & soRHS ) const = 0;
	virtual void Copy( const CSharedObject & soRHS ) = 0;
	virtual void Dump() const = 0;

	// Called by CSharedObjectTypeCache::FindSharedObject(): looks in vecObjects for the object
	// whose key equals this one's and returns the entry of vecIndices beside it, which is the
	// object's index in the type cache, or -1. CS:GO did this search in the type cache itself.
	virtual int FindIndexInVector( const CUtlVector< CSharedObject * > &vecObjects, const CUtlVector< int > &vecIndices ) const = 0;

	virtual bool BAddToMessage( std::string *pBuffer, bool bUnk ) const = 0;
	virtual bool BAddDestroyToMessage( std::string *pBuffer ) const = 0;

	bool BIsKeyEqual( const CSharedObject & soRHS ) const;

	static void RegisterFactory( int nTypeID, SOCreationFunc_t fnFactory, uint32 unFlags, const char *pchClassName, const char* pszBuildCacheName, const char* pszCreateName, const char* pszUpdateName );
	static CSharedObject *Create( int nTypeID );
	static uint32 GetTypeFlags( int nTypeID );
	static const char *PchClassName( int nTypeID );
	static const char *PchClassBuildCacheNodeName( int nTypeID );
	static const char *PchClassCreateNodeName( int nTypeID );
	static const char *PchClassUpdateNodeName( int nTypeID );


private:
#if ENABLE_SO_CONSTRUCT_DESTRUCT_PARANOIA
	enum { kSharedObject_UnassignedType = -999 };
#endif // ENABLE_SO_CONSTRUCT_DESTRUCT_PARANOIA

	struct SharedObjectInfo_t
	{
		int m_nTypeID;
		uint32 m_unFlags;
		SOCreationFunc_t m_pFactoryFunction;
		const char *m_pchClassName;
		const char *m_pchBuildCacheSubNodeName;
		const char *m_pchUpdateNodeName;
		const char *m_pchCreateNodeName;
	};

	class CCompareSharedObject
	{
	public:
		bool Less( const SharedObjectInfo_t &lhs, const SharedObjectInfo_t &rhs, void * ) { return lhs.m_nTypeID < rhs.m_nTypeID; }
	};

	static CUtlSortVector< SharedObjectInfo_t, CCompareSharedObject > sm_vecFactories;

public:
	static const CUtlSortVector< SharedObjectInfo_t, CCompareSharedObject > & GetFactories() { return sm_vecFactories; }
};

typedef CUtlVectorFixedGrowable<CSharedObject *, 1> CSharedObjectVec;


//----------------------------------------------------------------------------
// Purpose: Templatized function to use as a factory method for 
//			CSharedObject subclasses
//----------------------------------------------------------------------------
template<typename SharedObjectSubclass_t>
CSharedObject *CreateSharedObjectSubclass()
{
	return new SharedObjectSubclass_t();
}

//internal utility to expand the provided shared object class name into all the names needed for the node
#define REG_SHARED_OBJECT_NAMES_INTERNAL( name )		#name, "BuildCacheSubscribed(" #name ")", "Create(" #name ")", "Update(" #name ")"

#define REG_SHARED_OBJECT_SUBCLASS( derivedClass ) GCSDK::CSharedObject::RegisterFactory( derivedClass::k_nTypeID, GCSDK::CreateSharedObjectSubclass<derivedClass>, 0, REG_SHARED_OBJECT_NAMES_INTERNAL( derivedClass ) )

} // namespace GCSDK


#include "tier0/memdbgoff.h"

#endif //SHAREDOBJECT_H
