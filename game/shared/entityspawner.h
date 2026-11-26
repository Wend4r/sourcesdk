//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Helpers that spawn an entity of a given class
//
//=============================================================================//

#ifndef ENTITYSPAWNER_H
#define ENTITYSPAWNER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "mathlib/mathlib.h"
#include "entitytypes.h"
#include "spawngrouptypes.h"

class CEntityClass;
class CEntityKeyValues;
class CKV3Arena;
template < class T > class CEntitySpawnerContext;

//-----------------------------------------------------------------------------
// The base of the helpers that spawn an entity of class T: the key values, the class and the
// spawn group of the next entity. 128 bytes on Linux (the class below starts its members at
// 120), 144 on Windows. Two slots on both platforms, no virtual destructor.
// CEntitySpawner< T > derives from it and overrides StartNewEntity (the server has one for
// CVoteController, CSceneEntity and CInstancedSceneEntity); CEntitySpawnerAsyncBase< T > as
// well.
//-----------------------------------------------------------------------------
template < class T >
abstract_class CEntitySpawnerBase
{
public:
	// Creates the key values of the next entity (a new CEntityKeyValues, add-referenced) and
	// applies the key values of every context to them
	virtual void StartNewEntity() = 0;
	virtual void OnEntitySpawned( T *pEntity ) = 0;

public:
	// The key values of the next entity, created by StartNewEntity
	CEntityKeyValues *m_pEntityKeyValues;
	// The arena StartNewEntity creates the key values in (the one of the entity system). Named
	// by what it holds
	CKV3Arena *m_pKeyValuesArena;
	CEntityClass *m_pEntityClass;
	// The name the class was looked up by
	CUtlString m_scriptClassName;
	// Passed to the creation of the entity
	EntityNetworkingMode_t m_nMode;
	SpawnGroupHandle_t m_hSpawnGroupHandle;
	matrix3x4a_t m_mSpawnGroupToWorld;
	bool m_bValidateKeys;
	// Set while the spawner writes the "classname" key of the key values. Named by what it does
	bool m_bSettingClassname;
	// The contexts of the spawn, each linked to the next through its parent: StartNewEntity
	// applies their key values, the spawn calls their callbacks with the new entity
	CEntitySpawnerContext< T > *m_pContext;
	// Nothing touches these 8 bytes
	uint64 m_nReserved;
};

#endif // ENTITYSPAWNER_H
