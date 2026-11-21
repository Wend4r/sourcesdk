//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The game system that keeps the scene objects of the game in step with their entities.
//
//=============================================================================//

#ifndef RENDERGAMESYSTEM_H
#define RENDERGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "tier1/utlconcurrentappendvector.h"
#include "tier1/utlmultijobprocessor.h"
#include "engine2/irenderhandler.h"

struct SceneObjectUpdaterHandle_t__;

//-----------------------------------------------------------------------------
// Registered as "RenderGameSystem" (reallocating factory), 384 bytes on both platforms, aligned to
// 128 (the deleting destructor frees it with the alignment). Second bases CUtlMultiJobProcessor
// (+0x10) and IRenderHandler (+0x30 Linux / +0x38 Windows). The updaters, the light styles and
// the secondary scene worlds are globals of rendergamesystem.cpp, not members. The client class
// (512 bytes) has one more own virtual.
//-----------------------------------------------------------------------------
class CRenderGameSystem : public IGameSystem, public CUtlMultiJobProcessor, public IRenderHandler
{
public:
	DECLARE_GAME_SYSTEM();

	// The scene object updater a SceneObjectUpdaterHandle_t points to
	struct SceneObjectUpdater_t;

	// Frees the secondary scene world of every world group
	GS_EVENT( GameShutdown );
	// Resets the change frame of every updater list
	GS_EVENT( GameActivate );
	// Empty
	GS_EVENT( GameDeactivate );
	// Allocates the secondary scene world of the spawn group's world group (AllocateSecondarySceneWorld)
	GS_EVENT( PreSpawnGroupLoad );
	GS_EVENT( PostSpawnGroupUnload );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

	virtual ~CRenderGameSystem();

	// Stores a 4-bit value in the updater's flags and queues it as changed
	virtual void SetForceLOD( SceneObjectUpdaterHandle_t__ *hUpdater, int nLOD );

	// IRenderHandler. Returns IDebugVisualizerMgr::IsAnyVisualizerActive (the client first waits
	// for its pending job)
	virtual void OnClientOutput() override;
	// Returns backColorBuffer unchanged (the server adds no views)
	virtual RHBackColorBuffer_t AddViewsToSceneSystem( const EngineLoopState_t &state, double flRenderTime, double flRealTime,
		const RenderViewport_t &viewport, const RHBackColorBuffer_t &backColorBuffer ) override;

public:
	// uint16, uint16, bool after the DECLARE_GAME_SYSTEM byte
	uint16 m_nLastTickSceneSystemFrameCount;
	uint16 m_nLastFrameSceneSystemFrameCount;
	bool m_bIsSFMRecording;
	// Named by what it does: nesting count of the deferred change phases; while it is above zero
	// changed updaters go to m_DeferredUpdaters instead of the per-world lists
	int m_nDeferredChangeDepth;
	// Named by what it does: the updaters marked changed during a deferred phase (several threads
	// at once); the end of the phase queues them. Preallocated to 1024
	CUtlConcurrentAppendVector< SceneObjectUpdater_t * > m_DeferredUpdaters;
};

#endif // RENDERGAMESYSTEM_H
