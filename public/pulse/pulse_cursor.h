//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Virtual tables of the Pulse cursor (the execution state of one flow through a
//          graph instance) and of the scope of the variables a cursor observes.
//
//=============================================================================//

#ifndef PULSE_CURSOR_H
#define PULSE_CURSOR_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "pulse/pulse_interface.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CPulseCell_Base;
class CPulseCursorBaseObservableScope;
class CBasePulseGraphInstance;
class CEntityInstance;
class CGlobalSymbol;

// The clock time of a cursor
typedef float PulseCursorTime_t;

//-----------------------------------------------------------------------------
// CPulseExecCursor is a schema class; this class declares its virtual methods only (see
// playerpawn_component.h for the VTable pattern). Its primary base is the log, the host
// interfaces are second bases.
//
// The overrides of the second-base methods are declared where the class declares them: the
// Itanium ABI gives each of them an entry of the primary table in that order, MSVC keeps them
// in the second-base tables only. The second bases stand at other offsets here than in the
// game, so do not cast a pointer to the game object to them through this class.
//-----------------------------------------------------------------------------
abstract_class CPulseExecCursorVTable : public IPulseExecLog, public IPulseHost_Unknown, public IPulseHost_YieldingCursor, public IPulseHost_BaseObservable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual ~CPulseExecCursorVTable() = 0;

	virtual IPulseHost_Cursor *GetHost_Cursor() override = 0;
	virtual CBasePulseGraphInstance *GetGraphInstance() override = 0;
	virtual CPulseExecCursor *GetCursor() override = 0;
	virtual const CBasePulseGraphInstance *GetGraphInstance() const override = 0;
	virtual const CPulseExecCursor *GetCursor() const override = 0;
	virtual IPulseHost_YieldingCursor *GetHost_YieldingCursor() override = 0;
	virtual IPulseHost_BaseObservable *GetHost_BaseObservable() override = 0;
	virtual void AddObservable( const PulseSymbol_t &name ) override = 0;
	virtual void ReturnFromFlow() override = 0;
	virtual void Complete( int nResult ) override = 0;
	virtual void JumpToOutflow( const CPulse_OutflowConnection &outflow, const void *pOutflowValues ) override = 0;
	virtual PulseCursorRef_t CreateChildCursor( const CPulse_OutflowConnection &outflow, const void *pOutflowValues ) override = 0;
	virtual void OnYielded( const CPulseCell_Base *pCell ) override = 0;
	virtual void QueueYieldResume( const CPulse_OutflowConnection &outflow, const void *pOutflowValues ) override = 0;

	// Named by what they do: write / read cursor_id, parent_cursor_id and the rest of the cursor
	virtual void SaveCursor( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void LoadCursor( CKV3TransferLoadContext *pContext ) = 0;

	virtual void AddTag( const char *pszTagName, const CPulseCallContextValue &tagValue, bool bInherit, bool bFlag, bool bRecordHistory ) override = 0;
	virtual bool HasTag( const char *pszTagName, const CPulseCallContextValue &tagValue ) const override = 0;
	virtual CPulseCallContextValue GetTagValue( const char *pszTagName ) const override = 0;
	virtual void RemoveTag( const char *pszTagName, const CPulseCallContextValue &tagValue ) override = 0;
	virtual void GetOutflowCriteria( const OutflowWithRequirements_t &outflow, const char *pszCriteriaClassName, CUtlVector< void * > *pCriteria ) override = 0;
	virtual void *AddCriteria( const CPulseCell_BaseRequirement *pRequirement ) override = 0;

	// Named by what they do: log "Cursor[...]: Observer notifying ..." and "Resumed from cell ..."
	virtual void NotifyObserver( const CPulseCell_Base *pCell ) = 0;
	virtual void OnResumed( const CPulseCell_Base *pCell ) = 0;
	// Named by what it does: forwards to OnCursorWake of the cell the cursor waits in
	virtual void OnAutoWake() = 0;
	// Named by what it does: asks the clock of the graph instance for the time
	virtual PulseCursorTime_t GetCursorTime() const = 0;
	// Named by what it does: allocates the scope of the observed variables
	virtual CPulseCursorBaseObservableScope *CreateObservableScope() const = 0;
};

//-----------------------------------------------------------------------------
// The scope of the things a cursor observes (CPulseServerCursorObservableScope on the server
// keeps entity outputs, the test domain observable symbols). Methods named by what they do;
// empty or false in the base. No data of its own: CPulseExecCursor::CreateObservableScope
// allocates the base as the virtual table pointer alone.
//-----------------------------------------------------------------------------
abstract_class CPulseCursorBaseObservableScope
{
public:
	virtual ~CPulseCursorBaseObservableScope() = 0;
	// Registers the cursor with every observed entry (the server: output callbacks of the
	// pulse game system)
	virtual void StartObserving( const PulseCursorRef_t &cursor ) = 0;
	// Empties the list of observed entries
	virtual void ClearObservables() = 0;
	// Adds a symbol once (IPulseHost_BaseObservable::AddObservable); empty on the server
	virtual void AddObservable( const PulseSymbol_t &name ) = 0;
	// Whether the scope observes this output of the entity
	virtual bool IsObservingEntityOutput( CEntityInstance *pEntity, const CGlobalSymbol &output ) const = 0;
	// Write / read the observed entries (member "m_EntityOutputs" on the server)
	virtual void SaveObservables( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void LoadObservables( CKV3TransferLoadContext *pContext ) = 0;
};

#endif // PULSE_CURSOR_H
