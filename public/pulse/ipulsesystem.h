//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Pulse graph runtime app system
//
//===========================================================================//

#ifndef IPULSESYSTEM_H
#define IPULSESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include <functional>

#include "tier0/platform.h"
#include "appframework/iappsystem.h"
#include "entity2/entitypulsecallcontext.h"
#include "resourcesystem/resourcehandle.h"
#include "tier0/globalsymbol.h"
#include "tier0/threadtools.h"
#include "tier0/utlhandletable.h"
#include "tier0/utlhashtable.h"
#include "tier0/utlmap.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/utlvector.h"
#include "tier3/tier3.h"

#include "schemacompiler/schemaon.h"

class CBasePulseGraphInstance;
class CPulseDomainScopeLoader;
class CPulseExecCursor;
class CPulseGraphDef;
class CPulseGraphExecutionHistory;
class CPulseGraphLoadContext;
class CReadOnlyBlackboardView;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CUtlString;
class IPulseGraphInstance_TestDomain;
class IPulseHost_YieldingCursor;
class IPulseReloadListener;

struct PulseSystemEventBase_t;

class InfoForResourceTypeCPulseGraphDef
{
public:
	using RuntimeClass_t = CPulseGraphDef;
};

using HPulseGraphDefWeak = CWeakHandle< InfoForResourceTypeCPulseGraphDef >;

schema class PulseGraphInstanceID_t
{
public:
	TYPEMETA( MIsBoxedIntegerType );
	DECLARE_SCHEMA_DATA_CLASS( PulseGraphInstanceID_t );

	uint32 m_Value;
};
COMPILE_TIME_ASSERT( sizeof( PulseGraphInstanceID_t ) == 4 );

schema class PulseDocNodeID_t
{
public:
	TYPEMETA( MIsBoxedIntegerType );

	int32 m_Value;
};

schema class PulseCursorID_t
{
public:
	TYPEMETA( MIsBoxedIntegerType );

	int32 m_Value;
};

// A name interned twice: m_Name keeps the case for display, m_Symbol is the case-insensitive identity that
// comparisons, ordering and hashing use. The copy is user-provided like the engine's, so a symbol is passed and
// returned by address.
struct PulseSymbol_t
{
	PulseSymbol_t() {}
	explicit PulseSymbol_t( const char *pszName ) : m_Symbol( MakeGlobalSymbol( pszName ) ), m_Name( MakeGlobalSymbolCaseSensitive( pszName ) ) {}
	PulseSymbol_t( const PulseSymbol_t &other ) : m_Name( other.m_Name ), m_Symbol( other.m_Symbol ) {}

	PulseSymbol_t &operator=( const PulseSymbol_t &other )
	{
		m_Symbol = other.m_Symbol;
		m_Name = other.m_Name;

		return *this;
	}

	bool operator==( const PulseSymbol_t &other ) const { return m_Symbol == other.m_Symbol; }
	bool operator!=( const PulseSymbol_t &other ) const { return m_Symbol != other.m_Symbol; }
	bool operator<( const PulseSymbol_t &other ) const { return GetSymbolString() < other.GetSymbolString(); }

	bool IsValid() const { return m_Symbol.IsValid(); }
	const char *String() const { return m_Name.String(); }

	// The global symbol table stores the hash right before the string
	uint32 GetHash() const { return m_Symbol.IsValid() ? reinterpret_cast< const uint32 * >( m_Symbol.String() )[ -1 ] : 0; }

private:
	const char *GetSymbolString() const { return m_Symbol.IsValid() ? m_Symbol.String() : nullptr; }

public:
	CGlobalSymbolCaseSensitive m_Name;
	CGlobalSymbol m_Symbol;
};

schema class CPulseBreakpointLocation
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CPulseBreakpointLocation() : m_NodeID{ -1 }, m_bDeferBreak( false ) {}

	PulseDocNodeID_t m_NodeID;
	PulseSymbol_t m_SequencePoint;
	PulseSymbol_t m_PortName;
	bool m_bDeferBreak;
};

// Argument or return value of a graph method, as the graph definition stores it.
schema class CPulseRuntimeMethodArg
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	CKV3MemberNameWithStorage m_Name;
	CUtlString m_Description;
	CPulseValueFullType m_Type;
	noschema ALIGN16 CPulseCallContextValue m_DefaultValue ALIGN16_POST;
};

