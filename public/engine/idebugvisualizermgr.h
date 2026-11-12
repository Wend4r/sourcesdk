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

// Returned by IEngineServiceMgr::GetDebugVisualizerMgr. Every method takes the registry lock.
abstract_class IDebugVisualizerMgr
{
public:
	virtual int GetVisualizerCount() = 0;

	// nGroup selects the visualizers that unk004 forwards to.
	virtual void AddVisualizer( const CGlobalSymbol &name, IDebugVisualizer *pVisualizer, int nGroup ) = 0;

	// Returns the removed visualizer, or NULL. The caller destroys it.
	virtual IDebugVisualizer *RemoveVisualizer( const CGlobalSymbol &name ) = 0;

	virtual IDebugVisualizer *FindVisualizer( const CGlobalSymbol &name ) = 0;

	// Forwards the time and the flag to every visualizer added with nGroup.
	virtual void unk004( int nGroup, float flTime, bool ) = 0;

	// The remaining methods forward to every visualizer; their arguments are not recovered.
	virtual void unk005() = 0;
	virtual void unk006() = 0;
	// Empty in every visualizer of the engine.
	virtual void unk007() = 0;
	// True when the forwarded call returns true for at least one visualizer.
	virtual bool unk008() = 0;
};

#endif // IDEBUGVISUALIZERMGR_H
