//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The interfaces of the Pulse runtime: the host interfaces a graph instance or a
//          cursor answers queries through, the cursor log, the graph definition, the type
//          queries of a scope and the instance save / load APIs.
//
//=============================================================================//

#ifndef PULSE_INTERFACE_H
#define PULSE_INTERFACE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/globalsymbol.h"
#include "schemasystem/schematypes.h"
#include "pulse/pulsesymbol.h"

class CBaseEntity;
class CBasePulseGraphInstance;
class CBufferString;
class CEntityHandle;
class CEntityInstance;
class CEntityIOOutput;
class CGlobalSymbol;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CPulseExecCursor;
class CPulseCallContextValue;
class CPulseCell_Base;
class CPulseCell_BaseRequirement;
class CPulse_OutflowConnection;
class CPulseValueFullType;
class KeyValues3;
class IPulseHost_GameEntity;
class IPulseHost_Input;
class IPulseHost_Cursor;
class IPulseHost_YieldingCursor;
class IPulseHost_BaseObservable;
class IPulseHost_EntityObservable;
struct OutflowWithRequirements_t;
struct PulseCursorRef_t;

//-----------------------------------------------------------------------------
// The common base of the Pulse hosts (graph instances, cursors). Each getter returns the
// object as one of the host interfaces, or null when it is not one: a graph instance of the
// entity domain is an IPulseHost_GameEntity, a cursor an IPulseHost_Cursor, an
// IPulseHost_YieldingCursor and an IPulseHost_BaseObservable, a server cursor also an
// IPulseHost_Input and an IPulseHost_EntityObservable. The getters are named after the interface
// they return; the host table a binding receives (PulseHostTable_t) holds them in this order,
// and bit N of PulseBindingDesc_t::m_nRequiredCaps asks for host N. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseHost_Unknown
{
public:
	virtual IPulseHost_GameEntity *GetHost_GameEntity() = 0;
	virtual IPulseHost_Input *GetHost_Input() = 0;
	virtual IPulseHost_Cursor *GetHost_Cursor() = 0;
	virtual IPulseHost_YieldingCursor *GetHost_YieldingCursor() = 0;
	// Host 4: null in every class of the game
	virtual void *Stub_4() = 0;
	virtual IPulseHost_BaseObservable *GetHost_BaseObservable() = 0;
	virtual IPulseHost_EntityObservable *GetHost_EntityObservable() = 0;
};

//-----------------------------------------------------------------------------
// A graph instance that belongs to an entity (CPulseGraphInstance_ServerEntity). No virtual
// destructor. Methods named by what they do.
//-----------------------------------------------------------------------------
abstract_class IPulseHost_GameEntity
{
public:
	// The entity of m_hOwner, null when it is gone
	virtual CBaseEntity *GetOwnerEntity() = 0;
	// Finds the target entity and delivers the input to it
	virtual bool FireEntityInput( const char *pszTarget, const char *pszInput, const void *pValue ) = 0;
	// Writes the entity name with the name fixup of the instance (m_sNameFixupStaticPrefix,
	// m_sNameFixupParent, m_sNameFixupLocal) into sOut; false for an empty or reserved name
	virtual bool FixupEntityName( const char *pszName, CBufferString &sOut ) = 0;
};

//-----------------------------------------------------------------------------
// The entity input that started a cursor (CPulseInputHost holds the activator and the caller of
// the input; the server cursor answers with m_hActivator, or the owner of its graph instance, and
// the owner). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseHost_Input
{
public:
	virtual CEntityInstance *GetActivator() = 0;
	virtual CEntityInstance *GetCaller() = 0;
};