// A graph hook called on one graph instance, with its arguments and its first return value.
struct ALIGN16 PulseGraphHookCall_t
{
	CPulseArgumentPack m_Args;
	CBasePulseGraphInstance *m_pInstance;
	PulseSymbol_t m_Domain;
	PulseSymbol_t m_HookName;
	ALIGN16 CPulseCallContextValue m_ReturnValue ALIGN16_POST;
	ALIGN16 CPulseCallContextValue m_Unk0E0 ALIGN16_POST;

	// Return value of the instance's hook call
	int32 m_nCallResult;
	bool m_bCalled;
} ALIGN16_POST;

// Result a cursor completes with, reported to the cell observing it
enum PulseCursorResult_t
{
	PULSE_CURSOR_RESULT_FAILED = 0,
	PULSE_CURSOR_RESULT_SUCCEEDED = 1,
	// The observer gets its cancel callback instead of the completion one
	PULSE_CURSOR_RESULT_CANCELED = 2,
};

abstract_class IPulseCursorListener
{
public:
	// Key of the listener's table under "listeners" in the saved state of a cursor
	virtual CKV3MemberName GetName() = 0;

	virtual void SaveCursorState( CPulseExecCursor *pCursor, CKV3TransferSaveContext *pContext ) = 0;
	virtual void LoadCursorState( CPulseExecCursor *pCursor, CKV3TransferLoadContext *pContext ) = 0;

	// Called by IPulseSystem::RemovePendingEventsForCursor with its cursor
	virtual void RemovePendingEventsForCursor( CPulseExecCursor *pCursor ) = 0;
};

abstract_class IPulseBlackboardView
{
public:
	virtual ~IPulseBlackboardView() {}

	// Finds a variable by its editor node or by name and returns its type and storage
	virtual bool FindBlackboardReference( PulseDocNodeID_t nNodeID, const char *pszName, CPulseValueFullType &type, void **ppValue ) = 0;

	// The graph definition the blackboard's variables come from; RegisterBlackboard builds the read-only view from it
	virtual HPulseGraphDefWeak GetBlackboardResource() = 0;
	virtual PulseSymbol_t GetBlackboardResourceName() = 0;

	// Graph instances notified when an observable variable of the blackboard changes
	virtual void AddObservableVariableListener( PulseGraphInstanceID_t listenerID ) = 0;
	virtual void RemoveObservableVariableListener( PulseGraphInstanceID_t listenerID ) = 0;

	// The read-only view saves its values, rebuilds itself from its resource and loads them back; empty in the view of a
	// graph instance
	virtual void ReloadFromResource() = 0;

	// The graph instance whose variables the view shows; null for a read-only view
	virtual CBasePulseGraphInstance *GetGraphInstance() = 0;
};

// A line the turtle cursor draws while its pen is down
struct PulseTurtleGraphicsLine_t
{
	Vector2D m_vecStart;
	Vector2D m_vecEnd;
	Color m_Color;
};

abstract_class IPulseTurtleGraphicsListener
{
public:
	// Called after every line the cursor appends
	virtual void OnLineAdded() = 0;
};

abstract_class IPulseGraphInstance_TurtleGraphics
{
public:
	virtual ~IPulseGraphInstance_TurtleGraphics() {}

	virtual CBasePulseGraphInstance *GetGraphInstance() = 0;
	virtual CUtlVector< PulseTurtleGraphicsLine_t > *GetLines() = 0;

	// Calls the graph's Go hook
	virtual void Go() = 0;

	virtual void SetListener( IPulseTurtleGraphicsListener *pListener ) = 0;
};

