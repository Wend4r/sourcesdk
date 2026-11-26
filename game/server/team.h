#ifndef TEAM_H
#define TEAM_H

#pragma once

#include "baseentity.h"

class CCheckTransmitInfo;

//-----------------------------------------------------------------------------
// CTeam adds 2 methods to CBaseEntity: Linux slots 239-240, Windows 238-239. The client
// C_Team adds none: both are server-only.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CTeamVTable : public TBase
{
public:
	// CBaseEntity::ShouldTransmit asks the team of the receiving client when the entity has no transmit flags; returns transmit flags: 4 when a flag of pInfo is set and the entity is the one pInfo names, 16 otherwise; CCSTeam always transmits to spectators and to its own team.
	virtual int ShouldTransmitToPlayer( const CCheckTransmitInfo *pInfo, CBaseEntity *pEntity ) = 0;
	// Clears m_iScore; CCSTeam also clears m_bSurrendered and the half and overtime scores.
	virtual void ResetScores() = 0;
};

#endif // TEAM_H
