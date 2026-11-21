//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The camera path node graph and its editor (camera_path_edit_mode,
//          camera_path_load / _save / _add / _delete / _clear_all).
//
//=============================================================================//

#ifndef CAMPATHFINDGAMESYSTEM_H
#define CAMPATHFINDGAMESYSTEM_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"

#include <mathlib/vector.h>
#include <tier0/utlvector.h>

//-----------------------------------------------------------------------------
// Registered as "CamPathfindGameSystem" (static factory). No source names its members or the
// nested types; they are named by what the code does with them. It adds no virtual methods to
// IGameSystem.
//-----------------------------------------------------------------------------
class CCamPathfindGameSystem : public CAutoGameSystem
{
public:
	DECLARE_GAME_SYSTEM();

	// A link of a node: the index of the node it leads to and a float the drawing code does not read
	struct CamPathLink_t
	{
		int m_nTargetNode;
		float m_flCost;
	};

	// A node of the graph (40 bytes)
	struct CamPathNode_t
	{
		// The node's index; a link is drawn from the node whose index is lower
		int m_nIndex;
		Vector m_vecPosition;
		CUtlVector< CamPathLink_t > m_Links;
	};

	// The graph, embedded at 0x18
	struct CamPathGraph_t
	{
		// 2 while the editor draws the graph; GameActivate resets it to 0
		int m_nState;
		CUtlVector< CamPathNode_t > m_Nodes;
		// Zeroed by the constructor, read nowhere in the server
		int m_nZeroedAtConstruction;
		uint16 m_nZeroedAtConstruction2;
	};

	CCamPathfindGameSystem();
	virtual ~CCamPathfindGameSystem();

	// Resets m_Graph.m_nState
	GS_EVENT( GameActivate );
	// Run the editor; the client one also draws the graph (debug overlay) while
	// camera_path_edit_mode is on
	GS_EVENT( ServerPostSimulate );
	GS_EVENT( ClientPostSimulate );

	// Forward to the game system factory
	virtual void SetGameSystemGlobalPtrs( void *pValue ) override;
	virtual bool DoesGameSystemReallocate() override;

public:
	CamPathGraph_t m_Graph;
};

#endif // CAMPATHFINDGAMESYSTEM_H
