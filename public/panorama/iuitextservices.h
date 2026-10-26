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

// Encoding of the text passed to IUITextServices::CreateTextLayout()
enum UITextEncoding_t : uint8
{
	UITEXT_ENCODING_UTF8 = 1,
	UITEXT_ENCODING_UTF16,
	UITEXT_ENCODING_UTF32,
};

// How UITextLayoutParams_t::m_flLineHeight is read, as panorama parses line-height
enum UITextLineHeightMode_t : int
{
	// "normal": the line height comes from the font metrics
	UITEXT_LINE_HEIGHT_NORMAL = 0,
	UITEXT_LINE_HEIGHT_PIXELS,
	// Percent of the font size
	UITEXT_LINE_HEIGHT_PERCENT,
};

// The font-weight, font-style, font-stretch and text-align values in the order panorama parses them
enum UITextFontWeight_t : int8
{
	UITEXT_FONT_WEIGHT_UNSET = -1,
	UITEXT_FONT_WEIGHT_NORMAL,
	UITEXT_FONT_WEIGHT_MEDIUM,
	UITEXT_FONT_WEIGHT_BOLD,
	UITEXT_FONT_WEIGHT_BLACK,
	UITEXT_FONT_WEIGHT_THIN,
	UITEXT_FONT_WEIGHT_LIGHT,
	UITEXT_FONT_WEIGHT_SEMI_BOLD,
};

enum UITextFontStyle_t : int8
{
	UITEXT_FONT_STYLE_UNSET = -1,
	UITEXT_FONT_STYLE_NORMAL = 0,
	UITEXT_FONT_STYLE_ITALIC = 2,
};

enum UITextFontStretch_t : int8
{
	UITEXT_FONT_STRETCH_UNSET = -1,
	UITEXT_FONT_STRETCH_NORMAL,
	UITEXT_FONT_STRETCH_CONDENSED,
	UITEXT_FONT_STRETCH_EXPANDED,
};

enum UITextAlign_t : int8
{
	UITEXT_ALIGN_UNSET = -1,
	UITEXT_ALIGN_LEFT,
	UITEXT_ALIGN_CENTER,
	UITEXT_ALIGN_RIGHT,
	UITEXT_ALIGN_JUSTIFY,
	// Spreads the letters over m_flMaxWidth without wrapping
	UITEXT_ALIGN_JUSTIFY_LETTER_SPACING,
};

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
	float m_flLineHeight;
	UITextLineHeightMode_t m_nLineHeightMode;
	UITextFontWeight_t m_nFontWeight;
	UITextFontStyle_t m_nFontStyle;
	UITextFontStretch_t m_nFontStretch;
	UITextAlign_t m_nTextAlign;
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

	// nTextBytes includes the terminator; pText has the type of nEncoding. pFontMetrics may be NULL
	virtual IUITextLayout *CreateTextLayout( const void *pText, int nTextBytes, int nCharCount, UITextEncoding_t nEncoding, const UITextLayoutParams_t *pLayoutParams, UITextFontMetrics_t *pFontMetrics ) = 0;
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
