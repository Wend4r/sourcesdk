//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The choreo services of an entity (CGameChoreoServices): the scripted sequence
//          and the scene state of the entity.
//
//=============================================================================//

#ifndef CHOREOSERVICES_H
#define CHOREOSERVICES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CBaseModelEntity;
class CBaseEntity;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;

//-----------------------------------------------------------------------------
// A schema class (members m_hOwner, m_hScriptedSequence, m_scriptState, m_choreoState,
// m_flTimeStartedState in CGameChoreoServices). Methods named by what they do with those
// members.
//-----------------------------------------------------------------------------
abstract_class IChoreoServices
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual ~IChoreoServices() {}
	virtual const char *GetClassName() const = 0;
	// Stores the handle of the entity in m_hOwner (an invalid handle for null)
	virtual void SetOwner( CBaseModelEntity *pOwner ) = 0;
	virtual CBaseModelEntity *GetOwner() const = 0;
	// The body of the owner's ChoreoServicesThink: with a scripted sequence and an owner, steps
	// the scripted sequence by m_choreoState; the owner thinks again while it returns true.
	// Named by its caller
	virtual bool Think() = 0;
	virtual CBaseEntity *GetScriptedSequence() const = 0;
	// Named by what they do: clear the scripted sequence through ClearScriptedSequence, then store
	// the handle of the entity in m_hScriptedSequence and EnableThink( pScriptedSequence != null );
	// on a change of m_scriptState store the state, m_flTimeStartedState and the m_choreoState
	// the new state starts with
	virtual void SetScriptedSequence( CBaseEntity *pScriptedSequence ) = 0;
	virtual void SetScriptState( int nScriptState ) = 0;
	// Returns m_scriptState
	virtual int GetScriptState() const = 0;
	// Whether the owner can play a scripted sequence: 0 cannot play (the owner is dead, or the
	// queued sequence cannot be kicked out), 1 can play now (m_choreoState 0 or 6), 2 can play
	// after the current one
	virtual int CanPlaySequence() = 0;
	// Passes bCanceled to the owner's animgraph, clears the scripted sequence and returns true;
	// the scripted sequence calls it with true when it cancels, with false when its post idle ends
	virtual bool CineCleanup( bool bCanceled ) = 0;
	// Empty in CGameChoreoServices
	virtual void Stub_15() = 0;
	// Schedules the owner's ChoreoServicesThink (bEnable and the owner has choreo services) or
	// stops it. Named by what it does
	virtual void EnableThink( bool bEnable ) = 0;
	// Returns the class name ("CGameChoreoServices") as GetClassName does
	virtual const char *Stub_17() const = 0;
	// Empty in CGameChoreoServices
	virtual void Stub_18() = 0;
	// Returns true in CGameChoreoServices
	virtual bool Stub_19() const = 0;
	// Empty in CGameChoreoServices
	virtual void Stub_20() = 0;
	// Clears the script state, releases the scripted sequence entity and resets
	// m_hScriptedSequence, then calls EnableThink( false ); SetScriptedSequence uses it
	virtual void ClearScriptedSequence() = 0;
};

#endif // CHOREOSERVICES_H