//-----------------------------------------------------------------------------
// A running cursor (CPulseExecCursor): its flow, its tags and the criteria of the requirement
// cells. No virtual destructor. Methods named by what they do; the tag methods are the ones the
// cursor library functions TagCursor, CursorHasTag and RemoveTagFromCursor call (their arguments
// are pTagName, tagValue, bInherit).
//-----------------------------------------------------------------------------
abstract_class IPulseHost_Cursor
{
public:
	// Resumes the frame the flow was called from (the last entry of the call stack) when it is
	// still valid, otherwise completes the cursor
	virtual void ReturnFromFlow() = 0;
	// Ends the cursor ("Completed with result:%d") and queues the result for the cursors that
	// wait for it
	virtual void Complete( int nResult ) = 0;
	// Adds the tag with the value; bInherit makes the child cursors copy it, bRecordHistory adds
	// it to the execution history. bFlag is stored with the tag only
	virtual void AddTag( const char *pszTagName, const CPulseCallContextValue &tagValue, bool bInherit, bool bFlag, bool bRecordHistory ) = 0;
	virtual bool HasTag( const char *pszTagName, const CPulseCallContextValue &tagValue ) const = 0;
	// An empty value when the cursor has no such tag
	virtual CPulseCallContextValue GetTagValue( const char *pszTagName ) const = 0;
	virtual void RemoveTag( const char *pszTagName, const CPulseCallContextValue &tagValue ) = 0;
	// Continues at the outflow (ReturnFromFlow without a destination), copying the values of its
	// register map
	virtual void JumpToOutflow( const CPulse_OutflowConnection &outflow, const void *pOutflowValues ) = 0;
	// Creates a child cursor (CreateCursor of the graph instance) and runs it at the outflow;
	// the reference stays empty when the child finished at once
	virtual PulseCursorRef_t CreateChildCursor( const CPulse_OutflowConnection &outflow, const void *pOutflowValues ) = 0;
	// Two overloads each: the compiler lays them out in pairs on Windows
	virtual CBasePulseGraphInstance *GetGraphInstance() = 0;
	virtual CPulseExecCursor *GetCursor() = 0;
	virtual const CBasePulseGraphInstance *GetGraphInstance() const = 0;
	virtual const CPulseExecCursor *GetCursor() const = 0;
	// Appends the criteria (the per-cursor state blocks) of the requirement cells of the outflow
	// whose criteria class is pszCriteriaClassName (CPulseCell_BaseRequirement::GetCriteriaClassName)
	virtual void GetOutflowCriteria( const OutflowWithRequirements_t &outflow, const char *pszCriteriaClassName, CUtlVector< void * > *pCriteria ) = 0;
	// Creates the criteria of the requirement cell (its CreateRequirementState) and keeps it
	// with the cursor; the requirement writes its result into it
	virtual void *AddCriteria( const CPulseCell_BaseRequirement *pRequirement ) = 0;
};

//-----------------------------------------------------------------------------
// A cursor that can yield and be woken (CPulseExecCursor). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseHost_YieldingCursor : public IPulseHost_Cursor
{
public:
	// Named by what it does: logs that the cursor yielded in the cell
	virtual void OnYielded( const CPulseCell_Base *pCell ) = 0;
	// Named by what it does: gives the cursor a new yield token and queues a yield resume event
	// (IPulseSystem::QueueYieldResumeEvent) for the outflow; completes the cursor instead while
	// the graph instance shuts down
	virtual void QueueYieldResume( const CPulse_OutflowConnection &outflow, const void *pOutflowValues ) = 0;
};

//-----------------------------------------------------------------------------
// A cursor whose observable variables can be watched (CPulseExecCursor). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseHost_BaseObservable
{
public:
	// Named by what it does: adds the symbol to the nearest observable scope of the cursor or of
	// its ancestors (CPulseCursorBaseObservableScope::AddObservable)
	virtual void AddObservable( const PulseSymbol_t &name ) = 0;
};

//-----------------------------------------------------------------------------
// A server cursor that observes entity outputs (CPulseServerCursor). No virtual destructor.
// Methods named by what they do: they add an entry (entity handle, output symbol) to the
// observable scope of the cursor. ObserveEntityOutput is overloaded: the compiler lays the two
// out in reverse order on Windows.
//-----------------------------------------------------------------------------
abstract_class IPulseHost_EntityObservable
{
public:
	virtual void ObserveEntityOutput( CEntityHandle hEntity, const char *pszOutputName ) = 0;
	virtual void ObserveEntityOutput( CEntityInstance *pEntity, const CEntityIOOutput *pOutput ) = 0;
	// The output symbol is "$$AG2Event::<pszEventName>"
	virtual void ObserveAnimGraphEvent( CEntityHandle hEntity, const char *pszEventName ) = 0;
};

//-----------------------------------------------------------------------------
// The log of a cursor (CPulseExecCursor). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseExecLog
{
public:
	// Named by what they do: format the text, pass it to the debug listener of the graph
	// instance and log it ("Pulse: %s") on the Pulse channel at LS_MESSAGE / LS_WARNING
	virtual void LogMessage( const char *pszFormat, ... ) = 0;
	virtual void LogWarning( const char *pszFormat, ... ) = 0;
};

