//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The spawn group entity filters of shared environment maps.
//
//=============================================================================//

#ifndef SHAREDENVIRONMENTENTITYFILTER_H
#define SHAREDENVIRONMENTENTITYFILTER_H

#ifdef _WIN32
#pragma once
#endif

#include "gameentitysystem.h"

//-----------------------------------------------------------------------------
// Registered as "shared_environment" (SPAWN_GROUP_ENTITY_FILTER_FALLBACK); a static object.
// Passes worldspawn, light_environment, env_sky, env_tonemap_controller and
// env_volumetric_fog_controller, each through FilterEntityKeyValues, and warns about the other
// classes. A worldspawn whose "mapUsageType" is not "sharedenvironment" empties the output. 8 bytes.
//-----------------------------------------------------------------------------
class CSharedEnvironmentEntityFilter : public ISpawnGroupEntityFilter
{
public:
	void FilterSpawnGroupEntities( SpawnGroupHandle_t hSpawnGroup, bool bIsPrecaching, const CUtlVector< const CEntityKeyValues * > *pInKeyValues, CKV3Arena *pArena, CUtlVector< const CEntityKeyValues * > &outKeyValues ) override;

	// Named by what it does: the key values to spawn for one passed entity; returns pKeyValues
	// here. A copy an override makes is allocated from pArena (from the arena of pKeyValues when
	// it is null).
	virtual const CEntityKeyValues *FilterEntityKeyValues( const CEntityKeyValues *pKeyValues, CKV3Arena *pArena );
};

//-----------------------------------------------------------------------------
// Registered as "shared_environment_preview_lighting" (SPAWN_GROUP_ENTITY_FILTER_FALLBACK); a
// static object. 8 bytes.
//-----------------------------------------------------------------------------
class CSharedEnvironmentEntityFilter_PreviewLighting : public CSharedEnvironmentEntityFilter
{
public:
	// A light_environment with "directlight" 1 is replaced by a copy with "directlight" 2
	const CEntityKeyValues *FilterEntityKeyValues( const CEntityKeyValues *pKeyValues, CKV3Arena *pArena ) override;
};

#endif // SHAREDENVIRONMENTENTITYFILTER_H
