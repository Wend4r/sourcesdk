#ifndef NETWORKSYSTEM_NETWORKVAR_H
#define NETWORKSYSTEM_NETWORKVAR_H

#pragma once

#include "tier1/utlvector.h"
#include "tier1/utldelegateimpl.h"
#include "entity2/entityidentity.h"

#include "fieldpath.h"

// Memory policy the engine threads through every networked vector. Only the form it is
// instantiated in is known:
//
//		CUtlVector< T, int, CNetworkUtlVector_MemoryType< T, -1 > >
//
// The internals below are reconstructed - the one confirmed part is that GROW_SIZE is threaded down
// from CNetworkUtlVectorBase. Every networked-vector field observed so far passes -1.
template < class T, int GROW_SIZE = -1 >
class CNetworkUtlVector_MemoryType : public CUtlVectorMemory_Growable< T, int, GROW_SIZE >
{
};

// Schema field type of a networked CUtlVector. The schema system records it as, e.g.
//
//		CNetworkUtlVectorBase< int, NetworkVar_m_nBodyGroupChoices, -1, int >
//		CNetworkUtlVectorBase< CHandle< CBasePlayerPawn >, NetworkVar_m_aPlayers, -1, int >
//
// Changer is the per-field tag class the CNetworkUtlVector macro generates, exactly as for
// CNetworkVarBase. The client spells this template C_NetworkUtlVectorBase.
template < class T, class Changer, int GROW_SIZE = -1, typename I = int >
class CNetworkUtlVectorBase : public CUtlVectorBase< T, I, CNetworkUtlVector_MemoryType< T, GROW_SIZE > >
{
	typedef CUtlVectorBase< T, I, CNetworkUtlVector_MemoryType< T, GROW_SIZE > > BaseClass;

public:
	using BaseClass::BaseClass;
};

// Placeholder name - the variant used when the path to the vector runs through one or more
// pointers, so it has to carry its own owner and field path instead of a flat offset. 
// The fields are reconstructed; the engine's own name for this template is not known yet, so treat the name here as
// a placeholder.
template < class T, class Changer, int GROW_SIZE = -1, typename I = int >
class CNetworkUtlVectorBaseChained : public CUtlVectorBase< T, I, CNetworkUtlVector_MemoryType< T, GROW_SIZE > >
{
public:
	CEntityInstance *GetOwnerEntity() const { return m_pOwnerEntity.GetObject(); }
	const CFieldPath &GetPathToVector() const { return m_PathToVector; }

public:
	CEntityOwnerPtr m_pOwnerEntity;
	CFieldPath m_PathToVector;
	CUtlVector< ChangeAccessorFieldPathIndex_t > m_ElementPathIndices;
	byte m_bNetworkingEnabled;
	byte m_pad;
	byte m_bResolved;
	byte m_bHasFieldPath;
};

// Same, for a vector of embedded (schema struct) elements, e.g.
//
//		CUtlVectorEmbeddedNetworkVar< EntityRenderAttribute_t, NetworkVar_m_vecRenderAttributes, -1, int >
//
// The client spells this one C_UtlVectorEmbeddedNetworkVar.
template < class T, class Changer, int GROW_SIZE = -1, typename I = int >
class CUtlVectorEmbeddedNetworkVar : public CNetworkUtlVectorBase< T, Changer, GROW_SIZE, I >
{
public:
	CEntityInstance *GetOwnerEntity() const { return m_pOwnerEntity.GetObject(); }
	const CFieldPath &GetPathToVector() const { return m_PathToVector; }

public:
	CEntityOwnerPtr m_pOwnerEntity;
	CUtlDelegate< void () > *m_pArraySizeChangedDelegate;
	CFieldPath m_PathToVector;
	CUtlVector< ChangeAccessorFieldPathIndex_t > m_ElementPathIndices;
	byte m_nNetworkingFlags;
	byte m_nResolveFlags;
};

#endif // NETWORKSYSTEM_NETWORKVAR_H