// Opcodes of the compiled graph instructions; the execution history records some of them as events
schema enum class PulseInstructionCode_t : uint16
{
	INVALID = 0,
	IMMEDIATE_HALT = 1,
	RETURN_VOID = 2,
	RETURN_VALUE = 3,
	LOOP_BREAK = 4,
	NOP = 5,
	JUMP = 6,
	JUMP_COND = 7,
	CHUNK_LEAP = 8,
	CHUNK_LEAP_COND = 9,
	PULSE_CALL_SYNC = 10,
	PULSE_CALL_ASYNC_FIRE = 11,
	CREATE_CHILD_CURSOR_OUTFLOW = 12,
	CELL_INVOKE = 13,
	LIBRARY_INVOKE = 14,
	SET_VAR = 15,
	GET_VAR = 16,
	GET_VAR_DETACH = 17,
	DETACH_REGISTER = 18,
	SET_VAR_ARRAY_ELEMENT_1D = 19,
	SET_VAR_OBSERVABLE = 20,
	GET_CONST = 21,
	GET_ARRAY_ELEMENT = 22,
	GET_DOMAIN_VALUE = 23,
	COPY = 24,
	NOT = 25,
	NEGATE = 26,
	ADD = 27,
	SUB = 28,
	MUL = 29,
	DIV = 30,
	MOD = 31,
	LT = 32,
	LTE = 33,
	EQ = 34,
	NE = 35,
	AND = 36,
	OR = 37,
	SCALE = 38,
	SCALE_INV = 39,
	ELEMENT_ACCESS = 40,
	CONVERT_VALUE = 41,
	REINTERPRET_INSTANCE = 42,
	GET_BLACKBOARD_REFERENCE = 43,
	SET_BLACKBOARD_REFERENCE = 44,
	GET_TEMPVAR = 45,
	SET_TEMPVAR = 46,
	SET_TEMPVAR_OBSERVABLE = 47,
	LAST_SERIALIZED_CODE = 48,
	NEGATE_INT = 49,
	NEGATE_FLOAT = 50,
	NEGATE_VEC2 = 51,
	NEGATE_VEC3 = 52,
	NEGATE_VEC4 = 53,
	ADD_INT = 54,
	ADD_FLOAT = 55,
	ADD_STRING = 56,
	ADD_VEC2 = 57,
	ADD_VEC3 = 58,
	ADD_VEC3WS_VEC3 = 59,
	ADD_VEC3_VEC3WS = 60,
	ADD_VEC4 = 61,
	ADD_GAMETIME_FLOAT = 62,
	ADD_FLOAT_GAMETIME = 63,
	SUB_INT = 64,
	SUB_FLOAT = 65,
	SUB_VEC2 = 66,
	SUB_VEC3 = 67,
	SUB_VEC3WS_VEC3 = 68,
	SUB_VEC3WS_VEC3WS = 69,
	SUB_VEC4 = 70,
	SUB_GAMETIME_FLOAT = 71,
	SUB_GAMETIME = 72,
	MUL_INT = 73,
	MUL_FLOAT = 74,
	DIV_FLOAT = 75,
	MOD_INT = 76,
	MOD_FLOAT = 77,
	LT_INT = 78,
	LT_FLOAT = 79,
	LT_GAMETIME = 80,
	LTE_INT = 81,
	LTE_FLOAT = 82,
	LTE_GAMETIME = 83,
	EQ_BOOL = 84,
	EQ_INT = 85,
	EQ_FLOAT = 86,
	EQ_VEC2 = 87,
	EQ_VEC3 = 88,
	EQ_VEC3WS = 89,
	EQ_VEC4 = 90,
	EQ_STRING = 91,
	EQ_ENTITY_NAME = 92,
	EQ_SCHEMA_ENUM = 93,
	EQ_EHANDLE = 94,
	EQ_PANEL_HANDLE = 95,
	EQ_OPAQUE_HANDLE = 96,
	EQ_TEST_HANDLE = 97,
	EQ_COLOR_RGB = 98,
	EQ_ARRAY = 99,
	EQ_GAMETIME = 100,
	NE_BOOL = 101,
	NE_INT = 102,
	NE_FLOAT = 103,
	NE_VEC2 = 104,
	NE_VEC3 = 105,
	NE_VEC3WS = 106,
	NE_VEC4 = 107,
	NE_STRING = 108,
	NE_ENTITY_NAME = 109,
	NE_SCHEMA_ENUM = 110,
	NE_EHANDLE = 111,
	NE_PANEL_HANDLE = 112,
	NE_OPAQUE_HANDLE = 113,
	NE_TEST_HANDLE = 114,
	NE_COLOR_RGB = 115,
	NE_ARRAY = 116,
	NE_GAMETIME = 117,
	SCALE_VEC3 = 118,
	SCALE_VEC2 = 119,
	SCALE_VEC4 = 120,
	SCALE_INV_VEC3 = 121,
	SCALE_INV_VEC2 = 122,
	SCALE_INV_VEC4 = 123,
	ELEMENT_ACCESS_VEC2 = 124,
	ELEMENT_ACCESS_VEC3 = 125,
	ELEMENT_ACCESS_VEC3WS = 126,
	ELEMENT_ACCESS_VEC4 = 127,
	ELEMENT_ACCESS_COLOR_RGB = 128,
	GET_CONST_INLINE_STORAGE = 129,
};

