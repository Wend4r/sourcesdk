#ifndef IFONTMANAGER_H
#define IFONTMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcesystem/resourcehandletypes.h>

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier0/wchartypes.h>

typedef uint32 HFont;

template < class T > class CWeakHandle;
struct FontAlias_t;
struct FontAliasList_t;

enum FontDrawType_t : int;

//-----------------------------------------------------------------------------
// Font manager of the material system. Every slot has the s&box DWARF name of
// IFontManager at the same index; the CS2 bodies match (font alias groups,
// "%s%s-%s" range keys, "<Unknown font>").
//-----------------------------------------------------------------------------
abstract_class IFontManager : public IAppSystem
{
public:
	virtual HFont CreateFont() = 0;

	// Clears the glyph data of every font alias group
	virtual void ClearAllFonts() = 0;

	// Copied into a 64 char buffer
	virtual void SetLanguage( const char *pLanguage ) = 0;
	virtual const char *GetLanguage() = 0;

	virtual bool AddCustomFontFile( const char *pFontName, const char *pFontFileName, int nMinChar, int nMaxChar, const char *pTagName ) = 0;

	// Font alias list by tag token, the first list for 0
	virtual FontAliasList_t *FindFontAliasList( CUtlStringToken tagName ) = 0;
	virtual void RemoveFontAliasList( const char *pTagName ) = 0;

	// Font aliases of a tag, 32 bytes each
	virtual FontAlias_t *GetFontAliases( const char *pTagName ) = 0;

	virtual HFont CreateFontAlias( const char *pFontName, const char *pTagName, bool bIsProportional ) = 0;
	virtual HFont GetFont( const char *pFontName, const char *pTagName, bool bIsProportional ) = 0;
	virtual HFont LoadFont( const char *pFontName, const char *pWindowsFontName, int nTall, int nFlags, bool bIsProportional, int nWeight, int nBlur, int nScanlines, int nRangeMin, int nRangeMax, const char *pTagName ) = 0;
	virtual HFont FindFont( const char *pFontName, bool bIsProportional, const char *pTagName = NULL ) = 0;
	virtual HFont FindOrLoadFont( const char *pFontName, const char *pWindowsFontName, int nTall, int nFlags, bool bIsProportional, int nWeight, int nBlur, int nScanlines, int nRangeMin, int nRangeMax, const char *pTagName ) = 0;
	virtual const char *GetTrueFontName( const FontAlias_t &fontAlias ) = 0;
	virtual bool GetFontRange( const char *pFontName, int &nMin, int &nMax, const char *pTagName ) = 0;
	virtual void SetFontRange( const char *pFontName, int nMin, int nMax, const char *pTagName ) = 0;

	virtual bool SetFontGlyphSet( HFont hFont, const char *pWindowsFontName, int nTall, int nWeight, int nBlur, int nScanlines, int nFlags, int nRangeMin, int nRangeMax ) = 0;

	// "<Unknown font>" for an invalid font
	virtual const char *GetFontName( HFont hFont ) = 0;
	virtual int GetFontTall( HFont hFont ) = 0;
	virtual int GetCharacterWidth( HFont hFont, uchar32 ch ) = 0;
	virtual void GetKernedCharWidth( HFont hFont, uchar32 ch, uchar32 chBefore, uchar32 chAfter, float &flWide, float &flAbcA, float &flAbcC ) = 0;
	virtual void GetCharABCwide( HFont hFont, uchar32 ch, int &a, int &b, int &c ) = 0;
	virtual void GetTextSize( HFont hFont, const uchar32 *pText, int &nWide, int &nTall ) = 0;

	virtual CWeakHandle< InfoForResourceTypeCTextureBase > GetTextureHandle( int nTextureId ) = 0;
	virtual int GetFontAscent( HFont hFont, uchar32 ch ) = 0;
	virtual bool GetFontUnderlined( HFont hFont ) = 0;

	// FONTFLAG_ADDITIVE is set
	virtual bool IsFontAdditive( HFont hFont ) = 0;

	virtual void PrecacheFontCharacters( HFont hFont, const uchar32 *pCharacters ) = 0;

	// Both run on the glyph cache under the font manager lock
	virtual void ClearFontTextureCache() = 0;
	virtual bool GetTextureForChar( HFont hFont, FontDrawType_t nType, uchar32 ch, int *pTextureId, float **ppTexCoords ) = 0;

	// Empty in CS2
	virtual void ClearTemporaryFontCache() = 0;

	// Prints every font of a font alias group (s&box has an extra float argument, CS2 does not)
	virtual void SpewFonts( const char *pTagName ) = 0;

	virtual ~IFontManager() {}
};

#endif // IFONTMANAGER_H
