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
#include "resourcesystem/resourcehandle.h"
#include "tier0/threadtools.h"
#include "tier0/utlhandletable.h"
#include "tier0/utlsymbollarge.h"
#include "tier0/utlvector.h"
#include "tier3/tier3.h"

#include "schemacompiler/schemaon.h"

class CBasePulseGraphInstance;
class CPulseArgumentPack;
class CPulseExecCursor;
class CPulseGraphDef;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CUtlString;
class IPulseBlackboardView;
class IPulseGraphInstance_TestDomain;
class IPulseGraphInstance_TurtleGraphics;

struct PulseSymbol_t;
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

abstract_class IPulseSystem : public IAppSystem
{
public:
	virtual void AddListener( void *pListener ) = 0;
	virtual void RemoveListener( void *pListener ) = 0;

	virtual void SetInstanceDebugger( CBasePulseGraphInstance *pInstance, void *pDebugger ) = 0;

	virtual CBasePulseGraphInstance *FindGraphInstance( PulseGraphInstanceID_t id ) = 0;
	virtual PulseGraphInstanceID_t AllocateGraphInstanceID() = 0;

	virtual void AddCursorToAutoWakeList( CPulseExecCursor *pCursor, float flDelay ) = 0;
	// Returns false when the cursor is not in the auto-wake list.
	virtual bool GetCursorAutoWakeTime( CPulseExecCursor *pCursor, float *pflWakeTime ) = 0;
	virtual void RemoveCursorFromAutoWakeList( CPulseExecCursor *pCursor ) = 0;

	virtual int ProcessPendingEvents( const PulseSymbol_t &clock ) = 0;
	virtual int GetNumAutoWakeCursors( const PulseSymbol_t &clock ) = 0;

	virtual void QueueYieldResumeEvent( CPulseExecCursor *pCursor, const void *pResumeData ) = 0;
	virtual void QueueCursorResultEvent( int nSubject, const void *phObserver, int nResult, const CPulseArgumentPack &outParams ) = 0;
	virtual void QueueObservableVariableSourceChangedEvent( const PulseSymbol_t &clock, CBasePulseGraphInstance *pInstance, int nVariable, const void *pPrevValue, const void *pCurValue ) = 0;
	virtual void QueueObservableVariableListenerNotifyEvent( const PulseSymbol_t &clock, const PulseSymbol_t &blackboardResourceName, CBasePulseGraphInstance *pListenerInstance, int nVarDocNodeID, const void *pPrevValue, const void *pCurValue ) = 0;

	virtual void *GetTypeQueriesForScopes() = 0;
	virtual CUtlHandleTable< void, 10 > *GetOpaqueHandleTable() = 0;

	virtual void ForEachGraphInstance( const std::function< void( CBasePulseGraphInstance * ) > &func ) = 0;

	// Appends the live instances of the graph to the vector.
	virtual void GetGraphInstances( ResourceHandle_t hGraph, CUtlVector< CBasePulseGraphInstance * > &instances ) = 0;

	virtual void RemoveBreakpoint( ResourceHandle_t hGraph, void *pBreakpoint ) = 0;
	virtual void *AddBreakpoint( ResourceHandle_t hGraph, const void *pBreakpointDesc, bool *pbCreated ) = 0;
	// Without bExactMatch an unset location on either side also matches.
	virtual void *FindBreakpoint( ResourceHandle_t hGraph, const void *pBreakpointDesc, bool bExactMatch ) = 0;
	virtual void GetBreakpoints( ResourceHandle_t hGraph, const int *pnKey, CUtlVector< void * > &breakpoints ) = 0;
	virtual void GetAllBreakpoints( ResourceHandle_t hGraph, CUtlVector< void * > &breakpoints ) = 0;

	virtual void AddCursorStateListener( void *pListener ) = 0;
	virtual void RemoveCursorStateListener( void *pListener ) = 0;

	// Per-instance record of cursor steps ( cell, clock time ).
	virtual void ClearAllExecutionHistory() = 0;
	virtual void DumpExecutionHistory( const char *pszFilter ) = 0;
	virtual void ClearExecutionHistory( PulseGraphInstanceID_t id ) = 0;
	virtual void *GetExecutionHistory( PulseGraphInstanceID_t id ) = 0;

	// Opaque pointer stored with the graph's runtime data.
	virtual void SetGraphUserData( ResourceHandle_t hGraph, void *pUserData ) = 0;
	virtual void *GetGraphUserData( ResourceHandle_t hGraph ) = 0;

	virtual CPulseExecCursor *GetExecutingCursor() = 0;
	virtual bool PushExecutingCursor( CPulseExecCursor *pCursor, CPulseExecCursor **ppPrevious ) = 0;
	virtual void PopExecutingCursor( CPulseExecCursor *pCursor, CPulseExecCursor *pPrevious ) = 0;

	virtual void ScheduleGraphHookCall( const void *pHookCall, float flDelay ) = 0;

	// Only sets an internal flag.
	virtual void unk046() = 0;

	virtual HPulseGraphDefWeak CreateTestGraphDef( CPulseGraphDef *pDef ) = 0;

	virtual bool ReplaceGraphDefData( ResourceHandle_t hGraph, CPulseGraphDef *pNewDef, void *pUnk ) = 0;

	virtual int GetTestIntValue() = 0;
	virtual void SetTestIntValue( int nValue, bool bNotify ) = 0;
	virtual float *GetTestFloatValue() = 0;
	virtual CUtlString *GetTestStringValue() = 0;

	virtual IPulseGraphInstance_TurtleGraphics *CreateTurtleGraphicsInstance( ResourceHandle_t hGraph, void *pUnk ) = 0;
	virtual IPulseGraphInstance_TestDomain *CreateTestDomainInstance( const PulseSymbol_t &domain, ResourceHandle_t hGraph, void *pUnk ) = 0;
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

	virtual bool InitGraphInstance( CBasePulseGraphInstance *pInstance, ResourceHandle_t hGraph, void *pLoadContext ) = 0;

	virtual void LoadInstanceSystemState( CBasePulseGraphInstance *pInstance, CKV3TransferLoadContext *pContext ) = 0;
	virtual void SaveInstanceSystemState( CBasePulseGraphInstance *pInstance, CKV3TransferSaveContext *pContext ) = 0;

	virtual bool IsDomainAvailable( const char *pszDomain, int nScope ) = 0;
	virtual CBasePulseGraphInstance *CreateGraphInstanceForDomain( const char *pszDomain ) = 0;

	virtual void RemovePendingEventsForCursor( CPulseExecCursor *pCursor ) = 0;
	virtual void RecordExecutionHistory( CPulseExecCursor *pCursor, uint16 nEventType, int nEditorID, const PulseSymbol_t &seqPoint ) = 0;

	virtual void LoadCursorSystemState( CPulseExecCursor *pCursor, CKV3TransferLoadContext *pContext ) = 0;
	virtual void SaveCursorSystemState( CPulseExecCursor *pCursor, CKV3TransferSaveContext *pContext ) = 0;

	// Forwards a breakpoint add or remove to every listener.
	virtual void NotifyBreakpointChanged( ResourceHandle_t hGraph, const void *pBreakpointDesc ) = 0;
	virtual bool unk075() = 0;
};

