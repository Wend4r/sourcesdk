#ifndef ICHOREOEVENTCALLBACK_H
#define ICHOREOEVENTCALLBACK_H

#pragma once

#include "tier0/platform.h"

class CChoreoEvent;
class CChoreoScene;

//-----------------------------------------------------------------------------
// Receives the events of a playing choreo scene. A second base of CSceneEntity; the
// scene file cache takes one when it loads a scene.
//
// Virtual methods only, in the order of the game's table (6 slots on both platforms,
// no destructor).
//-----------------------------------------------------------------------------
abstract_class IChoreoEventCallback
{
public:
	// The scene reached the start of an event
	virtual void StartEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// The scene reached the end of an event
	virtual void EndEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// Called while an event runs
	virtual void ProcessEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// The scene's think asks these before it starts / stops an event and skips the event while
	// they return false. CSceneEntity answers from its list of requirement entries: an event that
	// is a requirement by the flag of its entry, another event's stop by the unflagged entries
	// that start no later than it (the events of type CChoreoEvent_BaseGlobalEvent never wait).
	// Named by what they do
	virtual bool CanStartEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	virtual bool CanStopEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
	// Whether an event that waits on its actor is complete; CSceneEntity finds the actor ("unable
	// to find actor") and asks its choreo component
	virtual bool CheckEvent( float flCurrentTime, CChoreoScene *pScene, CChoreoEvent *pEvent ) = 0;
};

#endif // ICHOREOEVENTCALLBACK_H
