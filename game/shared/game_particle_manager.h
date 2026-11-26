//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that owns the game particle manager.
//
//=============================================================================//

#ifndef GAME_PARTICLE_MANAGER_H
#define GAME_PARTICLE_MANAGER_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "particles/iparticlesystemmgr.h"
#include "entityhandle.h"
#include "tier0/utlvector.h"

class CBaseEntity;
class IGameMessageDelegateHook;
class CUserMsg_ParticleManager_t;

//-----------------------------------------------------------------------------
// The particle manager of the game, 128 bytes on both platforms; hooks the CUserMsg_ParticleManager
// message and sends the particle messages to the clients.
//
// Slot numbers in the Unk_ names are Windows indexes; on Linux the destructor takes two
// slots, so every slot after it is one further. The creation info the create methods take
// starts with the name of the particle system and holds the entity at offset 16; its type is
// not reconstructed.
//-----------------------------------------------------------------------------
abstract_class CGameParticleManager
{
public:
	// Unhooks the particle manager message ("%d unhooked %s")
	virtual ~CGameParticleManager() {}

	// Takes the next particle index and creates the particle with CreateParticleEffectInternal by name;
	// returns the index, -1 without a name or when bSkip is set.
	virtual int CreateParticle( bool bSkip, const void *pCreateInfo ) = 0;

	// Sends the release message (9) for the index and frees the particle's entry.
	virtual void ReleaseParticleIndex( int nIndex ) = 0;

	// Sends the destroy-involving message for the entity, immediately or not.
	virtual void DestroyAllParticleEffectsInvolving( CBaseEntity *pEntity, bool bDestroyImmediately ) = 0;

	// The particle system to use in place of pszName for the entity; the name itself here.
	virtual const char *GetParticleReplacement( const char *pszName, CBaseEntity *pEntity ) = 0;

	// Creates the particle of an index from a loaded particle system definition: keeps an entry with the
	// definition and the entity, then sends the create message. The two overloads take Linux slots 6 and
	// 7 in this order; MSVC puts them the other way round (Windows 5 is the one by name).
	virtual void CreateParticleEffectInternal( int nIndex, HParticleSystemDefinition hDefinition, const void *pCreateInfo ) = 0;

	// The same by name: resolves the name (after GetParticleReplacement) to its definition, loading it
	// just in time ("JustInTimeManifest"), and passes it on to the overload above.
	virtual void CreateParticleEffectInternal( int nIndex, const void *pCreateInfo ) = 0;

	// Sends a particle manager message to a recipient filter (the HLTV / demo path when recording one).
	virtual void Unk_7() = 0;

	// The attachment index of a name on a model entity (0 for other entities).
	virtual void Unk_8() = 0;

	// The network form of an entity's handle (0xFFFFFF for none) that the messages carry.
	virtual void Unk_9() = 0;

public:
	// A particle the manager created, 32 bytes; one heap allocation per particle
	struct ParticleEntry_t
	{
		// The index the clients know the particle by
		int m_nIndex;
		// The definition the particle was created from; the entry holds a reference to it
		HParticleSystemDefinition m_hDefinition;
		// The entity of the creation info
		CEntityHandle m_hEntity;
		// Released through its virtual table (byte offset 104) when the entry goes; the server never
		// sets it
		void *m_pReleasable;
	};

	// The hooks of the particle manager message (one, CGameMessageDelegateHook< CUserMsg_ParticleManager_t >)
	CUtlVector< IGameMessageDelegateHook * > m_MessageHooks;
	// True while m_MessageHooks holds no hook: set by the constructor until it hooks the message and
	// by the destructor after it unhooks and deletes every hook
	bool m_bMessageHooksEmpty;

	// A copy of every particle manager message the hook receives; nothing on the server reads them,
	// the destructor deletes them
	CUtlVector< CUserMsg_ParticleManager_t * > m_ReceivedMessages;

	// The messages sent while no client can receive them (the game is still loading and the recipient
	// filter is empty); sent to everyone and deleted when a client becomes active
	CUtlVector< CUserMsg_ParticleManager_t * > m_PendingMessages;

	// Cleared by the constructor; nothing else reads or writes it
	int m_nReserved;

	// The particles created by index, until their index is released
	CUtlVector< ParticleEntry_t * > m_Particles;

	// The index CreateParticle gives the next particle
	int m_nNextParticleIndex;
};

//-----------------------------------------------------------------------------
// Static instance (factory "CGameParticleManagerSystem"), 32 bytes on both
// platforms.
//-----------------------------------------------------------------------------
class CGameParticleManagerSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// Allocates m_pParticleManager
	GS_EVENT( GameInit );
	// Deletes m_pParticleManager
	GS_EVENT( GameShutdown );
	// Empty on the server
	GS_EVENT( ClientPreRender );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	// Named by what it holds: the particle manager of the running game, NULL outside of it
	CGameParticleManager *m_pParticleManager;
};

#endif // GAME_PARTICLE_MANAGER_H
