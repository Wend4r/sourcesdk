//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The native side of the spawn group entity filters that scripts register.
//
//=============================================================================//

#ifndef VSCRIPTGAMESYSTEM_H
#define VSCRIPTGAMESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "gameentitysystem.h"

//-----------------------------------------------------------------------------
// The script function RegisterSpawnGroupFilterProxy( name ) allocates one and registers it under
// that name (SPAWN_GROUP_ENTITY_FILTER_FALLBACK); RemoveSpawnGroupFilterProxy( name ) removes the
// filter. FilterSpawnGroupEntities is empty in the game, so a spawn group that names the filter
// spawns no entity. 8 bytes.
//-----------------------------------------------------------------------------
class CScriptSpawnGroupEntityFilterProxy : public ISpawnGroupEntityFilter
{
public:
	void FilterSpawnGroupEntities( SpawnGroupHandle_t hSpawnGroup, bool bIsPrecaching, const CUtlVector< const CEntityKeyValues * > *pInKeyValues, CKV3Arena *pArena, CUtlVector< const CEntityKeyValues * > &outKeyValues ) override;
};

#endif // VSCRIPTGAMESYSTEM_H