// Sources of the variable values a graph instance is loaded with
schema enum class PulseVariableKeysSource_t
{
	PRIVATE = 0,
	CPP = 1,
	VMAP = 2,
	VMDL = 3,
	XML = 4,
	VDATA = 5,
	COUNT = 6,
};

abstract_class IPulseGraphLoadErrorReporter
{
public:
	virtual void PushErrorScope( const CKV3MemberName &name ) = 0;
	// Stores the message under the innermost scope's name
	virtual void ReportError( const char *pszMessage ) = 0;
	virtual void PopErrorScope() = 0;
};

class CPulseGraphLoadContext : public IPulseGraphLoadErrorReporter
{
public:
	CKV3TransferLoadContext *m_pTransferContext;
	bool m_bOwnsTransferContext;

	const KeyValues3 *m_pVariableKeys[ static_cast< int >( PulseVariableKeysSource_t::COUNT ) ];

	CUtlVector< CKV3MemberName > m_ErrorScopes;
	KeyValues3 m_Errors;
};

abstract_class IPulseSystem : public IAppSystem
{
public:
	virtual void AddListener( void *pListener ) = 0;
	virtual void RemoveListener( void *pListener ) = 0;

	virtual void SetInstanceDebugger( CBasePulseGraphInstance *pInstance, void *pDebugger ) = 0;

	virtual CBasePulseGraphInstance *FindGraphInstance( PulseGraphInstanceID_t id ) = 0;
	virtual PulseGraphInstanceID_t AllocateGraphInstanceID() = 0;

	virtual void AddCursorToAutoWakeList( IPulseHost_YieldingCursor *pCursor, float flDelay ) = 0;
	// Returns false when the cursor is not in the auto-wake list.
	virtual bool GetCursorAutoWakeTime( IPulseHost_YieldingCursor *pCursor, float *pflWakeTime ) = 0;
	virtual void RemoveCursorFromAutoWakeList( IPulseHost_YieldingCursor *pCursor ) = 0;

	virtual int ProcessPendingEvents( const PulseSymbol_t &clock ) = 0;
	virtual int GetNumAutoWakeCursors( const PulseSymbol_t &clock ) = 0;

	virtual void QueueYieldResumeEvent( CPulseExecCursor *pCursor, const void *pResumeData ) = 0;
	// nSubject is the id of the cursor that completed; the observer is notified when the events are processed
	virtual void QueueCursorResultEvent( int nSubject, const void *phObserver, PulseCursorResult_t nResult, const CPulseArgumentPack &outParams ) = 0;
	virtual void QueueObservableVariableSourceChangedEvent( const PulseSymbol_t &clock, CBasePulseGraphInstance *pInstance, int nVariable, const CPulseCallContextValue &prevValue, const CPulseCallContextValue &curValue ) = 0;
	virtual void QueueObservableVariableListenerNotifyEvent( const PulseSymbol_t &clock, const PulseSymbol_t &blackboardResourceName, CBasePulseGraphInstance *pListenerInstance, PulseDocNodeID_t nVarDocNodeID, const CPulseCallContextValue &prevValue, const CPulseCallContextValue &curValue ) = 0;

	// One domain scope loader per module scope ( pulse system, server, client, resource compiler, test ), null when
	// the module did not register one
	virtual CPulseDomainScopeLoader **GetTypeQueriesForScopes() = 0;
	virtual CUtlHandleTable< void, 10 > *GetWeakHandleTable() = 0;

	virtual void ForEachGraphInstance( const std::function< void( CBasePulseGraphInstance * ) > &func ) = 0;

	// Appends the live instances of the graph to the vector.
	virtual void GetGraphInstances( ResourceHandle_t hGraph, CUtlVector< CBasePulseGraphInstance * > &instances ) = 0;

	virtual void RemoveBreakpoint( ResourceHandle_t hGraph, void *pBreakpoint ) = 0;
	virtual void *AddBreakpoint( ResourceHandle_t hGraph, const CPulseBreakpointLocation *pLocation, bool *pbCreated ) = 0;
	// Without bExactMatch an unset location on either side also matches.
	virtual void *FindBreakpoint( ResourceHandle_t hGraph, const CPulseBreakpointLocation *pLocation, bool bExactMatch ) = 0;
	virtual void GetBreakpoints( ResourceHandle_t hGraph, const PulseDocNodeID_t *pNodeID, CUtlVector< void * > &breakpoints ) = 0;
	virtual void GetAllBreakpoints( ResourceHandle_t hGraph, CUtlVector< void * > &breakpoints ) = 0;

	virtual void AddCursorStateListener( IPulseCursorListener *pListener ) = 0;
	virtual void RemoveCursorStateListener( IPulseCursorListener *pListener ) = 0;

	// Per-instance record of cursor steps ( cell, clock time ).
	virtual void ClearAllExecutionHistory() = 0;
	virtual void DumpExecutionHistory( const char *pszFilter ) = 0;
	virtual void ClearExecutionHistory( PulseGraphInstanceID_t id ) = 0;
	virtual CPulseGraphExecutionHistory *GetExecutionHistory( PulseGraphInstanceID_t id ) = 0;

	// Opaque pointer stored with the graph's runtime data.
	virtual void SetGraphUserData( ResourceHandle_t hGraph, void *pUserData ) = 0;
	virtual void *GetGraphUserData( ResourceHandle_t hGraph ) = 0;

	virtual CPulseExecCursor *GetExecutingCursor() = 0;
	virtual bool PushExecutingCursor( CPulseExecCursor *pCursor, CPulseExecCursor **ppPrevious ) = 0;
	virtual void PopExecutingCursor( CPulseExecCursor *pCursor, CPulseExecCursor *pPrevious ) = 0;

	// A negative delay cancels the scheduled call of the same instance and hook
	virtual void ScheduleGraphHookCall( const PulseGraphHookCall_t &call, float flDelay ) = 0;

	// Only sets an internal flag.
	virtual void unk046() = 0;

	virtual HPulseGraphDefWeak CreateTestGraphDef( CPulseGraphDef *pDef ) = 0;

	// pReloadListener gets its first method called before the swap and is offered to each instance's EndGraphReload
	// as the IPulseReloadListener transfer interface
	virtual bool ReplaceGraphDefData( ResourceHandle_t hGraph, CPulseGraphDef *pNewDef, IPulseReloadListener *pReloadListener ) = 0;

	virtual int GetTestIntValue() = 0;
	virtual void SetTestIntValue( int nValue, bool bNotify ) = 0;
	virtual float *GetTestFloatValue() = 0;
	virtual CUtlString *GetTestStringValue() = 0;

	// The keys are loaded as the C++ variable keys source of the instance
	virtual IPulseGraphInstance_TurtleGraphics *CreateTurtleGraphicsInstance( ResourceHandle_t hGraph, const KeyValues3 *pVariableKeys ) = 0;
	virtual IPulseGraphInstance_TestDomain *CreateTestDomainInstance( const PulseSymbol_t &domain, ResourceHandle_t hGraph, const KeyValues3 *pVariableKeys ) = 0;
	// Time of the TestClock_ExplicitStepped test clock.
	virtual void SetTestClockTime( double flTime ) = 0;

	virtual void RegisterBlackboard( const PulseSymbol_t &name, IPulseBlackboardView *pBlackboard ) = 0;
	virtual IPulseBlackboardView *FindBlackboard( const PulseSymbol_t &name ) = 0;
	virtual void RemoveAllBlackboards() = 0;
	// Copies every registered blackboard into its read-only view.
	virtual void UpdateReadOnlyBlackboardViews() = 0;

	virtual void EnableMinimalDefinitionLoading() = 0;
	virtual bool IsMinimalDefinitionLoadingEnabled() = 0;

	virtual void CallGraphHookOnAllInstances( const PulseSymbol_t &domain, const PulseSymbol_t &hook, void *pArgs ) = 0;

	virtual void Internal_InstanceCreated( CBasePulseGraphInstance *pInstance ) = 0;
	virtual void Internal_InstanceDestroyed( CBasePulseGraphInstance *pInstance ) = 0;

	virtual bool InitGraphInstance( CBasePulseGraphInstance *pInstance, ResourceHandle_t hGraph, CPulseGraphLoadContext *pLoadContext ) = 0;

	virtual void LoadInstanceSystemState( CBasePulseGraphInstance *pInstance, CKV3TransferLoadContext *pContext ) = 0;
	virtual void SaveInstanceSystemState( CBasePulseGraphInstance *pInstance, CKV3TransferSaveContext *pContext ) = 0;

	virtual bool IsDomainAvailable( const char *pszDomain, int nScope ) = 0;
	virtual CBasePulseGraphInstance *CreateGraphInstanceForDomain( const char *pszDomain ) = 0;

	virtual void RemovePendingEventsForCursor( CPulseExecCursor *pCursor ) = 0;
	// RETURN_VOID and RETURN_VALUE flag the entry as RETURN and PULSE_CALL_SYNC as CALL_TO_PULSE
	// ( EPulseGraphExecutionHistoryFlag ); after an entry of the same cursor, NOP and
	// any other instruction on the same cell add no entry
	virtual void RecordExecutionHistory( CPulseExecCursor *pCursor, PulseInstructionCode_t nEventType, int nEditorID, const PulseSymbol_t &seqPoint ) = 0;

	virtual void LoadCursorSystemState( CPulseExecCursor *pCursor, CKV3TransferLoadContext *pContext ) = 0;
	virtual void SaveCursorSystemState( CPulseExecCursor *pCursor, CKV3TransferSaveContext *pContext ) = 0;

	// Forwards a breakpoint add or remove to every listener.
	virtual void NotifyBreakpointChanged( ResourceHandle_t hGraph, const CPulseBreakpointLocation *pLocation ) = 0;

	// True when any listener's fifth method returns true; the engine ticks the server in parallel
	// with the client (sv_tick_parallel_with_client) only while this returns false
	virtual bool ShouldBlockParallelTick() = 0;
};

