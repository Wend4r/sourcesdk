//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: A ladder connecting nav areas.
//
//=============================================================================//

#ifndef NAV_LADDER_H
#define NAV_LADDER_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "mathlib/vector.h"
#include "nav_area.h"

class CNavArea;
class CNavMesh;

//-----------------------------------------------------------------------------
// Owned by the mesh's m_Ladders. A path climbing up the ladder may end in any of the top forward,
// left and right areas; one climbing down, in any of the three bottom areas.
//-----------------------------------------------------------------------------
class CNavLadder
{
public:
	float GetLength() const { return m_flLength; }
	uint32 GetID() const { return m_id; }

public:
	Vector m_vTop;
	Vector m_vBottom;
	float m_flLength;
	float m_flWidth;

	CNavArea *m_pTopForwardArea;
	CNavArea *m_pTopLeftArea;
	CNavArea *m_pTopRightArea;
	CNavArea *m_pTopBehindArea;
	CNavArea *m_pBottomArea;

private:
	// Two more bottom areas, read from nav files of version 33 and later and accepted as the end
	// of a downward ladder segment like m_pBottomArea; nothing names them.
	CNavArea *m_pUnk48;
	CNavArea *m_pUnk50;

public:
	// The side of the ladder the climbing surface faces; m_vNormal is derived from it.
	NavDirType m_dir;
	// Points away from the climbing surface.
	Vector m_vNormal;
	uint32 m_id;
	CNavMesh *m_pMesh;
};

#endif // NAV_LADDER_H
