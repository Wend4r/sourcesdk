#ifndef ITEXTLAYOUT_H
#define ITEXTLAYOUT_H

#ifdef _WIN32
	#pragma once
#endif

#include "ifontmanager.h"

#include <appframework/iappsystem.h>
#include <mathlib/vector2d.h>

#include <color.h>
#include <tier0/platform.h>
#include <tier0/utlvector.h>
#include <tier0/wchartypes.h>

class IRenderContext;
class CRenderAttributes;
class CTextLayoutInfo;
struct CCharRenderGeometry;
struct RenderViewport_t;

//-----------------------------------------------------------------------------
// Text layout of the material system. Every slot has the s&box DWARF name of
// ITextLayout at the same index; the CS2 bodies match.
//-----------------------------------------------------------------------------
abstract_class ITextLayout : public IAppSystem
{
public:
	typedef CUtlVector< CUtlVector< CCharRenderGeometry > > RenderGeometryLists_t;

	virtual Vector2D SetupPrintTextRenderGeometry( RenderGeometryLists_t *pRenderGeometryLists, const char *pText, const Vector2D &vecPos, Color drawColor, HFont hFont ) = 0;
	virtual Vector2D SetupPrintTextRenderGeometry( RenderGeometryLists_t *pRenderGeometryLists, const uchar16 *pText, const Vector2D &vecPos, Color drawColor, HFont hFont ) = 0;
	virtual Vector2D SetupPrintTextRenderGeometry( RenderGeometryLists_t *pRenderGeometryLists, const uchar32 *pText, const Vector2D &vecPos, Color drawColor, HFont hFont ) = 0;

	// Overwrites the color of every glyph
	virtual void UpdateTextRenderGeometryColor( RenderGeometryLists_t *pRenderGeometryLists, Color color ) = 0;

	virtual Vector2D SetupTextLayoutRenderGeometry( RenderGeometryLists_t *pRenderGeometryLists, CTextLayoutInfo *pTextLayoutInfo, const RenderViewport_t *pViewport, float flProportionalBaseHeight ) = 0;

	// Renders the geometry through the material system utils
	virtual bool RenderTextGeometry( IRenderContext *pRenderContext, RenderGeometryLists_t &renderGeometryLists, const Vector2D &vecOffset, const CRenderAttributes *pAttributes ) = 0;

	virtual void GetBounds( IRenderContext *pRenderContext, CTextLayoutInfo *pTextLayoutInfo, Vector2D *pMins, Vector2D *pMaxs ) = 0;
	virtual void GetWBounds( CTextLayoutInfo *pTextLayoutInfo, Vector2D *pMins, Vector2D *pMaxs ) = 0;

	// Size of the laid out text
	virtual Vector2D GetWSize( CTextLayoutInfo *pTextLayoutInfo ) = 0;

	virtual ~ITextLayout() {}
};

#endif // ITEXTLAYOUT_H