class CPulseSystem : public CTier3AppSystem< IPulseSystem >
{
public:
	struct AutoWakeCursor_t
	{
		PulseGraphInstanceID_t m_nGraphInstanceID;
		PulseCursorID_t m_nCursorID;
		int32 m_nYieldToken;
		float m_flWakeTime;
		PulseSymbol_t m_Clock;
	};

	struct ScheduledGraphHookCall_t
	{
		PulseGraphInstanceID_t m_nGraphInstanceID;
		PulseGraphHookCall_t m_Call;
		float m_flCallTime;
		PulseSymbol_t m_Clock;
	};

	// Added by AddListener
	CUtlVector< void * > m_Listeners;

	CUtlSymbolTableLarge m_Unk040;

	// CPulseTypeManager, the resource type manager of the graph definitions
	uint64 m_TypeManager[ 6 ];

	// Set by unk046
	bool m_bUnk0D8;

	bool m_bMinimalDefinitionLoadingEnabled;

	// Incremented by AllocateGraphInstanceID, which returns the new value
	uint32 m_nLastGraphInstanceID;

	// Appended by the Queue*Event methods
	CUtlVector< PulseSystemEventBase_t * > m_PendingEvents;

	// AddCursorToAutoWakeList, GetCursorAutoWakeTime, RemoveCursorFromAutoWakeList and GetNumAutoWakeCursors
	CUtlVectorFixedGrowable< AutoWakeCursor_t, 128 > m_AutoWakeCursors;

