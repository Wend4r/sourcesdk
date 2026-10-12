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
#include <tier0/wchartypes.h>

class IRenderContext;
class CTextLayoutInfo;
struct RenderViewport_t;

abstract_class ITextLayout : public IAppSystem
{
public:
	virtual Vector2D LayoutText( void *pLayout, const char *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;
	virtual Vector2D LayoutText( void *pLayout, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;
	virtual Vector2D LayoutText( void *pLayout, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;

	// Overwrites the color of every glyph in pLayout
	virtual void UpdateTextRenderGeometryColor( void *pLayout, Color color ) = 0;

	virtual Vector2D SetupTextLayoutRenderGeometry( void *pLayout, CTextLayoutInfo *pTextLayoutInfo, const RenderViewport_t *pViewport, float flProportionalBaseHeight ) = 0;

	// Renders pLayout through the material system utils
	virtual bool RenderTextGeometry( IRenderContext *pRenderContext, const void *pLayout, const Vector2D &vecOffset, int n ) = 0;

	virtual void GetBounds( IRenderContext *pRenderContext, CTextLayoutInfo *pTextLayoutInfo, Vector2D *pMins, Vector2D *pMaxs ) = 0;
	virtual void GetWBounds( CTextLayoutInfo *pTextLayoutInfo, Vector2D *pMins, Vector2D *pMaxs ) = 0;

	// Size of the text in pTextLayoutInfo
	virtual Vector2D GetWSize( CTextLayoutInfo *pTextLayoutInfo ) = 0;

	virtual ~ITextLayout() {}
};

#endif // ITEXTLAYOUT_H
