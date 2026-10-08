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

abstract_class ITextLayout : public IAppSystem
{
public:
	virtual Vector2D LayoutText( void *pLayout, const char *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;
	virtual Vector2D LayoutText( void *pLayout, const uchar16 *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;
	virtual Vector2D LayoutText( void *pLayout, const uchar32 *pText, const Vector2D &vecPos, Color color, HFont hFont ) = 0;

	// Overwrites the color of every glyph in pLayout
	virtual void unk014( void *pLayout, Color color ) = 0;

	virtual Vector2D unk015( void *pLayout, const void *pTextDesc, void *p, float flMaxWidth ) = 0;

	// Renders pLayout through the material system utils
	virtual bool unk016( IRenderContext *pRenderContext, const void *pLayout, void *p, int n ) = 0;

	virtual void unk017( void *p ) = 0;
	virtual void unk018( void *p ) = 0;

	// Size of the text in pTextDesc
	virtual Vector2D unk019( const void *pTextDesc ) = 0;

	virtual ~ITextLayout() {}
};

#endif // ITEXTLAYOUT_H
