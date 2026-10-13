#ifndef ITEXTLAYOUT_H
#define ITEXTLAYOUT_H

#ifdef _WIN32
	#pragma once
#endif

#include "ifontmanager.h"

#include <appframework/iappsystem.h>
#include <mathlib/mathlib.h>
#include <mathlib/vector.h>
#include <mathlib/vector2d.h>

#include <color.h>
#include <tier0/platform.h>
#include <tier0/tier0_unicode.h>
#include <tier0/utlvector.h>
#include <tier0/wchartypes.h>

class IRenderContext;
class CRenderAttributes;
struct RenderViewport_t;

enum LayoutPositionType_e : uint32
{
	LAYOUTPOSITIONTYPE_VIEWPORT_RELATIVE = 0,
	LAYOUTPOSITIONTYPE_FRACTIONAL,
	LAYOUTPOSITIONTYPE_NONE,
};

enum HorizJustification_e : uint32
{
	HORIZ_JUSTIFICATION_LEFT = 0,
	HORIZ_JUSTIFICATION_CENTER,
	HORIZ_JUSTIFICATION_RIGHT,
	HORIZ_JUSTIFICATION_NONE,
};

enum VertJustification_e : uint32
{
	VERT_JUSTIFICATION_TOP = 0,
	VERT_JUSTIFICATION_CENTER,
	VERT_JUSTIFICATION_BOTTOM,
	VERT_JUSTIFICATION_NONE,
};

//-----------------------------------------------------------------------------
// The quad of one glyph
//-----------------------------------------------------------------------------
struct CCharRenderGeometry
{
	Vector m_Positions[ 4 ];
	Vector2D m_TextureCoords[ 4 ];
	Color m_VertexColors[ 4 ];

	// Font texture of the glyph, see IFontManager::GetTextureHandle()
	int m_FontTextureID;
};

//-----------------------------------------------------------------------------
// Text and its layout parameters
//-----------------------------------------------------------------------------
class CTextLayoutInfo
{
public:
	Vector2D m_TextPos;
	LayoutPositionType_e m_PositionType;
	HorizJustification_e m_viewportHorizJust;
	VertJustification_e m_viewportVertJust;

	// The default font when 0
	HFont m_FontID;

	bool m_bHorizGradient;
	Color m_TopColor;
	Color m_BottomColor;
	bool m_bAllCaps;
	HorizJustification_e m_HorizJust;
	bool m_bIsProportional;
	float m_flFontScaleFactor;
	matrix3x4_t m_RenderToScreen;
	CStrAutoEncode m_text;
};

abstract_class ITextLayout : public IAppSystem
{
public:
	virtual Vector2D LayoutText( CUtlVector< CUtlVector< CCharRenderGeometry > > *pGeometry, const char *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;
	virtual Vector2D LayoutText( CUtlVector< CUtlVector< CCharRenderGeometry > > *pGeometry, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;
	virtual Vector2D LayoutText( CUtlVector< CUtlVector< CCharRenderGeometry > > *pGeometry, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;

	// Overwrites the color of every glyph in pGeometry
	virtual void UpdateTextRenderGeometryColor( CUtlVector< CUtlVector< CCharRenderGeometry > > *pGeometry, Color color ) = 0;

	virtual Vector2D SetupTextLayoutRenderGeometry( CUtlVector< CUtlVector< CCharRenderGeometry > > *pGeometry, CTextLayoutInfo *pTextLayoutInfo, const RenderViewport_t *pViewport, float flProportionalBaseHeight ) = 0;

	// Renders the geometry through the material system utils
	virtual bool RenderTextGeometry( IRenderContext *pRenderContext, CUtlVector< CUtlVector< CCharRenderGeometry > > &geometry, const Vector2D &vecOffset, const CRenderAttributes *pAttributes ) = 0;

	virtual void GetBounds( IRenderContext *pRenderContext, CTextLayoutInfo *pTextLayoutInfo, Vector2D *pMins, Vector2D *pMaxs ) = 0;
	virtual void GetWBounds( CTextLayoutInfo *pTextLayoutInfo, Vector2D *pMins, Vector2D *pMaxs ) = 0;

	// Size of the text in pTextLayoutInfo
	virtual Vector2D GetWSize( CTextLayoutInfo *pTextLayoutInfo ) = 0;

	virtual ~ITextLayout() {}
};

#endif // ITEXTLAYOUT_H
