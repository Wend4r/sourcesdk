//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef IGAMESYSTEM_H
#define IGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "iloopmode.h"

#include <entity2/entityidentity.h>
#include <tier0/utlstring.h>
#include <tier1/utlvector.h>

/*
* AMNOTE: To create your own gamesystem, you need to inherit from CBaseGameSystem or CAutoGameSystem,
* and define the events that you are interested in receiving via the GS_EVENT macro, example:
* 
* class TestGameSystem : CBaseGameSystem
* {
*     GS_EVENT( GameInit )
*     {
*         // In every event there's a ``msg`` argument provided which is a struct for this particular event
*         // refer to struct implementation to see what members are available in it, example usage:
*         msg->m_pConfig;
*         msg->m_pRegistry;
*     }
*     
*     // Or you can declare the needed events in your gamesystem, and define their body somewhere else
*     GS_EVENT( GameShutdown );
* };
* 
* GS_EVENT_MEMBER( TestGameSystem, GameShutdown )
* {
*     // The same ``msg`` argument would be available in here as well.
* }
*/

// Forward declaration
class GameSessionConfiguration_t;
class ILoopModePrerequisiteRegistry;
class IEntityResourceManifest;
struct EngineLoopState_t;
class ISpawnGroupPrerequisiteRegistry;
class IEntityPrecacheConfiguration;
struct EntitySpawnInfo_t;
class CBaseGameSystemFactory;

typedef int GameSystemEventId_t;

#define DECLARE_GAME_SYSTEM() \
	virtual void YouForgot_DECLARE_GAME_SYSTEM_InYourClassDefinition() override {}; \
	struct YouForgot { } m_YouForgot;

#define GS_EVENT_MSG( name ) struct Event##name##_t
#define GS_EVENT_MSG_CHILD( name, parent ) struct Event##name##_t : Event##parent##_t

GS_EVENT_MSG( GameInit )
{
	const GameSessionConfiguration_t *m_pConfig;
	ILoopModePrerequisiteRegistry *m_pRegistry;
};

GS_EVENT_MSG( GamePostInit )
{
	const GameSessionConfiguration_t *m_pConfig;
	ILoopModePrerequisiteRegistry *m_pRegistry;
};

GS_EVENT_MSG( GameShutdown );
GS_EVENT_MSG( GamePreShutdown );

GS_EVENT_MSG( GameActivate )
{
	const EngineLoopState_t *m_pState;
	bool m_bBackgroundMap;
};

GS_EVENT_MSG( GameDeactivate )
{
	const EngineLoopState_t *m_pState;
	bool m_bBackgroundMap;
};

GS_EVENT_MSG( ClientFullySignedOn );
GS_EVENT_MSG( Disconnect );

GS_EVENT_MSG( BuildGameSessionManifest )
{
	IEntityResourceManifest *m_pResourceManifest;
};

GS_EVENT_MSG( SpawnGroupPrecache )
{
	CUtlString m_SpawnGroupName;
	CUtlString m_EntityLumpName;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	int m_nEntityCount;
	const EntitySpawnInfo_t *m_pEntitiesToSpawn;
	ISpawnGroupPrerequisiteRegistry *m_pRegistry;
	IEntityResourceManifest *m_pManifest;
	IEntityPrecacheConfiguration *m_pConfig;
};

GS_EVENT_MSG( SpawnGroupUncache )
{
	CUtlString m_SpawnGroupName;
	CUtlString m_EntityLumpName;
	SpawnGroupHandle_t m_SpawnGroupHandle;
};

GS_EVENT_MSG( PreSpawnGroupLoad )
{
	CUtlString m_SpawnGroupName;
	CUtlString m_EntityLumpName;
	SpawnGroupHandle_t m_SpawnGroupHandle;
};

GS_EVENT_MSG( PostSpawnGroupLoad )
{
	CUtlString m_SpawnGroupName;
	CUtlString m_EntityLumpName;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CUtlVector<CEntityHandle> m_EntityList;
};

GS_EVENT_MSG( PreSpawnGroupUnload )
{
	CUtlString m_SpawnGroupName;
	CUtlString m_EntityLumpName;
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CUtlVector<CEntityHandle> m_EntityList;
};

