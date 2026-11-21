//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A debug visualizer registered with the engine's IDebugVisualizerMgr
//
//=============================================================================//

#ifndef IDEBUGVISUALIZER_H
#define IDEBUGVISUALIZER_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>

class CFrustum;
class ISceneView;

//-----------------------------------------------------------------------------
// The last two are named after the IDebugVisualizerMgr methods that forward to them.
//-----------------------------------------------------------------------------
abstract_class IDebugVisualizer
{
public:
	virtual ~IDebugVisualizer() {}

	virtual void SetOverlayStartTime( float flOverlayStartTime, bool bInterpolated ) = 0;
	virtual void DebugDraw( CFrustum *pFrustum, ISceneView *pView, float flRenderTime, float flTimeStep ) = 0;
	virtual void AgeList( float flRenderTime, float flTimeStep ) = 0;
	virtual void SetTimeDelta( double flTimeDelta ) = 0;
	virtual bool IsAnyVisualizerActive() = 0;
};

#endif // IDEBUGVISUALIZER_H
