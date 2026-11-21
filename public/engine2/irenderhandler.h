//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A handler the render service calls each frame (IRenderService::InstallRenderHandler).
//
//=============================================================================//

#ifndef IRENDERHANDLER_H
#define IRENDERHANDLER_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "iloopmode.h"

struct RenderViewport_t;

//-----------------------------------------------------------------------------
// Three slots (CRenderGameSystem of server and client, CCsgoClientUI,
// CCSItemIconGeneratorGameSystem), no destructor. The two besides AddViewsToSceneSystem are
// named by what CRenderService does with them.
//-----------------------------------------------------------------------------
abstract_class IRenderHandler
{
public:
	// Called by the render service's AsyncAddViewsToSceneSystem job for the handlers whose
	// ShouldAddViewsToSceneSystem returns true; returns the back colour buffer to pass on
	virtual RHBackColorBuffer_t AddViewsToSceneSystem( const EngineLoopState_t &state, double flRenderTime, double flRealTime,
		const RenderViewport_t &viewport, const RHBackColorBuffer_t &backColorBuffer ) = 0;

	// Asked before AddViewsToSceneSystem; the default returns false
	virtual bool ShouldAddViewsToSceneSystem() { return false; }

	// Called for every handler from CRenderService::OnClientOutput; empty by default
	virtual void OnClientOutput() {}
};

#endif // IRENDERHANDLER_H
