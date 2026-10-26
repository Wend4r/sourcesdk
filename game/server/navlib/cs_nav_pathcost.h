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
#include <utility>
#include <vector>
#include "nav_pathcost.h"

#include "schemacompiler/schemaon.h"

class CCSBot;

// The kind of route a bot asks for when it computes a path.
enum RouteType
{
	FASTEST_ROUTE = 1,
	// Adds the area's danger for the bot's team to the cost.
	SAFEST_ROUTE = 2,
};

//-----------------------------------------------------------------------------
// Adds danger and damaging-area penalties to the floor cost. Adds no virtuals of its own.
//-----------------------------------------------------------------------------
schema abstract_class PathCost : public CNavPathCost
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );

	// The bot the cost is computed for; null costs skip the bot-specific penalties.
	noschema CCSBot *m_me;
	noschema RouteType m_route;
	float m_dangerFactor;
	float m_damagingAreasPenaltyCost;
	float m_flAgentMaxClimb;

private:
	// An area and an extra cost added whenever the step enters that area.
	noschema std::vector< std::pair< CNavArea *, float > > m_ExtraAreaCosts;
};

#include "schemacompiler/schemaoff.h"

#endif // CS_NAV_PATHCOST_H
