#ifndef CSTRIKE15_CS_TEAM_H
#define CSTRIKE15_CS_TEAM_H

#pragma once

#include "../team.h"

//-----------------------------------------------------------------------------
// CCSTeam adds 1 method to CTeam: Linux slot 241, Windows 240. It also overrides
// ShouldTransmitToPlayer and ResetScores.
//-----------------------------------------------------------------------------
template < class TBase = CTeamVTable<> >
abstract_class CCSTeamVTable : public TBase
{
public:
	// Sets m_numMapVictories with a state change.
	virtual void SetNumMapVictories( int nNumVictories ) = 0;
};

#endif // CSTRIKE15_CS_TEAM_H
