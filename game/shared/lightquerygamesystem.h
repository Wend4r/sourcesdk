//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that answers lighting queries on the server (the lights of the map
//          and the light probe volumes).
//
//=============================================================================//

#ifndef LIGHTQUERYGAMESYSTEM_H
#define LIGHTQUERYGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier0/bitvec.h"
#include "tier0/utlvector.h"
#include "mathlib/vector4d.h"
#include "mathlib/lightdesc.h"

class CLightComponent;
class CBaseLightProbeVolume;

//-----------------------------------------------------------------------------
// Registered as "LightQueryGameSystem" (reallocating factory), 168 bytes on both platforms.
// The members are named by what the code does.
//-----------------------------------------------------------------------------
class CLightQueryGameSystem : public IGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Clears m_vEnvironmentLightSH and reads m_bLightmapUVQuery from the game info
	GS_EVENT( GameInit );
	// The next five share one empty body
	GS_EVENT( GameShutdown );
	GS_EVENT( GameActivate );
	GS_EVENT( GameDeactivate );
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupUnload );
	// Both run OnPostSimulate ("CLightQueryGameSystem::OnPostSimulate"): updates the dirty lights
	// and queries the lighting at the local player's view (when the query convar is on)
	GS_EVENT( ServerPostSimulate );
	GS_EVENT( ClientPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CLightQueryGameSystem();

public:
	// The light components of the enabled light entities (CLightEntity and its subclasses add and
	// remove theirs); the three containers below are parallel to it
	CUtlVector< CLightComponent * > m_Lights;
	// The scene light description of each light, rebuilt from the component when it is dirty
	CUtlVector< LightDesc_t > m_LightDescs;
	// One bit per light: its description must be rebuilt before the next query
	CLargeVarBitVec m_DirtyLights;
	// The light probe volumes of CEnvLightProbeVolume / CEnvCombinedLightProbeVolume (their
	// embedded CBaseLightProbeVolume)
	CUtlVector< CBaseLightProbeVolume * > m_LightProbeVolumes;
	// Named by what it does: the lighting light_environment adds (sky color and intensity, ambient
	// bounce), 12 floats the light component writes when its description is rebuilt
	Vector4D m_vEnvironmentLightSH[ 3 ];
	// GetGameInfoBool( "Engine2/LightmapUVQuery", true ), read at GameInit
	bool m_bLightmapUVQuery;
};

#endif // LIGHTQUERYGAMESYSTEM_H
