#ifndef NETWORKVAR_H
#define NETWORKVAR_H

#pragma once

#include "tier1/utlvector.h"
#include "tier1/utldelegateimpl.h"
#include "entity2/entityidentity.h"

#include "fieldpath.h"

template< class T, bool CHAINED = false, typename I = int, typename A = CUtlVectorMemory< T, I > >
class CNetworkUtlVectorBase : public CUtlVectorBase< T, I, A >
{
	typedef CUtlVectorBase< T, I, A > BaseClass;
public:
	using BaseClass::BaseClass;
};

template < class T, typename I, typename A >
class CNetworkUtlVectorBase< T, true, I, A > : public CUtlVectorBase< T, I, A >
{
public:
	CEntityInstance *GetOwnerEntity() const { return m_pOwnerEntity; }
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

template < typename T, typename I = int >
class CUtlVectorEmbeddedNetworkVar : public CNetworkUtlVectorBase< T, false, I >
{
public:
	CEntityInstance *GetOwnerEntity() const { return m_pOwnerEntity; }
	const CFieldPath &GetPathToVector() const { return m_PathToVector; }

public:
	CEntityOwnerPtr m_pOwnerEntity;
	CUtlDelegate< void () > *m_pArraySizeChangedDelegate;
	CFieldPath m_PathToVector;
	CUtlVector< ChangeAccessorFieldPathIndex_t > m_ElementPathIndices;
	byte m_nNetworkingFlags;
	byte m_nResolveFlags;
};

#endif // !defined( NETWORKVAR_H )
