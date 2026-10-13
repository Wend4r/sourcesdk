//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Rubikon name indices, intersection dictionary and intersection controller
//
//===========================================================================//

#ifndef INTERSECTION_CONTROLLER_INTERFACE_H
#define INTERSECTION_CONTROLLER_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstringtoken.h"
#include "rubikon/intersection.h"

class IVPhysics2World;
struct RnCollisionAttr_t;
struct RnQueryShapeAttr_t;

// Up to 64 names registered by string token, each owning one bit of a 64-bit mask
abstract_class IPhysNameIndex
{
public:
	// Returns the index of the name, registering it when it is not known yet
	virtual int AcquireName( const CUtlStringToken &token, const char *pszName ) = 0;

	// Acquires every name of a list separated by whitespace, ',', ';' or '|' and returns the mask of their bits
	virtual uint64 unk001( const char *pszNames ) = 0;

	virtual bool AcquirePreindexedName( const CUtlStringToken &token, const char *pszName, int nIndex ) = 0;

	virtual int unk003() const = 0;

	// Bits of all registered names
	virtual uint64 unk004() const = 0;

	// Index of a registered name or -1
	virtual int unk005( CUtlStringToken token ) const = 0;

	// Mask of the bits of registered names, unknown names add nothing.
	// Three overloads, the second takes a span of tokens by value which C++17 cannot declare
	virtual uint64 unk006( CUtlStringToken token ) const = 0;
	virtual uint64 unk006( void *p ) const = 0;
	virtual uint64 unk006( const char *pszNames ) const = 0;

	virtual CUtlStringToken GetName( int nIndex ) const = 0;
	virtual const char *GetNameAsString( int nIndex ) const = 0;

	// Whether every bit of the mask belongs to a registered name
	virtual bool unk011( uint64 nMask ) const = 0;

	virtual bool unk012( int nIndex ) const = 0;

	// Name of the index itself, printed by IPhysIntersectionDictionary::DumpStatus
	virtual const char *unk013() const = 0;

	virtual ~IPhysNameIndex() {}
};

abstract_class IPhysIntersectionDictionary
{
public:
	// The interaction layers ("interacts as" and "interacts with" attributes)
	virtual const IPhysNameIndex *GetInteractionLayers() const = 0;
	virtual IPhysNameIndex *GetInteractionLayers() = 0;

	virtual const IPhysNameIndex *unk002() const = 0;
	virtual IPhysNameIndex *unk002() = 0;

	virtual bool unk004() const = 0;

	// Looks up a 64-byte record by its key
	virtual const void *unk005( uint32 nKey ) const = 0;

	virtual void unk006( void *p ) = 0;

	// Takes two spans of names by value, which C++17 cannot declare
	virtual void unk007( void *p ) = 0;

	// The collision groups ("collision group" attribute)
	virtual const IPhysNameIndex *GetCollisionGroups() const = 0;
	virtual IPhysNameIndex *GetCollisionGroups() = 0;

	// Sets m_nInteractsAs and m_nInteractsWith to the mask returned by unk011 and the collision group to 4
	virtual void unk010( RnCollisionAttr_t &attr ) const = 0;

	virtual uint64 unk011() const = 0;
	virtual void unk012( uint64 nMask ) = 0;
	virtual void unk013( uint64 nMask ) = 0;

	// 0 is GetInteractionLayers, 1 is unk002, 2 is GetCollisionGroups
	virtual const IPhysNameIndex *unk014( int nIndex ) const = 0;
	virtual IPhysNameIndex *unk014( int nIndex ) = 0;

	// Keyed list of interaction masks
	virtual int unk016( int nKey ) const = 0;
	virtual uint64 unk017( int nIndex ) const = 0;
	virtual void unk018( int nIndex, uint64 nMask ) = 0;

	// Takes the key and a span of tokens by value, which C++17 cannot declare
	virtual void unk019( void *p ) = 0;

	virtual void DumpStatus() const = 0;

	virtual ~IPhysIntersectionDictionary() {}
};

abstract_class IPhysIntersectionController
{
public:
	virtual void SetCollisionGroupPairFlags( CollisionGroupIndex nGroup1, CollisionGroupIndex nGroup2, IntersectionLayerPairFlags nFlags ) = 0;
	virtual void EnableCollisionGroupPairFlags( CollisionGroupIndex nGroup1, CollisionGroupIndex nGroup2, IntersectionLayerPairFlags nFlags ) = 0;
	virtual void DisableCollisionGroupPairFlags( CollisionGroupIndex nGroup1, CollisionGroupIndex nGroup2, IntersectionLayerPairFlags nFlags ) = 0;
	virtual void CopyCollisionGroupFlagsAllPairs( CollisionGroupIndex nDestGroup, CollisionGroupIndex nSourceGroup ) = 0;

	virtual IntersectionLayerPairFlags TestSimulation( const RnCollisionAttr_t &attr1, const RnCollisionAttr_t &attr2 ) const = 0;
	virtual bool TestQuery( const RnQueryShapeAttr_t &queryAttr, const RnCollisionAttr_t &collisionAttr, IVPhysics2World *pWorld ) const = 0;

	virtual IntersectionLayerPairFlags GetCollisionGroupPairFlags( CollisionGroupIndex nGroup1, CollisionGroupIndex nGroup2 ) = 0;

	// Empty in release builds
	virtual void DumpStatus() const = 0;

	virtual ~IPhysIntersectionController() {}
};

#endif // INTERSECTION_CONTROLLER_INTERFACE_H
