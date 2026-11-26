#ifndef ILOOPMODEGAMESHAREDSTATE_H
#define ILOOPMODEGAMESHAREDSTATE_H

#pragma once

#include "tier0/platform.h"
#include "iloopmode.h"
#include "mathlib/vector4d.h"

class CUtlString;
class IWorldSession;

//-----------------------------------------------------------------------------
// The state of the game loop mode that the client and the server share. CLoopModeGame
// implements it as its second base (after ILoopMode): on Linux its overrides also take slots
// of the main table, on Windows they stay in this table. Eight slots on both platforms, no
// virtual destructor.
//-----------------------------------------------------------------------------
abstract_class ILoopModeGameSharedState
{
public:
	virtual IWorldSession *GetWorldSession() = 0;
	virtual ClientServerMode_t GetClientServerMode() = 0;
	virtual void ReceivedServerInfo( const GameSessionConfiguration_t &config, ILoopModePrerequisiteRegistry *pRegistry ) = 0;
	virtual bool IsInHeadlessMode() = 0;
	virtual void SetFadeColor( Vector4D vColor ) = 0;
	virtual bool IsBackgroundMap() = 0;
	virtual bool IsLevelTransition() = 0;
	// Copies the names ReceivedServerInfo kept from the session configuration: the previous level
	// (only during a level transition, otherwise pPreviousLevel is left alone) and the map name.
	// Named by what it does
	virtual void GetLevelNames( CUtlString *pPreviousLevel, CUtlString *pMapName ) = 0;
};

#endif // ILOOPMODEGAMESHAREDSTATE_H
