#ifndef CENTITYLUMP_H
#define CENTITYLUMP_H

#ifdef _WIN32
	#pragma once
#endif

#include "resourcefile/resourcehandle.h"

#include <entity2/entitykeyvalues.h>
#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <tier1/utlvector.h>

struct PermEntityLumpData_t;

class CEntityLump;

class InfoForResourceTypeCEntityLump
{
public:
	using RuntimeClass_t = CEntityLump;
};

using HEntityLump = CWeakHandle<InfoForResourceTypeCEntityLump>;
using HEntityLumpStrong = CStrongHandleCopyable<InfoForResourceTypeCEntityLump>;

class CEntityLump
{
public:
	CEntityLump( const PermEntityLumpData_t* pData );
	~CEntityLump();

	const char* GetName() const { return m_name.Get(); }

	int GetNumChildLumps() const { return m_childLumps.Count(); }
	HEntityLump GetChildLump( int nChild ) const
	{
		return HEntityLump::FromUntypedHandleUnchecked( m_childLumps[nChild] );
	}

	int GetNumEntities() const { return m_entityKeyValues.Count(); }
	const CEntityKeyValues* GetEntityKeyValues( int nEntity ) const { return m_entityKeyValues[nEntity]; }
	const CUtlVector<const CEntityKeyValues*>* GetEntityKeyValues() const
	{
		return reinterpret_cast<const CUtlVector<const CEntityKeyValues*>*>( &m_entityKeyValues );
	}

public:
	CUtlString m_name;
	CUtlVector<HEntityLumpStrong> m_childLumps;
	CKV3Arena m_keyValueArena;
	CUtlVector<CEntityKeyValues*> m_entityKeyValues;
};

#endif // CENTITYLUMP_H
