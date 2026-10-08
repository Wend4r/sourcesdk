//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: The cost function the Counter-Strike bots path with.
//
//=============================================================================//

#ifndef CS_NAV_PATHCOST_H
#define CS_NAV_PATHCOST_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "nav_pathcost.h"

class CCSBot;

//-----------------------------------------------------------------------------
// Adds danger and damaging-area penalties to the floor cost. Adds no virtuals of its own.
//-----------------------------------------------------------------------------
schema abstract_class PathCost : public CNavPathCost
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );

	// The bot the cost is computed for; null costs skip the bot-specific penalties.
	noschema CCSBot *m_me;
	// The bot's RouteType; with 2 (SAFEST_ROUTE) the area's danger for the bot's team is added.
	noschema int m_route;
	float m_dangerFactor;
	float m_damagingAreasPenaltyCost;
	float m_flAgentMaxClimb;

private:
	// Begin, end and capacity pointers of 16-byte entries: an area and an extra cost added
	// whenever the step enters that area.
	noschema uint8 m_Unk48[ 0x18 ];
};

#endif // CS_NAV_PATHCOST_H