//-----------------------------------------------------------------------------
// The compiled graph (CPulseGraphDef). No virtual destructor. The methods are named by what
// they do with the members of CPulseGraphDef (m_DomainIdentifier, m_Vars, m_PublicOutputs,
// m_DomainValues, m_Cells); a variable index out of range gives an empty value.
//-----------------------------------------------------------------------------
abstract_class IPulseGraphDef
{
public:
	virtual PulseSymbol_t GetDomainIdentifier() const = 0;
	virtual CPulseValueFullType GetDomainSubType() const = 0;
	virtual PulseSymbol_t GetParentMapName() const = 0;
	// Appends the blackboard resource of every entry of m_BlackboardReferences, each once
	virtual void GetBlackboardReferences( CUtlVector< PulseSymbol_t > *pBlackboards ) const = 0;
	// Builds the entity I/O signature of the graph (CPulseGraphIOSignature reads it): every cell
	// appends its entry (CPulseCell_Base::AppendIOSignature; the method entry cells are inputs),
	// then one entry per public output. pEntries is a CUtlVector of 224-byte entries
	virtual void BuildIOSignature( void *pEntries ) const = 0;
	// Appends the value of every domain value of the type
	virtual void GetDomainValuesOfType( int nType, CUtlVector< CGlobalSymbolCaseSensitive > *pValues ) const = 0;
	virtual const char *GetPublicOutputName( int nOutput ) const = 0;
	// Returns a pointer member that follows the schema members
	virtual void *Stub_7() const = 0;
	// Prints "== PulseGraphDef ==", the domain, the parent map, the game blackboards and the rest
	virtual void DumpGraphDef() const = 0;
	virtual int GetVariableCount() const = 0;
	// The index of the variable, -1 if there is none
	virtual int FindVariable( const char *pszName ) const = 0;
	virtual int FindVariableByEditorNodeID( int nEditorNodeID ) const = 0;
	virtual const KeyValues3 *GetVariableDefaultValue( int nVariable ) const = 0;
	// The PulseVariableKeysSource_t of the variable
	virtual int GetVariableKeysSource( int nVariable ) const = 0;
	virtual const char *GetVariableName( int nVariable ) const = 0;
	virtual const char *GetVariableDescription( int nVariable ) const = 0;
	// -1 out of range
	virtual int GetVariableEditorNodeID( int nVariable ) const = 0;
	virtual bool IsVariablePublicBlackboardVariable( int nVariable ) const = 0;
	virtual bool IsVariableObservable( int nVariable ) const = 0;
	virtual const CPulseValueFullType &GetVariableType( int nVariable ) const = 0;
	// Appends the indexes of the cells of the schema class named pszClassName
	virtual void GetCellsOfClass( const char *pszClassName, CUtlVector< int > *pCells ) const = 0;
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	// The method entry cell (CPulseCell_Inflow_Method) of the name, null if there is none
	virtual const CPulseCell_Base *FindMethodCell( const PulseSymbol_t &name ) const = 0;
};


//-----------------------------------------------------------------------------
// The type queries of one Pulse scope (CPulseScopeEnumeratingTypeService,
// CPulseSharedEHandleService). No virtual destructor. The scope loader forwards its own type
// queries (CPulseDomainScopeLoader::IsSubTypeOf and the next four) to the service of the
// value type. Methods named by what they do.
//-----------------------------------------------------------------------------
abstract_class IPulseTypeQueriesForScope
{
public:
	// The PulseValueType_t the queries are for (13, the entity handle, in
	// CPulseSharedEHandleService)
	virtual int GetValueType() const = 0;
	// Called once when the scope loader stores the service (BuildBindings); empty in both server
	// services
	virtual void OnRegistered() = 0;
	// Walks the base types of type (GetBaseType) up to baseType
	virtual bool IsSubTypeOf( const CPulseValueFullType &type, const CPulseValueFullType &baseType ) = 0;
	// The type of the value (GetValueTypeOf) is testType or a subtype of it; false for an invalid
	// handle
	virtual bool IsValueOfType( const CPulseValueFullType &type, const void *pValue, const CPulseValueFullType &testType ) = 0;
	// The value refers to an object that still exists
	virtual bool IsValueValid( const CPulseValueFullType &type, const void *pValue ) = 0;
	// The type the subtype of type derives from (an invalid type at the root)
	virtual CPulseValueFullType GetBaseType( const CPulseValueFullType &type ) = 0;
	// The type of the object the value refers to (type when there is none)
	virtual CPulseValueFullType GetValueTypeOf( const CPulseValueFullType &type, const void *pValue ) = 0;
	// Named by what it does: appends every subtype of the value type (every entity class for
	// the entity handle service)
	virtual void GetAllSubTypes( CUtlVector< CPulseValueFullType > *pTypes ) = 0;
};

//-----------------------------------------------------------------------------
// Passed to a graph instance when it loads its state (spawn, restore). No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseInstanceLoadAPI
{
public:
	// Takes a 32-bit id and returns one; the spawn and no-op versions return it unchanged
	virtual uint32 Stub_0( uint32 nId ) = 0;
};

//-----------------------------------------------------------------------------
// Passed to a graph instance when it saves its state. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class IPulseInstanceSaveAPI
{
public:
	// Takes a 32-bit id and returns it; the save game version records a nonzero id in a locked
	// hash table first
	virtual uint32 Stub_0( uint32 nId ) = 0;
};

//-----------------------------------------------------------------------------
// Collects the errors of a graph load (CPulseGraphLoadContext). No virtual destructor. Methods
// named by what they do.
//-----------------------------------------------------------------------------
abstract_class IPulseGraphLoadErrorReporter
{
public:
	// Push / pop a 16-byte context entry (two ints and a pointer) that the errors are filed under
	virtual void PushErrorContext( const void *pContext ) = 0;
	// Files the message under the innermost context; nothing without one
	virtual void ReportError( const char *pszMessage ) = 0;
	virtual void PopErrorContext() = 0;
};

#endif // PULSE_INTERFACE_H