class CPulseSystem : public CTier3AppSystem< IPulseSystem >
{
public:
	struct AutoWakeCursor_t
	{
		uint8 m_pad000[ 0x20 ];
	};

	struct ALIGN16 ScheduledGraphHookCall_t
	{
		uint8 m_pad000[ 0x140 ];
	} ALIGN16_POST;

	// Added by AddListener
	CUtlVector< void * > m_Listeners;

	CUtlSymbolTableLarge m_Unk040;

	// CPulseTypeManager
	uint64 m_Unk0A8[ 6 ];

	// Set by unk046
	bool m_bUnk0D8;

	bool m_bMinimalDefinitionLoadingEnabled;

	// Incremented by AllocateGraphInstanceID
	uint32 m_nUnk0DC;

	// Appended by the Queue*Event methods
	CUtlVector< PulseSystemEventBase_t * > m_PendingEvents;

	// AddCursorToAutoWakeList, GetCursorAutoWakeTime, RemoveCursorFromAutoWakeList and GetNumAutoWakeCursors
	CUtlVectorFixedGrowable< AutoWakeCursor_t, 128 > m_AutoWakeCursors;

	// Appended by ScheduleGraphHookCall
	CUtlVectorFixedGrowable< ScheduledGraphHookCall_t, 128 > m_ScheduledGraphHookCalls;

	// CUtlHashtable with 24-byte entries, walked by CallGraphHookOnAllInstances
	uint64 m_UnkB130[ 4 ];

	// CUtlHashtable with 24-byte entries, walked by ForEachGraphInstance and the execution history methods
	uint64 m_UnkB150[ 4 ];

	// Returned by GetTypeQueriesForScopes
	void *m_pUnkB170;

	CUtlHandleTable< void, 10 > *m_pOpaqueHandleTable;

	// CUtlMap< PulseSymbol_t, IPulseBlackboardView *, int > filled by RegisterBlackboard and read by FindBlackboard
	uint64 m_Blackboards[ 5 ];

	// CUtlMap of the read-only blackboard views refreshed by UpdateReadOnlyBlackboardViews
	uint64 m_UnkB1A8[ 5 ];

	bool m_bUnkB1D0;

	// Added by AddCursorStateListener
	CUtlVector< void * > m_CursorStateListeners;

	// Guards the graph instances
	CAtomicMutex m_UnkB1F0;
};

#include "schemacompiler/schemaoff.h"

#endif // IPULSESYSTEM_H
