//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system the Pulse graphs of entities wait on: entity output
//          callbacks, queued outputs and the DebugWorldText overlays.
//
//=============================================================================//

#ifndef BASEPULSEGAMESYSTEM_H
#define BASEPULSEGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"
#include "entity2/entityio.h"
#include "entity2/entitypulsecallcontext.h"
#include "pulse/ipulsecursorlistener.h"
#include "pulse/ipulsesystem.h"

#include <tier0/globalsymbol.h>
#include <tier0/threadtools.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>

// A reference to a Pulse cursor (12 bytes; IPulseSystem::QueueCursorResultEvent takes one): valid while
// the graph instance exists, has a cursor at m_nCursorIndex and that cursor's serial is still
// m_nCursorSerial. Member names are descriptive.
struct PulseCursorRef_t
{
	PulseGraphInstanceID_t m_nGraphInstance;
	int m_nCursorIndex;
	int m_nCursorSerial;
};

//-----------------------------------------------------------------------------
// Abstract base of CServerPulseGameSystem. No source names its members; they are named by what the
// code does with them (the save keys "output_callbacks" and "queued_outputs" name two of them).
// It adds no virtual methods of its own; on Linux the overrides of the two second bases also take
// slots 66-72 of the primary table.
//-----------------------------------------------------------------------------
abstract_class CBasePulseGameSystem : public CAutoGameSystem, public IEntityIONotify, public IPulseCursorListener
{
public:
	DECLARE_GAME_SYSTEM();

	// An entity text overlay added by the DebugWorldText Pulse node (40 bytes)
	struct DebugWorldText_t
	{
		CEntityHandle m_hEntity;
		// Added to the entity's origin z
		float m_flVerticalOffset;
		int m_nLineOffset;
		CUtlString m_Text;
		// RGB and alpha << 24
		uint32 m_nColor;
		// Counts down by 1/64 every draw; the entry is dropped at 0
		float m_flTimeLeft;
		float m_flScale;
	};

	// A cursor waiting for any output of an entity (16 bytes)
	struct EntityOutputWaiter_t
	{
		CEntityHandle m_hEntity;
		PulseCursorRef_t m_Cursor;
	};

	// A cursor waiting for one output of an entity (24 bytes, "output_callbacks"); the entity gets an
	// output connection for it
	struct OutputCallback_t
	{
		CEntityHandle m_hEntity;
		PulseCursorRef_t m_Cursor;
		CGlobalSymbol m_OutputName;
	};

	// An output that fired for a waiting cursor, resumed in the next ServerPostEntityThink (184 bytes,
	// "queued_outputs")
	struct QueuedOutput_t
	{
		CEntityHandle m_hCaller;
		CEntityHandle m_hActivator;
		PulseCursorRef_t m_Cursor;
		CGlobalSymbol m_OutputName;
		// 1 while the output still has to be delivered
		int m_nPending;
		CPulseArgumentPack m_Args;
	};

	CBasePulseGameSystem();
	virtual ~CBasePulseGameSystem();

	// Register / unregister this system as the entity system's IO notify and as a cursor state
	// listener of the Pulse system
	GS_EVENT( GameInit );
	GS_EVENT( GameShutdown );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IEntityIONotify: only OnOutputFired does something (queues the output for the waiting cursors)
	virtual void OnConnectionFired( CEntityInstance *pActivator, CEntityInstance *pCaller, const EntityIOConnection_t *pConnection, const CPulseArgumentPack *pArgs ) override;
	virtual void OnInputAccepted( CEntityInstance *pTarget, const char *pszInputName, CEntityInstance *pActivator, CEntityInstance *pCaller, const CVariant *pValue, const CPulseArgumentPack *pArgs ) override;
	virtual void OnConnectionRemoved( CEntityInstance *pActivator, CEntityInstance *pCaller, const EntityIOConnection_t *pConnection ) override;
	virtual void OnOutputFired( CEntityInstance *pActivator, CEntityInstance *pCaller, float flDelay, const EntityIOOutputDesc_t *pDesc, const CPulseArgumentPack *pArgs ) override;

	// IPulseCursorListener (GetSaveStateMemberName is left to the derived class)
	virtual void SaveCursorState( CPulseExecCursor *pCursor, CKV3TransferSaveContext *pContext ) override;
	virtual void LoadCursorState( CPulseExecCursor *pCursor, CKV3TransferLoadContext *pContext ) override;
	virtual void OnCursorRemoved( CPulseExecCursor *pCursor ) override;

public:
	CUtlVector< DebugWorldText_t > m_DebugWorldTexts;
	CUtlVector< EntityOutputWaiter_t > m_EntityOutputWaiters;
	CUtlVector< OutputCallback_t > m_OutputCallbacks;
	CUtlVector< QueuedOutput_t > m_QueuedOutputs;
	// Guards m_EntityOutputWaiters and m_QueuedOutputs (outputs fire on any thread)
	CThreadFastMutex m_Mutex;
};

#endif // BASEPULSEGAMESYSTEM_H
