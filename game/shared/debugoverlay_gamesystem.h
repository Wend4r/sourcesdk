//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The debug overlay game system: owns the game's debug overlay list,
//          the debug overlay windows and their settings.
//
//=============================================================================//

#ifndef DEBUGOVERLAY_GAMESYSTEM_H
#define DEBUGOVERLAY_GAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "entityhandle.h"
#include "engine/idebugvisualizer.h"
#include "engine2/iinputhandler.h"
#include "scenesystem/isceneviewdebugoverlays.h"

#include <kv3lib/keyvalues3.h>
#include <mathlib/vector4d.h>
#include <tier0/bufferstring.h>
#include <tier0/eventdispatcher.h>
#include <tier0/threadtools.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <Color.h>

#include "schemacompiler/schemaon.h"

class CDebugOverlayWindow;

//-----------------------------------------------------------------------------
// One drawn overlay kept for the overlay history, 120 bytes (a schema class of the client; the client
// declares m_hEntity as CHandle< C_BaseEntity >)
//-----------------------------------------------------------------------------
schema class CDebugDrawHistoryData
{
public:
	CEntityHandle m_hEntity;
	ESceneViewDebugOverlaysListenerDataType_t m_etype;
	CUtlLeanVector< Vector4D > m_vectors;
	CUtlLeanVector< Color > m_colors;
	CUtlLeanVector< float > m_dimensions;
	CUtlLeanVector< double > m_times;
	CUtlLeanVector< uint64 > m_uint64s;
	CUtlLeanVector< bool > m_bools;
	CUtlLeanVector< CUtlString > m_strings;
};

//-----------------------------------------------------------------------------
// Reallocating factory, 4832 bytes on both platforms. Registered as the "GameDebugVisualizer"
// debug visualizer and as the
// "CDebugOverlayGameSystem" input handler (priority 2500) on GameActivate. It adds no virtual
// methods to IGameSystem; on Linux the overrides of the secondary bases repeat in the main table
// (slots 66-72, in declaration order). No source names the members; they are named by what the
// code does with them.
//-----------------------------------------------------------------------------
class CDebugOverlayGameSystem : public CAutoGameSystem, public IDebugVisualizer, public IInputHandler, public ISceneViewDebugOverlaysListener
{
public:
	DECLARE_GAME_SYSTEM();

	// One entry of m_StringLists, 56 bytes
	struct StringList_t
	{
		// A 16-byte object with an empty destructor; not identified (no server code fills the list)
		uint8 m_Key[ 16 ];
		CBufferString m_sName;
		CUtlVector< CUtlString > m_Strings;
	};

	CDebugOverlayGameSystem();
	virtual ~CDebugOverlayGameSystem();

	// Creates the "Server Tick" overlay list (m_pOverlays), registers the visualizer, the input handler
	// and the overlay listener, creates m_pUIContext ("debugoverlay")
	GS_EVENT( GameActivate );
	// Undoes GameActivate (also destroys m_hInputWindow, m_pSceneObject and m_pSecondaryOverlays)
	GS_EVENT( GameDeactivate );
	// Empty
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupUnload );
	// OnPreRender of every window, then FlushDrawHistory
	GS_EVENT( ClientPreRender );
	// FlushDrawHistory
	GS_EVENT( ClientPreRenderAlt );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	// IDebugVisualizer: forwarded to m_pOverlays and m_pSecondaryOverlays
	virtual void SetOverlayStartTime( float flOverlayStartTime, bool bInterpolated ) override;
	// Only while m_bVisible
	virtual void DebugDraw( CFrustum *pFrustum, ISceneView *pView, float flRenderTime, float flTimeStep ) override;
	virtual void AgeList( float flRenderTime, float flTimeStep ) override;
	// Forwarded to m_pSecondaryOverlays
	virtual void SetTimeDelta( double flTimeDelta ) override;
	// m_nActiveCount != 0
	virtual bool IsAnyVisualizerActive() override;

	// Empty
	virtual void OnDebugOverlayData( const void *pData ) override;

	// Runs the "debugoverlay_*" key bindings and the overlay UI's mouse and keyboard input
	virtual bool HandleInputEvent( const InputEvent_t &event, CSplitScreenSlot nSlot ) override;

public:
	// The event dispatcher the system registered its OnServerAdvanceTick / OnServerEndSimulate / ...
	// listeners with; the destructor unregisters them
	CEventDispatcher< CEventIDManager_Default > *m_pEventDispatcher;
	// The platform window the overlay UI sets the cursor of (input system slot 18); destroyed on
	// GameDeactivate
	void *m_hInputWindow;
	// Cleared by the constructor; no server code reads it
	bool m_bClearedOnConstruction;
	// Cleared by the constructor; no server code reads it
	int m_nClearedOnConstruction;
	// IsAnyVisualizerActive returns m_nActiveCount != 0
	int m_nActiveCount;
	// While non-zero the input handler consumes key and mouse events (1 after construction)
	int m_nCaptureInput;
	// The game's overlay list ("Server Tick"); eiface's GetDebugOverlayGameSystem returns it
	ISceneViewDebugOverlays *m_pOverlays;
	// Cleared on GameActivate
	void *m_pClearedOnActivate;
	// A second overlay list the visualizer forwards to when set
	ISceneViewDebugOverlays *m_pSecondaryOverlays;
	// A scene object of the overlay UI, removed from its layer and destroyed on GameDeactivate
	void *m_pSceneObject;
	// Overlays recorded for the history, handed to the renderer (or freed) by FlushDrawHistory
	CUtlVector< CDebugDrawHistoryData * > m_PendingDrawHistory;
	CThreadFastMutex m_PendingDrawHistoryMutex;
	// Every registered CDebugOverlayWindow (added on their GameActivate)
	CUtlVector< CDebugOverlayWindow * > m_Windows;
	// Cleared by the constructor; no server code reads it
	int m_nClearedOnConstruction2;
	bool m_bFalseOnConstruction;
	bool m_bTrueOnConstruction;
	// Freed by the destructor only
	CUtlLeanVector< StringList_t > m_StringLists;
	// The overlay settings; the windows keep their KeyValues3 in it, by name
	CKV3Arena m_Settings;
	// The UI header status text ("imgui_set_status_text")
	CUtlString m_sStatusText;
	// The UI context made on GameActivate (UI system slot 27, "debugoverlay"), released on GameDeactivate
	void *m_pUIContext;
	// Toggled by "debugoverlay_toggle"; DebugDraw draws only while set (true after construction)
	bool m_bVisible;
	// The bInterpolated argument of the last SetOverlayStartTime
	bool m_bInterpolated;
};

#include "schemacompiler/schemaoff.h"

#endif // DEBUGOVERLAY_GAMESYSTEM_H
