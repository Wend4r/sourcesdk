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
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"
#include "rubikon/intersection.h"

class IVPhysics2World;
struct RnCollisionAttr_t;
struct RnQueryShapeAttr_t;

// A detail layer of the collision detail layers resource, keyed by the token of its name
struct CollisionDetailLayerInfo_t
{
	struct Name_t
	{
		CUtlStringToken m_nNameToken;
		CUtlString m_sNameString;
	};

	CUtlString m_sDescription;
	CUtlString m_sFriendlyName;
	bool m_bIsQueryOnly;
	CUtlString m_sParentDetailLayer;
	CUtlVector< Name_t > m_vecSubtreeDetailLayers;
	bool m_bNotPickable;
};

// Stand-in for std::span< T > with a dynamic extent, which these interfaces take by value.
// It keeps the pointer and the count in the same order and size, so passed by value it goes in
// two registers on System V and by a pointer to a temporary copy on Windows, like the span
template< class T >
struct PhysSpan_t
{
	T *m_pData;
	size_t m_nCount;
};

// Up to 64 names registered by string token, each owning one bit of a 64-bit mask
abstract_class IPhysNameIndex
{
public:
	// Returns the index of the name, registering it when it is not known yet
	virtual int AcquireName( const CUtlStringToken &token, const char *pszName ) = 0;

	// Acquires every name of a list separated by whitespace, ',', ';' or '|' and returns the mask of their bits
	virtual uint64 AcquireNames( const char *pszNames ) = 0;

	virtual bool AcquirePreindexedName( const CUtlStringToken &token, const char *pszName, int nIndex ) = 0;

	// Number of names the index can hold, the bound of the indices
	virtual int GetNameCapacity() const = 0;

	// Bits of all registered names, printed as "All" by IPhysIntersectionDictionary::DumpStatus
	virtual uint64 GetAllNamesMask() const = 0;

	// Index of a registered name or -1
	virtual int FindName( CUtlStringToken token ) const = 0;

	// Mask of the bits of registered names, unknown names add nothing
	virtual uint64 GetMask( CUtlStringToken token ) const = 0;
	virtual uint64 GetMask( PhysSpan_t< const CUtlStringToken > tokens ) const = 0;
	virtual uint64 GetMask( const char *pszNames ) const = 0;

	virtual CUtlStringToken GetName( int nIndex ) const = 0;
	virtual const char *GetNameAsString( int nIndex ) const = 0;

	// Whether every bit of the mask belongs to a registered name
	virtual bool IsValidMask( uint64 nMask ) const = 0;

	// Whether the index is in range and belongs to a registered name
	virtual bool IsValidIndex( int nIndex ) const = 0;

	// Name of the index itself ("Interaction Layers", "Detail Layers", "Collision Groups"),
	// printed by its log messages and by IPhysIntersectionDictionary::DumpStatus
	virtual const char *GetDebugName() const = 0;

	virtual ~IPhysNameIndex() {}
};

abstract_class IPhysIntersectionDictionary
{
public:
	// The interaction layers ("interacts as" and "interacts with" attributes)
	virtual const IPhysNameIndex *GetInteractionLayers() const = 0;
	virtual IPhysNameIndex *GetInteractionLayers() = 0;

	// The detail layers ("Detail Layers", 16 names)
	virtual const IPhysNameIndex *GetDetailLayers() const = 0;
	virtual IPhysNameIndex *GetDetailLayers() = 0;

	// The detail layer infos are loaded from scripts/collision_detail_layers.vdata, keyed by the token of the lowercase name
	virtual bool HasDetailLayerInfos() const = 0;
	virtual const CollisionDetailLayerInfo_t *FindDetailLayerInfo( uint32 nToken ) const = 0;

	// Fills the vector with all detail layer infos sorted by their friendly names
	virtual void GetDetailLayerInfos( CUtlVector< const CollisionDetailLayerInfo_t * > &infos ) = 0;

	// Sets m_nDetailLayerMask and m_nDetailLayerMaskType of the attributes from the subtree layers of the named
	// detail layer infos: with included layers the type is 1 and the excluded layers are cleared from the included ones,
	// otherwise the type is 0 and the mask holds the excluded layers
	virtual void GetDetailLayerMask( PhysSpan_t< const char * const > includedLayers, PhysSpan_t< const char * const > excludedLayers, RnCollisionAttr_t &attr ) = 0;

	// The collision groups ("collision group" attribute)
	virtual const IPhysNameIndex *GetCollisionGroups() const = 0;
	virtual IPhysNameIndex *GetCollisionGroups() = 0;

	// Sets m_nInteractsAs and m_nInteractsWith to GetDefaultInteractionMask and the collision group to 4
	virtual void InitDefaultCollisionAttr( RnCollisionAttr_t &attr ) const = 0;

	// Mask of the default interaction mask entry (the "CONTENTS_SOLID" one by default),
	// used for shapes with the default collision group and no "interacts as" layers
	virtual uint64 GetDefaultInteractionMask() const = 0;

	// ORs into the interaction layers a shape interacts-as override replaces in the attributes
	virtual void AddOverrideMask( uint64 nMask ) = 0;

	// Interaction layers that make a shape interacts-as override replace the whole attributes, clearing m_nInteractsWith and m_nInteractsExclude
	virtual void SetExclusiveOverrideMask( uint64 nMask ) = 0;

	// 0 is GetInteractionLayers, 1 is GetDetailLayers, 2 is GetCollisionGroups
	virtual const IPhysNameIndex *GetNameIndex( int nIndex ) const = 0;
	virtual IPhysNameIndex *GetNameIndex( int nIndex ) = 0;

	// Named interaction masks keyed by a string token ("CONTENTS_SOLID", "CONTENTS_SOLID_NO_BLOCK_LOS", ...)
	virtual int FindInteractionMask( int nKey ) const = 0;
	virtual uint64 GetInteractionMask( int nIndex ) const = 0;
	virtual void SetInteractionMask( int nIndex, uint64 nMask ) = 0;

	// Sets the mask of the key to the mask of the interaction layer tokens, adding the key when it is missing, and returns its index
	virtual int AddInteractionMask( const int &nKey, PhysSpan_t< const CUtlStringToken > tokens ) = 0;

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
