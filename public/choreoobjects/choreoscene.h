//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A choreographed scene (actors, their channels and the events on them).
//
//=============================================================================//

#ifndef CHOREOSCENE_H
#define CHOREOSCENE_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlstring.h"
#include "tier0/logging.h"
#include "mathlib/piecewisecurve.h"
#include "choreoobjects/choreoobjectbase.h"

class CChoreoActor;
class CChoreoChannel;
class CChoreoEvent;
class IChoreoEventCallback;

//-----------------------------------------------------------------------------
// Owns its actors, channels and events. 360 bytes on both platforms.
//-----------------------------------------------------------------------------
abstract_class CChoreoScene : public CChoreoObjectBase
{
public:
	virtual ~CChoreoScene() = 0;

public:
	// Freed by the destructor; nothing else reads or writes it
	CUtlString m_ReservedString;
	CUtlVector< CChoreoEvent * > m_Events;
	CUtlVector< CChoreoActor * > m_Actors;
	CUtlVector< CChoreoChannel * > m_Channels;
	// Named by what it holds: the requirements events; an event linked to one of them does not
	// start while that event is processing
	CUtlVector< CChoreoEvent * > m_RequirementEvents;
	// The pause events (CChoreoEvent_SECTION) the scene stops at
	CUtlVector< CChoreoEvent * > m_PauseEvents;
	float m_flCurrentTime;
	float m_flStartTime;
	float m_flEndTime;
	// Named by what it holds: the time step of the last think
	float m_flThinkTimeStep;
	float m_flEarliestTime;
	float m_flLatestTime;
	int m_nActiveEvents;
	// Cleared by the constructor; nothing else reads or writes it
	int m_nReserved;
	float m_flSoundSystemLatency;
	float m_flLastActiveTime;
	void ( *m_pfnPrint )( const char *pszFormat, ... );
	IChoreoEventCallback *m_pIChoreoEventCallback;
	LoggingChannelID_t m_nLogChannel;
	// The file the scene was loaded from
	CUtlString m_sFileName;
	// "mapname"
	CUtlString m_sMapname;
	CPiecewiseCurve m_SceneRamp;
	// 60 by default
	int m_nSceneFPS;
	// Index into m_PauseEvents of the pause the scene resumes from (-1 for none)
	int m_nLastPauseEvent;
	float m_flPrecomputedStopTime;
	// Only the Windows constructor touches these bytes (it clears the fifth one); nothing reads them
	uint8 m_ReservedBytes[ 12 ];
	// A container whose type is not established: 32 bytes; its elements are 32-byte nodes that
	// own a string at offset 16. Only the constructor and the destructor (which frees those strings
	// and the nodes) touch it
	uint8 m_ReservedContainer[ 32 ];
	// One bit per event type: whether the scene has an event of that type
	uint32 m_bitvecHasEventOfType[ 2 ];
	// Cleared by the constructor (with m_bUseFrameSnap); nothing else reads or writes them
	bool m_bReservedFlags[ 3 ];
	// Whether event times snap to m_nSceneFPS
	bool m_bUseFrameSnap;
	// The constructor clears the first two and sets the third; no other reader or writer found
	bool m_bReservedStates[ 3 ];
};

#endif // CHOREOSCENE_H
