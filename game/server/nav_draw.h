//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The debug drawing of the navigation mesh (CNavDraw, CNavDrawArea, CNavDrawMesh
//          implement it on the debug overlay of the game).
//
//=============================================================================//

#ifndef NAV_DRAW_H
#define NAV_DRAW_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "color.h"

class CNavArea;
class CNavMesh;
class CNavPoly;
class Vector;

//-----------------------------------------------------------------------------
// The drawing primitives. Every method draws through the debug overlay when there is one;
// the colour components are bytes packed into one overlay colour. bZTest is passed on to the
// overlay. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class INavDraw
{
public:
	// A line between two points with a Color, a z-test flag and a duration
	virtual void Stub_0() = 0;
	virtual void DrawLine_Internal( const Vector &vecStart, const Vector &vecEnd, int r, int g, int b, int a, float flDuration ) = 0;
	virtual void DrawSphere_Internal( const Vector &vecCenter, float flRadius, int r, int g, int b, int a, bool bZTest, float flDuration ) = 0;
	// An axis-aligned box between two corners
	virtual void DrawBox_Internal( const Vector &vecMins, const Vector &vecMaxs, int r, int g, int b, int a, bool bZTest, float flDuration ) = 0;
	// An oriented box: origin, mins, maxs and angles, then the colour components, z-test and duration
	virtual void Stub_4() = 0;
	virtual void DrawText_Internal( const Vector &vecOrigin, int nLine, float flDuration, int r, int g, int b, int a, const char *pszText ) = 0;
	// Forwards its arguments to a text helper of the overlay
	virtual void Stub_6() = 0;
	// Draws a shape computed by a helper from a point list (up to 30 points), segment by segment
	virtual void Stub_7() = 0;
	// Two points, two floats, the colour components and a flag; does not test for the overlay
	virtual void Stub_8() = 0;
	// Shapes drawn segment by segment from two points and a Color
	virtual void Stub_9() = 0;
	virtual void Stub_10() = 0;
	// Three points and a Color; with the flag set draws the three edges, otherwise a filled shape
	virtual void Stub_11() = 0;
	// Draws around a point with a size and a Color, segment by segment
	virtual void Stub_12() = 0;
	// A cube of half size flSize around a point, with a Color
	virtual void Stub_13() = 0;
	// Forwards its arguments to a helper that draws a computed outline
	virtual void Stub_14() = 0;
};

//-----------------------------------------------------------------------------
// Draws polygons and areas. The z-test mode and the edit colour are enumerations of the
// navlib, passed as int here. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class INavDrawArea
{
public:
	virtual void DrawPolyFilled_Internal( const CNavPoly &poly, Color color, float flDuration, int nZTest, const Vector &vecOffset, float flInset ) const = 0;
	virtual void DrawPolyOutline_Internal( const CNavPoly &poly, Color color, float flDuration, int nZTest, const Vector &vecOffset, float flInset ) const = 0;
	virtual void DrawArea_Internal( const CNavArea &area, float flDuration, int nColor ) const = 0;
	virtual void DrawGetColor_Internal( const CNavArea &area, int *pnColor, bool *pbOverride ) const = 0;
};

//-----------------------------------------------------------------------------
// Draws the whole mesh. No virtual destructor.
//-----------------------------------------------------------------------------
abstract_class INavDrawMesh
{
public:
	virtual void DrawPlayerCounts_Internal( const CNavMesh &mesh ) = 0;
};

#endif // NAV_DRAW_H
