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

#include <vector>

#include "tier0/platform.h"
#include "nav_pathcost.h"

#include "schemacompiler/schemaon.h"

class CCSBot;

//-----------------------------------------------------------------------------
// Adds danger and damaging-area penalties to the floor cost. Adds no virtuals of its own.
//-----------------------------------------------------------------------------
schema abstract_class PathCost : public CNavPathCost
{
public:
	TYPEMETA( MGetKV3ClassDefaults; MHasKV3TransferPolymorphicClassname );
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( PathCost );

	// An extra cost added whenever a step enters the area.
	struct AreaCost_t
	{
		CNavArea *m_pArea;
		float m_flCost;
	};

	// The bot the cost is computed for; null costs skip the bot-specific penalties.
	noschema CCSBot *m_me;
	// The bot's RouteType; with 2 (SAFEST_ROUTE) the area's danger for the bot's team is added.
	noschema int m_route;
	float m_dangerFactor;
	float m_damagingAreasPenaltyCost;
	float m_flAgentMaxClimb;
	// A std::vector in the game too (Clone copies it with std::vector). Named by what it holds.
	noschema std::vector< AreaCost_t > m_extraAreaCosts;
};

#include "schemacompiler/schemaoff.h"

#endif // CS_NAV_PATHCOST_H
