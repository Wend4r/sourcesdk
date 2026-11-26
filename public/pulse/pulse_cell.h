//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Virtual tables of the Pulse cells (the nodes of a compiled graph) that introduce
//          virtual methods.
//
//=============================================================================//

#ifndef PULSE_CELL_H
#define PULSE_CELL_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "pulse/pulsesymbol.h"

class CBufferString;
class CEntityHandle;
class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CPulseExecCursor;
class IPulseCellDebugInfo;
class IPulseHost_YieldingCursor;

// Size and alignment of the per-instance state block of a cell ({ 0, 1 } for a cell without one).
// Named by what the code does with it
struct PulseCellStateLayout_t
{
	int m_nSize;
	int m_nAlignment;
};

//-----------------------------------------------------------------------------
// The cells are schema classes; these classes declare their virtual methods only (see
// playerpawn_component.h for the VTable pattern). Every class below CPulseCell_BaseVTable is a
// template over its base class and adds only the virtual methods its own level introduces:
//
//   class CPulseCell_Base : public CPulseCell_BaseVTable {};
//   class CPulseCell_BaseFlow : public CPulseCell_BaseFlowVTable< CPulseCell_Base > {};
//
// The cells have no virtual destructor. The cells of a graph definition are const: every
// method that runs a cell takes the cursor and the per-instance state as arguments.
//-----------------------------------------------------------------------------
abstract_class CPulseCell_BaseVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual void KV3TransferSave( CKV3TransferSaveContext *pContext ) const = 0;
	virtual void KV3TransferLoad( CKV3TransferLoadContext *pContext ) = 0;
	virtual const char *GetClassName() const = 0;
	// Named by what it does: writes a description of the cell into sOut, the schema class name by default
	virtual void GetDebugName( void *pUnused, CBufferString &sOut ) const = 0;
	// Named by what it does: whether pClass is the schema class of the cell or one of its bases
	virtual bool IsA( const CSchemaClassInfo *pClass ) const = 0;
	// Named by what it does: appends the entry of the cell to the entity I/O signature of the graph
	// (IPulseGraphDef::BuildIOSignature; pEntries is a CUtlVector of 224-byte entries): the method
	// entry cell an input with its arguments, the entity output handler "<class>::<output> (Entity
	// Output Handler)"; empty in the other cells
	virtual void AppendIOSignature( void *pEntries ) const = 0;
	// Named by what they do: the per-instance state block of the cell ({ 4, 4 } for the
	// counting cells), its construction, destruction and save / load ("Double-save to Member '%s'")
	virtual PulseCellStateLayout_t GetCellStateLayout() const = 0;
	virtual void ConstructCellState( void *pState ) const = 0;
	virtual void DestructCellState( void *pState ) const = 0;
	virtual void LoadCellState( void *pState, CKV3TransferLoadContext *pContext ) const = 0;
	virtual void SaveCellState( const void *pState, CKV3TransferSaveContext *pContext ) const = 0;
};

//-----------------------------------------------------------------------------
// Adds no virtual method.
//-----------------------------------------------------------------------------
template < class TBase = CPulseCell_BaseVTable >
abstract_class CPulseCell_BaseFlowVTable : public TBase
{
};

