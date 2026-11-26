//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Virtual table of the base of the Pulse graph instances (one running copy of a
//          graph, owned by an entity or a game blackboard on the server).
//
//=============================================================================//

#ifndef PULSE_GRAPH_INSTANCE_H
#define PULSE_GRAPH_INSTANCE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "pulse/pulsesymbol.h"
#include "pulse/pulse_interface.h"

class CBufferString;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CPulseArgumentPack;
class CPulseExecCursor;
class IPulseBlackboardView;
class KeyValues3;

//-----------------------------------------------------------------------------
// CBasePulseGraphInstance is an abstract schema class; this class declares its virtual methods
// only (see playerpawn_component.h for the VTable pattern). The server derives
// CPulseGraphInstance_ServerEntity (the entity domain, which adds IPulseHost_GameEntity as a
// second base) and CPulseGraphInstance_GameBlackboard from it.
//-----------------------------------------------------------------------------
abstract_class CBasePulseGraphInstanceVTable : public IPulseHost_Unknown
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	// Named by what they do: the domain of the instance ("ServerEntity", "GameBlackboard"),
	// whether the instance belongs to a domain (its own or a base one, "BaseDomain") and the
	// symbol of its domain
	virtual const char *GetDomainName() const = 0;
	virtual bool IsInDomain( const PulseSymbol_t &domain ) const = 0;
	virtual PulseSymbol_t GetDomainSymbol() const = 0;
	// Named by what it does: allocates and constructs the cursor of the domain (CPulseServerCursor)
	virtual CPulseExecCursor *CreateCursor( CPulseExecCursor *pParent ) = 0;
	virtual ~CBasePulseGraphInstanceVTable() = 0;
	// Named by what they do: replaces the graph definition (refcounted) and initialises the
	// instance again; activates the instance and registers it with the pulse system once
	virtual void SetGraphDef( void *pGraphDef ) = 0;
	virtual void Activate() = 0;
	virtual void StartGraphReload( void *pContext ) = 0;
	virtual void EndGraphReload() = 0;
	// Named by what it does: finds the graph hook cell (CPulseCell_Inflow_GraphHook) of the hook
	// symbol, creates a cursor (CreateCursor) and runs it from there with the two arguments;
	// false when the graph has no such hook
	virtual bool CallGraphHook( const PulseSymbol_t &hook, const void *pArgData1, const void *pArgData2 ) = 0;
	// Named by what it does: copies the value of the named variable (IPulseGraphDef::FindVariable)
	// into pValue; false when the graph has no such variable
	virtual bool GetVariableValue( const char *pszVariable, KeyValues3 *pValue ) = 0;
	// Named by what it does: runs the method entry cell of the name (IPulseGraphDef::FindMethodCell)
	// in a new cursor, as CallGraphHook does
	virtual bool CallMethod( const char *pszMethod, const void *pArgs ) = 0;
	// Named by what it does: the graph instance of the entity the handle value refers to (the
	// pulse graph instance of the entity, or of a point prefab's target); null in the base
	virtual CBasePulseGraphInstance *GetGraphInstanceOfEntity( const void *pHandleValue ) = 0;
	// Named by what it does: the game blackboard of the name; null in the base
	virtual IPulseBlackboardView *FindBlackboard( const PulseSymbol_t &name ) = 0;
	// A domain test wider than IsInDomain: the base asks IsInDomain (or compares with
	// "BaseDomain"), the server entity domain also accepts "GameBlackboard"
	virtual bool Stub_23( const PulseSymbol_t &domain ) = 0;
	// Named by what it does: the time of the clock the cursors of the instance run on and the
	// name of that clock (Plat_FloatTime() in the base)
	virtual void GetClockTime( float *pflTime, PulseSymbol_t *pClockName ) const = 0;
	// Named by what they do: write / read the members of the instance (outputs, owner, activation)
	virtual void SaveInstanceState( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void LoadInstanceState( CKV3TransferLoadContext *pContext ) = 0;
	// Named by what they do: report "Probable runaway cursor creation detected!" and
	// "Probable infinite loop detected!"
	virtual void OnRunawayCursorCreation() = 0;
	virtual void OnInfiniteLoopDetected() = 0;
	// Named by what it does: formats the owner of the instance
	virtual void GetOwnerDescription( CBufferString &sOut ) const = 0;
	// Named by what it does: writes the value as text (a schema enum as its enumerator name; the
	// entity domain writes an entity handle as "<name> [<index>]"); returns true
	virtual bool FormatValue( const CPulseValueFullType &type, const void *pValue, CBufferString &sOut ) = 0;
	// The entity domain resolves a named domain value ("ResolveDomainValue entity not found: %s")
	virtual bool ResolveDomainValue( const PulseSymbol_t &name, void *pValueOut ) = 0;
	// Named by what it returns: the scope of the module the domain lives in, in the numbering of
	// the scope loaders (0 pulse_system for the test domain, 1 server for both server domains)
	// and 5, the scope of "BaseDomain" (every scope), in the base
	virtual int GetDomainScope() const = 0;
	// Named by what it does: a public output of the graph fired with the arguments; empty in the
	// base, the entity domain fires the entity output of the owner, the test domain logs
	// "public_output::%s"
	virtual void OnPublicOutputFired( const void *pOutput, const CPulseArgumentPack *pArgs ) = 0;
	// Named by what it does: finds the game blackboard of the name or spawns its entity (the
	// test domain only looks it up); null in the base
	virtual IPulseBlackboardView *FindOrCreateBlackboard( const PulseSymbol_t &name ) = 0;
	// Named by what it does: runs the event handler cell (CPulseCell_Inflow_EventHandler) of the
	// event symbol in a new cursor, as CallGraphHook does
	virtual bool CallEventHandler( const PulseSymbol_t &event, const void *pArgs ) = 0;
	// Named by what it does: runs the observable variable listener entry points of the variable
	virtual void NotifyObservableVariableListeners( uint32 nVariable, const void *pNewValue, const void *pOldValue ) = 0;
	// Named by what it does: runs the observable variable listener cell of the listener symbol
	// with previous_value / current_value (the handler of
	// IPulseSystem::QueueObservableVariableListenerNotifyEvent)
	virtual bool CallObservableVariableListener( const PulseSymbol_t &listener, uint32 nVariable, const void *pNewValue, const void *pOldValue ) = 0;
};

#endif // PULSE_GRAPH_INSTANCE_H
