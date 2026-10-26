//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Text layout and font services used by the panorama UI engine
//
//===========================================================================//

#ifndef IUITEXTSERVICES_H
#define IUITEXTSERVICES_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "appframework/iappsystem.h"

namespace panorama
{

class IUITextLayout;
class IUITextTextureCache;
class IUITextLayoutDrawCache;
class IUITextTextureProvider;
class IUITextTextureStorage;

// Layout parameters of IUITextServices::CreateTextLayout(); the int8 style members are -1 when the property is unset
struct UITextLayoutParams_t
{
private:
	// Not read by the text services
	uint8 m_pad0[ 8 ];

public:
	// Font family list; "Arial" is appended as the last fallback
	const char *m_pszFontFamily;
	float m_flFontSize;
	// Pixels for line height mode 1, percent of the font size for mode 2
	float m_flLineHeight;
	// 0 takes the line height from the font metrics
	int m_nLineHeightMode;
	// Pango weight 500, 700, 900, 100, 300 and 600 for 1 to 6, 400 otherwise
	int8 m_nFontWeight;
	// 2 is italic
	int8 m_nFontStyle;
	// 1 is condensed, 2 expanded
	int8 m_nFontStretch;
	// 1 center, 2 right, 3 justify, 4 spreads the letters over m_flMaxWidth without wrapping
	int8 m_nTextAlign;
	bool m_bWrap;
	bool m_bEllipsis;

private:
	uint8 m_pad22[ 2 ];

public:
	float m_flLetterSpacing;
	// Font size of the empty lines between paragraphs ( "\n\n" ); FLT_MIN when unset
	float m_flParagraphBreakFontSize;
	// FLT_MIN when unset; a negative value, like a negative font size, fails the layout
	float m_flMaxWidth;
	float m_flMaxHeight;
};

// Font metrics IUITextServices::CreateTextLayout() fills in pixels
struct UITextFontMetrics_t
{
	float m_flAscent;
	float m_flDescent;
	float m_flLineHeight;
	float m_flMaxAdvanceWidth;
	bool m_bFixedWidth;
	// Set when the face metrics were read; m_bFixedWidth and m_flMaxAdvanceWidth are only valid then
	bool m_bFaceMetricsRead;
};

abstract_class IUITextServices : public IAppSystem
{
public:
	virtual ~IUITextServices() {}

	virtual void InitializeTextLayout() = 0;
	virtual void ShutdownTextLayout() = 0;

	virtual bool RegisterCustomFontPath( const char *pszSearchPath, const char *pszPath ) = 0;

	virtual bool RegisterCustomFontFile( const char *pszFullPath, const char *pszFileName ) = 0;

	// nTextBytes includes the terminator, nEncoding is 1 for UTF-8, 2 for UTF-16 and 3 for UTF-32, so pText has no single type.
	// pFontMetrics may be NULL
	virtual IUITextLayout *CreateTextLayout( const void *pText, int nTextBytes, int nCharCount, uint8 nEncoding, const UITextLayoutParams_t *pLayoutParams, UITextFontMetrics_t *pFontMetrics ) = 0;
	virtual void FreeTextLayout( IUITextLayout *pLayout ) = 0;

	virtual const CUtlVector< CUtlString > &GetSortedValidFontNames() = 0;

	virtual IUITextTextureCache *CreateTextTextureCache( IUITextTextureProvider *pProvider ) = 0;
	virtual void DestroyTextTextureCache( IUITextTextureCache *pCache ) = 0;

	virtual IUITextLayoutDrawCache *CreateTextLayoutDrawCache( IUITextTextureStorage *pStorage ) = 0;
	virtual void DestroyTextLayoutDrawCache( IUITextLayoutDrawCache *pCache ) = 0;

	virtual void SetJapaneseLineBreaking( bool bEnabled ) = 0;
};

} // namespace panorama

#endif // IUITEXTSERVICES_H