GS_EVENT_MSG( PostSpawnGroupUnload )
{
	CUtlString m_SpawnGroupName;
	CUtlString m_EntityLumpName;
	SpawnGroupHandle_t m_SpawnGroupHandle;
};

GS_EVENT_MSG( ActiveSpawnGroupChanged )
{
	SpawnGroupHandle_t m_SpawnGroupHandle;
	CUtlString m_SpawnGroupName;
	SpawnGroupHandle_t m_PreviousHandle;
};

GS_EVENT_MSG( ClientPostDataUpdate );

GS_EVENT_MSG( ClientPreRender )
{
	float m_flFrameTime;
};
GS_EVENT_MSG( ClientPostRender );

GS_EVENT_MSG( ClientPreEntityThink )
{
	bool m_bFirstTick;
	bool m_bLastTick;
};

GS_EVENT_MSG( ClientPollNetworking );

GS_EVENT_MSG( ClientUpdate )
{
	float m_flFrameTime;
	bool m_bFirstTick;
	bool m_bLastTick;
};

GS_EVENT_MSG( ServerPreEntityThink )
{
	bool m_bFirstTick;
	bool m_bLastTick;
};

GS_EVENT_MSG( ServerPostEntityThink )
{
	bool m_bFirstTick;
	bool m_bLastTick;
};

GS_EVENT_MSG( ServerPreClientUpdate );
GS_EVENT_MSG( ServerAdvanceTick );
GS_EVENT_MSG( ClientAdvanceTick );

GS_EVENT_MSG( Simulate )
{
	EngineLoopState_t m_LoopState;
	bool m_bFirstTick;
	bool m_bLastTick;
};

GS_EVENT_MSG_CHILD( ServerGamePostSimulate, Simulate ) { };
GS_EVENT_MSG_CHILD( ClientGamePostSimulate, Simulate ) { };

GS_EVENT_MSG( ServerPostAdvanceTick );
GS_EVENT_MSG( ServerBeginAsyncPostTickWork );

GS_EVENT_MSG( ServerEndAsyncPostTickWork );

GS_EVENT_MSG( ClientFrameSimulate );
GS_EVENT_MSG( ClientPauseSimulate );
GS_EVENT_MSG( ClientAdvanceNonRenderedFrame );

GS_EVENT_MSG( FrameBoundary )
{
	float m_flFrameTime;
};

GS_EVENT_MSG_CHILD( GameFrameBoundary, FrameBoundary ) { };
GS_EVENT_MSG_CHILD( OutOfGameFrameBoundary, FrameBoundary ) { };

GS_EVENT_MSG( SaveGame )
{
	const CUtlVector<CEntityHandle> *m_pEntityList;
};

GS_EVENT_MSG( RestoreGame )
{
	const CUtlVector<CEntityHandle> *m_pEntityList;
};

#define GS_EVENT_IMPL( name ) virtual void name(const Event##name##_t& msg) = 0;
#define GS_EVENT( name ) virtual void name(const Event##name##_t& msg) override
#define GS_EVENT_MEMBER( gamesystem, name ) void gamesystem::name(const Event##name##_t& msg)

