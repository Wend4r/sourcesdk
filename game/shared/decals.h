//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Decal groups ("scripts/decalgroups.vdata") and the game system that
//          loads them.
//
//=============================================================================//

#ifndef DECALS_H
#define DECALS_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "vdatafilemanager.h"

#include <materialsystem2/imaterial2.h>
#include <resourcesystem/resourcehandle.h>
#include <tier0/globalsymbol.h>
#include <tier0/utlvector.h>

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// One decal of a group, 32 bytes (schema)
//-----------------------------------------------------------------------------
schema struct DecalGroupOption_t
{
public:
	CStrongHandleCopyable< InfoForResourceTypeIMaterial2 > m_hMaterial;
	CGlobalSymbol m_sSequenceName;
	float m_flProbability;
	bool m_bEnableAngleBetweenNormalAndGravityRange;
	float m_flMinAngleBetweenNormalAndGravity;
	float m_flMaxAngleBetweenNormalAndGravity;
};

//-----------------------------------------------------------------------------
// A decal group, an entry of "scripts/decalgroups.vdata", 32 bytes (schema)
//-----------------------------------------------------------------------------
schema class CDecalGroupVData
{
public:
	CUtlVector< DecalGroupOption_t > m_vecOptions;
	float m_flTotalProbability;
};

//-----------------------------------------------------------------------------
// The decal group loader (RTTI name); it overrides nothing of CVDataFileManager
//-----------------------------------------------------------------------------
class CDecalVDataFileManager : public CVDataFileManager< CDecalGroupVData >
{
};

//-----------------------------------------------------------------------------
// Static instance ("DecalEmitterSystem"), 144 bytes on both platforms. Everything is the vdata
// loader.
// It adds no virtual methods to IGameSystem.
//-----------------------------------------------------------------------------
class CDecalEmitterSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	CDecalEmitterSystem();
	virtual ~CDecalEmitterSystem();

	// m_DecalGroups.RemoveAllFiles()
	GS_EVENT( GameShutdown );
	// Adds "scripts/decalgroups.vdata" to the manifest; its load callback passes the file to
	// m_DecalGroups.AddVDataFile
	GS_EVENT( BuildGameSessionManifest );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds: the decal groups by name
	CDecalVDataFileManager m_DecalGroups;
};

#include "schemacompiler/schemaoff.h"

#endif // DECALS_H
