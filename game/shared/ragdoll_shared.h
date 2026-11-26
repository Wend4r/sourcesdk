//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that retires the oldest ragdolls. Factory "RagdollLRURetirement".
//
//=============================================================================//

#ifndef RAGDOLL_SHARED_H
#define RAGDOLL_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "ehandle.h"
#include "entitytypes.h"
#include "tier0/utllinkedlist.h"
#include "schemasystem/schematypes.h"

class CBaseAnimGraph;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of the schema struct PhysicsRagdollPose_t: the pose of a ragdoll,
// networked. The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class PhysicsRagdollPose_tVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data ) = 0;
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data ) = 0;
	virtual void NetworkStateChangedLog( const char *pszFieldName, const char *pszInfo ) = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() const = 0;
};

//-----------------------------------------------------------------------------
// Static instance, 88 bytes on both platforms. The lists keep the time each ragdoll was added.
//-----------------------------------------------------------------------------
class CRagdollLRURetirement : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Empties both lists and resets m_iMaxRagdolls to -1 (use g_ragdoll_maxcount)
	GS_EVENT( GameInit );
	// Removes the deleted and the oldest ragdolls beyond the limit, counts the others
	GS_EVENT( ClientPostUpdate );
	// Calls ClientPostUpdate
	GS_EVENT( ServerPostEntityThink );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds
	struct LRURagdoll_t
	{
		CHandle< CBaseAnimGraph > m_hRagdoll;
		// Game time the ragdoll was added; it may be retired once it is older than
		// ragdoll_lru_min_age
		GameTime_t m_flAddedTime;
	};

	CUtlLinkedList< LRURagdoll_t > m_LRU;
	CUtlLinkedList< LRURagdoll_t > m_LRUImportantRagdolls;
	// -1 uses the g_ragdoll_maxcount convar
	int m_iMaxRagdolls;
	// Counted by ClientPostUpdate
	int m_iSimulatedRagdollCount;
	int m_iRagdollCount;
};

#endif // RAGDOLL_SHARED_H