//-----------------------------------------------------------------------------
// A cell a cursor waits in: the cursor yields in the cell and the cell is told when the
// cursor is woken, a child cursor or an external call finishes, a watched variable changes or
// an entity output fires. Methods named by what they do.
//-----------------------------------------------------------------------------
template < class TBase = CPulseCell_BaseFlowVTable<> >
abstract_class CPulseCell_BaseYieldingInflowVTable : public TBase
{
public:
	// Called by the cursor when it is woken or cancelled immediately
	virtual void WakeCursor( CPulseExecCursor *pCursor ) const = 0;
	// Delivers the result of a child cursor or of an external call (IPulseSystem::QueueCursorResultEvent)
	virtual void OnCursorResult( CPulseExecCursor *pCursor, const void *pResultInfo, bool bFailed, const void *pReturnValues ) const = 0;
	// Empty by default; CPulseCell_WaitForCursorsWithTagBase counts the other cursors with its tag
	// and, when they allow it, tags the cursor and queues its resume at m_WaitComplete. What
	// triggers it is not seen
	virtual void Stub_14( IPulseHost_YieldingCursor *pCursor ) const = 0;
	// The auto-wake time of the cursor (IPulseSystem::AddCursorToAutoWakeList) is reached
	virtual void OnCursorWake( CPulseExecCursor *pCursor ) const = 0;
	// Overridden only by the cells that wait on observable variables
	virtual void OnObservableVariableChanged( CPulseExecCursor *pCursor, const PulseSymbol_t &variable, int nVariable ) const = 0;
	// Overridden by the cells that listen to entity outputs and scene events
	virtual void OnEntityOutputFired( CPulseExecCursor *pCursor, const char *pszOutput, const void *pValue, const CEntityHandle *phCaller ) const = 0;
	// The per-cursor state of the cell (null by default), its destruction and save / load under
	// the member "state"
	virtual void *CreateCursorState() const = 0;
	virtual void DestroyCursorState( void *pState ) const = 0;
	virtual bool SaveCursorState( const void *pState, CKV3TransferSaveContext *pContext ) const = 0;
	virtual bool LoadCursorState( void *pState, CKV3TransferLoadContext *pContext ) const = 0;
	// Empty in every cell
	virtual void Stub_22() = 0;
	// Fills the debug info of the cell's own IPulseCellDebugInfo subclass
	virtual bool GetCursorDebugInfo( CPulseExecCursor *pCursor, IPulseCellDebugInfo *pInfo ) const = 0;
};

//-----------------------------------------------------------------------------
// A requirement cell (CPulseCell_IsRequirementValid, CPulseCell_LimitCount). Methods named by
// what they do.
//-----------------------------------------------------------------------------
template < class TBase = CPulseCell_BaseVTable >
abstract_class CPulseCell_BaseRequirementVTable : public TBase
{
public:
	// A one-byte state set to 1 (CPulseCell_LimitCount); null by default
	virtual void *CreateRequirementState() const = 0;
	// The schema class name of the criteria the requirement fills ("...::Criteria_t"); empty in the
	// base. The selectors find the criteria of their outflows by it
	// (IPulseHost_Cursor::GetOutflowCriteria)
	virtual const char *GetCriteriaClassName() const = 0;
	virtual void DestroyRequirementState( void *pState ) const = 0;
};

//-----------------------------------------------------------------------------
// A cell that interpolates over time (CPulseCell_LerpCameraSettings). Methods named by what
// they do.
//-----------------------------------------------------------------------------
template < class TBase = CPulseCell_BaseYieldingInflowVTable<> >
abstract_class CPulseCell_BaseLerpVTable : public TBase
{
public:
	// 0.05, the period between two steps
	virtual float GetUpdateInterval() const = 0;
	// Called by OnCursorWake with the clamped fraction, and with 1 at the end
	virtual int OnLerp( CPulseExecCursor *pCursor, float flFraction ) const = 0;
	// Empty in every cell
	virtual void Stub_26() = 0;
};

//-----------------------------------------------------------------------------
// The cells that wait for the cursors with a tag (CPulseCell_CursorQueue,
// CPulseCell_WaitForCursorsWithTag). Methods named by what they do; 0 / false in the base.
//-----------------------------------------------------------------------------
template < class TBase = CPulseCell_BaseYieldingInflowVTable<> >
abstract_class CPulseCell_WaitForCursorsWithTagBaseVTable : public TBase
{
public:
	// How many other cursors with the tag may run before the cursor waits
	// (m_nCursorsAllowedToRunParallel - 1 for the queue)
	virtual int GetMaxOtherCursorsWithTag() const = 0;
	// Whether a cursor that stops waiting tags itself (m_bTagSelfWhenComplete; always for the queue)
	virtual bool ShouldTagSelfWhenComplete() const = 0;
	// The bInherit of that tag (true for the queue)
	virtual bool ShouldTagBeInherited() const = 0;
};

#endif // PULSE_CELL_H