//-----------------------------------------------------------------------------
// Game systems are singleton objects in the client + server codebase responsible for
// various tasks
// The order in which the server systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized.
//-----------------------------------------------------------------------------
abstract_class IGameSystem
{
public:
	struct FactoryInfo_t
	{
		FactoryInfo_t() : m_pFactory( NULL ), m_bHasBeenAdded( true ) {}

		CBaseGameSystemFactory *m_pFactory;
		bool m_bHasBeenAdded;
	};

public:
	// Init, shutdown
	// return true on success. false to abort DLL init!
	virtual bool Init() = 0;								// 0
	virtual void PostInit() = 0;							// 1
	virtual void Shutdown() = 0;							// 2

	// Game init, shutdown
	GS_EVENT_IMPL( GameInit )								// 3
	GS_EVENT_IMPL( GameShutdown )							// 4
	GS_EVENT_IMPL( GamePostInit )							// 5
	GS_EVENT_IMPL( GamePreShutdown )						// 6

	GS_EVENT_IMPL( BuildGameSessionManifest )				// 7

	GS_EVENT_IMPL( GameActivate )							// 8

	GS_EVENT_IMPL( ClientFullySignedOn )					// 9
	GS_EVENT_IMPL( Disconnect )								// 10

	GS_EVENT_IMPL( GameDeactivate )							// 11

	GS_EVENT_IMPL( SpawnGroupPrecache )						// 12
	GS_EVENT_IMPL( SpawnGroupUncache )						// 13
	GS_EVENT_IMPL( PreSpawnGroupLoad )						// 14
	GS_EVENT_IMPL( PostSpawnGroupLoad )						// 15
	GS_EVENT_IMPL( PreSpawnGroupUnload )					// 16
	GS_EVENT_IMPL( PostSpawnGroupUnload )					// 17
	GS_EVENT_IMPL( ActiveSpawnGroupChanged )				// 18

	GS_EVENT_IMPL( ClientPostDataUpdate )					// 19

	// Called before rendering
	GS_EVENT_IMPL( ClientPreRender )						// 20

	GS_EVENT_IMPL( ClientPreEntityThink )					// 21

	virtual void unk_001( const void *const msg ) = 0;		// 22
	virtual void unk_002( const void *const msg ) = 0;		// 23

	GS_EVENT_IMPL( ClientPollNetworking )					// 24

	virtual void unk_101( const void *const msg ) = 0;		// 25

	// Gets called each frame
	GS_EVENT_IMPL( ClientUpdate )							// 26

	virtual void unk_201( const void *const msg ) = 0;		// 27

	// Called after rendering
	GS_EVENT_IMPL( ClientPostRender )						// 28

	// Called each frame before entities think
	GS_EVENT_IMPL( ServerPreEntityThink )					// 29
	// called after entities think
	GS_EVENT_IMPL( ServerPostEntityThink )					// 30

	virtual void unk_301( const void *const msg ) = 0;		// 31

	GS_EVENT_IMPL( ServerPreClientUpdate )					// 32
	GS_EVENT_IMPL( ServerAdvanceTick )						// 33
	GS_EVENT_IMPL( ClientAdvanceTick )						// 34
	GS_EVENT_IMPL( ServerGamePostSimulate )					// 35
	GS_EVENT_IMPL( ClientGamePostSimulate )					// 36
	GS_EVENT_IMPL( ServerPostAdvanceTick )					// 37
	GS_EVENT_IMPL( ServerBeginAsyncPostTickWork )			// 38

	virtual void unk_401( const void *const msg ) = 0;		// 39

	GS_EVENT_IMPL( ServerEndAsyncPostTickWork )				// 40

	GS_EVENT_IMPL( ClientFrameSimulate )					// 41
	GS_EVENT_IMPL( ClientPauseSimulate )					// 42
	GS_EVENT_IMPL( ClientAdvanceNonRenderedFrame )			// 43

	GS_EVENT_IMPL( GameFrameBoundary )						// 44
	GS_EVENT_IMPL( OutOfGameFrameBoundary )					// 45

	GS_EVENT_IMPL( SaveGame )								// 46
	GS_EVENT_IMPL( RestoreGame )							// 47

	virtual void unk_501( const void *const msg ) = 0;		// 48
	virtual void unk_502( const void *const msg ) = 0;		// 49
	virtual void unk_503( const void *const msg ) = 0;		// 50
	virtual void unk_504( const void *const msg ) = 0;		// 51
	virtual void unk_505( const void *const msg ) = 0;		// 52
	virtual void unk_506( const void *const msg ) = 0;		// 53

	virtual const char* GetName() const = 0;				// 54
	virtual void SetGameSystemGlobalPtrs(void* pValue) = 0;	// 55
	virtual void SetName(const char* pName) = 0;			// 56
	virtual bool DoesGameSystemReallocate() = 0;			// 57
	virtual ~IGameSystem() {}
	virtual void YouForgot_DECLARE_GAME_SYSTEM_InYourClassDefinition() = 0;
};