	// Appended by ScheduleGraphHookCall
	CUtlVectorFixedGrowable< ScheduledGraphHookCall_t, 128 > m_ScheduledGraphHookCalls;

	// Per-graph data ( instances, breakpoints, user data ) keyed by the 64-bit hash of the graph resource name
	CUtlHashtable< uint64, void * > m_GraphData;

	CUtlHashtable< uint32, CBasePulseGraphInstance *, MurmurHash3IntFunctor > m_GraphInstances;

	// Allocated by the constructor, returned by GetTypeQueriesForScopes
	CPulseDomainScopeLoader **m_ppDomainScopeLoaders;

	CUtlHandleTable< void, 10 > *m_pWeakHandleTable;

	CUtlMap< PulseSymbol_t, IPulseBlackboardView *, int > m_Blackboards;
	CUtlMap< PulseSymbol_t, CReadOnlyBlackboardView *, int > m_ReadOnlyBlackboardViews;

	// Set when Init installs m_TypeManager, cleared when Shutdown removes it
	bool m_bTypeManagerInstalled;

	CUtlVector< IPulseCursorListener * > m_CursorStateListeners;

	// Guards m_GraphInstances and the graph instance id counter
	CAtomicMutex m_GraphInstancesMutex;
};

#include "schemacompiler/schemaoff.h"

#endif // IPULSESYSTEM_H
