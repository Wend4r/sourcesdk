//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Registry of named debug visualizers owned by the engine service manager
//
//=============================================================================//

#ifndef IDEBUGVISUALIZERMGR_H
#define IDEBUGVISUALIZERMGR_H
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/globalsymbol.h>

class IDebugVisualizer;

class CFrustum;
class ISceneView;

// Returned by IEngineServiceMgr::GetDebugVisualizerMgr. Every method takes the registry lock.
//
// AMNOTE: Names come from CDebugVisualizerMgr of s&box (libengine2.so DWARF) and Dota 2 2018 (libengine2.dylib
// symbols); CS2 adds the group argument and IsAnyVisualizerActive.
abstract_class IDebugVisualizerMgr
{
public:
	virtual int GetVisualizerCount() = 0;

	// nGroup selects the visualizers that SetOverlayStartTime forwards to.
	virtual void AddVisualizer( const CGlobalSymbol &name, IDebugVisualizer *pVisualizer, int nGroup ) = 0;

	// Returns the removed visualizer, or NULL. The caller destroys it.
	virtual IDebugVisualizer *RemoveVisualizer( const CGlobalSymbol &name ) = 0;

	virtual IDebugVisualizer *FindVisualizer( const CGlobalSymbol &name ) = 0;

	// Forwards the time and the flag to every visualizer added with nGroup.
	virtual void SetOverlayStartTime( int nGroup, float flTime, bool ) = 0;

	// The remaining methods forward to every visualizer.
	virtual void DebugDraw( CFrustum *pFrustum, ISceneView *pView, float, float ) = 0;
	virtual void AgeList( float, float ) = 0;
	// Empty in every visualizer of the engine.
	virtual void SetTimeDelta( double flTimeDelta ) = 0;
	// Named by what it does: true when at least one visualizer reports itself active
	virtual bool IsAnyVisualizerActive() = 0;
};

#endif // IDEBUGVISUALIZERMGR_H
