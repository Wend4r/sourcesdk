//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Calls the delegates registered for the completion of a spawn group.
//
//=============================================================================//

#ifndef SPAWNGROUPCOMPLETIONCALLBACKGAMESYSTEM_H
#define SPAWNGROUPCOMPLETIONCALLBACKGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "spawngrouptypes.h"

#include <entity2/entityidentity.h>
#include <tier0/utlvector.h>
#include <tier0/utldelegateimpl.h>

class CEntityInstance;
class CEntityKeyValues;

typedef uint32 SpawnGroupCompletionDelegateHandle_t;

typedef CUtlDelegate< void( SpawnGroupHandle_t, const CUtlVector< CEntityHandle > & ) > SpawnGroupCompletionDelegate_t;
typedef CUtlDelegate< void( CEntityInstance *, const CEntityKeyValues * ) > SpawnGroupEntitySpawnCompletionDelegate_t;
// Named by what it does: the delegates of m_createdDelegates get only the spawn group handle
typedef CUtlDelegate< void( SpawnGroupHandle_t ) > SpawnGroupCreatedDelegate_t;

//-----------------------------------------------------------------------------
// Registered as "SpawnGroupCompletionCallbackGameSystem" (reallocating factory). 56 bytes on both
// platforms.
//-----------------------------------------------------------------------------
class CSpawnGroupCompletionCallbackGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Named by what it does: laid out like SpawnGroupCompletionDelegateInfo_t
	struct SpawnGroupCreatedDelegateInfo_t
	{
		CEntityHandle m_hListener;
		SpawnGroupCreatedDelegate_t m_delegate;
		SpawnGroupCompletionDelegateHandle_t m_nHandle;
	};

	struct SpawnGroupCompletionDelegateInfo_t
	{
		CEntityHandle m_hListener;
		SpawnGroupCompletionDelegate_t m_delegate;
		SpawnGroupCompletionDelegateHandle_t m_nHandle;
	};

	struct SpawnGroupEntitySpawnCompletionDelegateInfo_t
	{
		CEntityHandle m_hListener;
		CEntityInstance *m_pSpawnedEntity;
		// Holds a reference
		const CEntityKeyValues *m_pKeyValues;
		SpawnGroupEntitySpawnCompletionDelegate_t m_delegate;
		SpawnGroupCompletionDelegateHandle_t m_nHandle;
	};

	struct SpawnGroupFinishListener_t
	{
		SpawnGroupHandle_t m_hSpawnGroup;
		// Called (and emptied) on the first tick on which the spawn group
		// exists in the spawn group manager
		CUtlVector< SpawnGroupCreatedDelegateInfo_t > m_createdDelegates;
		CUtlVector< SpawnGroupCompletionDelegateInfo_t > m_completionDelegates;
		CUtlVector< SpawnGroupEntitySpawnCompletionDelegateInfo_t > m_entityCompletionDelegates;
		CUtlVector< CEntityHandle > m_SpawnedEntityHandles;
		int m_nSpawnGroupRefCount;
	};

	virtual ~CSpawnGroupCompletionCallbackGameSystem();

	// Removes the listener of the spawn group
	GS_EVENT( SpawnGroupUncache );
	// Calls the completion delegates of the spawn group
	GS_EVENT( PostSpawnGroupLoad );
	// Call the created delegates of the client (ClientPreUpdate) and server (ServerPreEntityThink)
	// spawn groups that exist now
	GS_EVENT( ClientPreUpdate );
	GS_EVENT( ServerPreEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CUtlVector< SpawnGroupFinishListener_t > m_listenForSpawnGroup;
	// The handle the next registered delegate gets
	SpawnGroupCompletionDelegateHandle_t m_nCurrentHandle;
};

#endif // SPAWNGROUPCOMPLETIONCALLBACKGAMESYSTEM_H