// Quick and dirty server system for users who don't care about precise ordering
// and usually only want to implement a few of the callbacks
class CBaseGameSystem : public IGameSystem
{
public:
	CBaseGameSystem(const char* pszInitName = "unnamed")
	 :  m_pName(pszInitName)
	{
	}

	// Init, shutdown
	// return true on success. false to abort DLL init!
	virtual bool Init() override { return true; }
	virtual void PostInit() override {}
	virtual void Shutdown() override {}

	// Game init, shutdown
	GS_EVENT( GameInit ) {}
	GS_EVENT( GameShutdown ) {}
	GS_EVENT( GamePostInit ) {}
	GS_EVENT( GamePreShutdown ) {}

	GS_EVENT( BuildGameSessionManifest ) {}

	GS_EVENT( GameActivate ) {}

	GS_EVENT( ClientFullySignedOn ) {}
	GS_EVENT( Disconnect ) {}

	GS_EVENT( GameDeactivate ) {}

	GS_EVENT( SpawnGroupPrecache ) {}
	GS_EVENT( SpawnGroupUncache ) {}
	GS_EVENT( PreSpawnGroupLoad ) {}
	GS_EVENT( PostSpawnGroupLoad ) {}
	GS_EVENT( PreSpawnGroupUnload ) {}
	GS_EVENT( PostSpawnGroupUnload ) {}
	GS_EVENT( ActiveSpawnGroupChanged ) {}

	GS_EVENT( ClientPostDataUpdate ) {}

	// Called before rendering
	GS_EVENT( ClientPreRender ) {}

	GS_EVENT( ClientPreEntityThink ) {}

	virtual void unk_001( const void *const msg ) override {}
	virtual void unk_002( const void *const msg ) override {}

	GS_EVENT( ClientPollNetworking ) {}

	virtual void unk_101( const void *const msg ) override {}

	// Gets called each frame
	GS_EVENT( ClientUpdate ) {}

	virtual void unk_201( const void *const msg ) override {}

	// Called after rendering
	GS_EVENT( ClientPostRender ) {}

	// Called each frame before entities think
	GS_EVENT( ServerPreEntityThink ) {}
	// called after entities think
	GS_EVENT( ServerPostEntityThink ) {}

	virtual void unk_301( const void *const msg ) override {}

	GS_EVENT( ServerPreClientUpdate ) {}
	GS_EVENT( ServerAdvanceTick ) {}
	GS_EVENT( ClientAdvanceTick ) {}
	GS_EVENT( ServerGamePostSimulate ) {}
	GS_EVENT( ClientGamePostSimulate ) {}
	GS_EVENT( ServerPostAdvanceTick ) {}
	GS_EVENT( ServerBeginAsyncPostTickWork ) {}

	virtual void unk_401( const void *const msg ) override {}

	GS_EVENT( ServerEndAsyncPostTickWork ) {}

	GS_EVENT( ClientFrameSimulate ) {}
	GS_EVENT( ClientPauseSimulate ) {}
	GS_EVENT( ClientAdvanceNonRenderedFrame ) {}

	GS_EVENT( GameFrameBoundary ) {}
	GS_EVENT( OutOfGameFrameBoundary ) {}

	GS_EVENT( SaveGame ) {}
	GS_EVENT( RestoreGame ) {}

	virtual void unk_501( const void *const msg ) override {}
	virtual void unk_502( const void *const msg ) override {}
	virtual void unk_503( const void *const msg ) override {}
	virtual void unk_504( const void *const msg ) override {}
	virtual void unk_505( const void *const msg ) override {}
	virtual void unk_506( const void *const msg ) override {}

	virtual const char* GetName() const override { return m_pName; }
	virtual void SetGameSystemGlobalPtrs(void* pValue) override {}
	virtual void SetName(const char* pName) override { m_pName = pName; }
	virtual bool DoesGameSystemReallocate() override { return false; }
	virtual ~CBaseGameSystem() {}

private:
	const char* m_pName;
};

class CAutoGameSystem : public CBaseGameSystem
{
protected:
	virtual ~CAutoGameSystem() {};
};

#endif // IGAMESYSTEM_H
