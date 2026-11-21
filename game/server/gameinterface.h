#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/gameappsystem.h"
#include "eiface.h"
#include "tier0/threadtools.h"
#include "tier1/utldelegate.h"
#include "tier0/utlmap.h"
#include "tier0/utlvector.h"
#include "tier0/bitvec.h"
#include "entity2/ientitylistener.h"
#include "entity2/istreamentitiesfromfilecompletion.h"
#include "game/server/iservertoolsinfo.h"

#include <cs_usercmd.pb.h>

class CBasePlayerController;
class KeyValues;

//-----------------------------------------------------------------------------
// The server game DLL interface
//-----------------------------------------------------------------------------
class CSource2Server : public CGameAppSystem< ISource2Server >, public IStreamEntitiesFromFileCompletion
{
public:
	// Named by what it does: the last user command of every player slot, taken by CaptureUserCommands()
	// and turned into a message by CreateUserCommandsMessage()
	struct CapturedUserCommands_t
	{
		struct PlayerCommand_t
		{
			// 0 when the slot has no command
			int m_nCommandNumber;
			CSGOUserCmdPB m_UserCmd;
		};

		// gpGlobals->tickcount at the capture
		int m_nTickCount;
		PlayerCommand_t m_Commands[ ABSOLUTE_PLAYER_LIMIT ];
	};

	virtual bool Connect( CreateInterfaceFn factory );
	virtual void Disconnect();
	virtual void *QueryInterface( const char *pInterfaceName );
	virtual InitReturnVal_t Init();
	virtual void Shutdown();
	virtual void PreShutdown();
	virtual const AppSystemInfo_t *GetDependencies();

	// IStreamEntitiesFromFileCompletion; empty in this game
	virtual void OnStreamEntitiesFromFileCompleted( CUtlVector< CEntityHandle > &entities );

	CreateInterfaceFn m_appSystemFactory;
	// gpGlobals->curtime until which an autosave stays pending, 0 when none is
	float m_fAutoSaveDangerousTime;
	float m_fAutoSaveDangerousMinHealthToCommit;
	bool m_bIsHibernating;
	// Nothing in this game reads it
	KeyValues *m_pkvSessionPersisterEntities;
	// Allocated by Init(), freed by Shutdown()
	CapturedUserCommands_t *m_pCapturedUserCommands;
};

//-----------------------------------------------------------------------------
// The server entity interface
//-----------------------------------------------------------------------------
class CSource2GameEntities : public CGameAppSystem< ISource2GameEntities >, public IEntityListener
{
public:
	// Removes the entity listener
	virtual void Shutdown();

	// IEntityListener: keep m_EntitiesWithSecondarySendProxies up to date
	virtual void OnEntitySpawned( CEntityInstance *pEntity );
	virtual void OnEntityDeleted( CEntityInstance *pEntity );

	// gpGlobals->curtime - ent_steadystate_delay at the last CheckTransmit
	float m_flSteadyStateChangeTime;
	// Named by what it does: the entities whose class is set in m_EntityClassesWithSecondarySendProxies
	CBitVec< 16384 > m_EntitiesWithSecondarySendProxies;
	int m_nEntitiesWithSecondarySendProxies;
	// Named by what it does: a bit per entity class index, set when the flattened serializer of the class has
	// secondary send proxies (IFlattenedSerializers::GetFieldPathCaches); filled when the level starts
	CLargeVarBitVec m_EntityClassesWithSecondarySendProxies;
	// ent_steadystate_enable at the last CheckTransmit
	bool m_bSteadyStateEnabled;
	// Named by what it does: true while CheckTransmit runs its parallel job over the clients
	bool m_bInParallelCheckTransmit;
};

//-----------------------------------------------------------------------------
// The server player / client interface
//-----------------------------------------------------------------------------
class CSource2GameClients : public CGameAppSystem< ISource2GameClients >
{
public:
	struct SvcUserMessageHandlerInfo_t;

	typedef void ( *SvcUserMessageHandlerFn_t )( CSource2GameClients *pSource2GameClients, uint32 nSize, const void *pBuffer, SvcUserMessageHandlerInfo_t *pInfo, CBasePlayerController *pPlayer );

	struct SvcUserMessageHandlerInfo_t
	{
		SvcUserMessageHandlerInfo_t() : SvcUserMessageHandlerInfo_t( nullptr )
		{
		}

		explicit SvcUserMessageHandlerInfo_t( SvcUserMessageHandlerFn_t pfnHandler ) : m_pfnHandler( pfnHandler )
#ifndef _WIN32
			, m_nThisAdjust( 0 )
#endif
		{
		}

		// Native declaration: typedef void ( CSource2GameClients::*SvcUserMessageHandlerFn_t )( uint32 nSize, const void *pBuffer, SvcUserMessageHandlerInfo_t *pInfo, CBasePlayerController *pPlayer );
		SvcUserMessageHandlerFn_t m_pfnHandler;
#ifndef _WIN32
		// On Linux, m_nThisAdjust completes the native member-function pointer.
		ptrdiff_t m_nThisAdjust;
#endif
		CUtlVector< CUtlAbstractDelegate > m_Delegates;
	};

	CUtlMap< int, SvcUserMessageHandlerInfo_t > m_UserMessageHandlers;
	CThreadRWLock_FastRead m_UserMessageLock;
};

//-----------------------------------------------------------------------------
// Exposes the server's entities to the tools
//-----------------------------------------------------------------------------
class CServerToolsInfo : public CGameAppSystem< IServerToolsInfo >
{
public:
	virtual InitReturnVal_t Init();
	virtual void Shutdown();
	virtual const AppSystemInfo_t *GetDependencies();
};

#endif // GAMEINTERFACE_H
