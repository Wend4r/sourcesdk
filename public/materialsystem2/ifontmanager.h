#ifndef IFONTMANAGER_H
#define IFONTMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include <appframework/iappsystem.h>
#include <resourcesystem/resourcehandle.h>

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier0/wchartypes.h>

typedef uint32 HFont;

class InfoForResourceTypeCTextureBase;

struct FontRange_t
{
	int _min;
	int _max;
};

//-----------------------------------------------------------------------------
// A font of a font alias group
//-----------------------------------------------------------------------------
struct FontAlias_t
{
	CUtlSymbolLarge m_FontName;
	CUtlSymbolLarge m_TrueFontName;
	FontRange_t m_FontRange;
	uint32 m_nUnk018;
};

//-----------------------------------------------------------------------------
// A font alias group, found by its tag name or its token
//-----------------------------------------------------------------------------
struct FontAliasList_t
{
	CUtlString m_pTag;
	CUtlStringToken m_nTagToken;
	CUtlVector< FontAlias_t > m_FontAliases;
};

abstract_class IFontManager : public IAppSystem
{
public:
	virtual HFont CreateFont() = 0;

	// Clears the glyph data of every font alias group
	virtual void ClearAllFonts() = 0;

	// Copied into a 64 char buffer
	virtual void SetLanguage( const char *pLanguage ) = 0;
	virtual const char *GetLanguage() = 0;

	virtual bool AddCustomFontFile( const char *pFontName, const char *pFontFileName, int nRangeMin, int nRangeMax, const char *pGroupName ) = 0;

	// Font alias group by name token, the first group for 0
	virtual FontAliasList_t *FindFontAliasGroup( CUtlStringToken groupName ) = 0;

	virtual void RemoveFontAliasList( const char *pGroupName ) = 0;

	virtual CUtlVector< FontAlias_t > *GetFontAliases( const char *pGroupName ) = 0;

	virtual HFont CreateFontAlias( const char *pAliasName, const char *pFontName, bool bIsProportional ) = 0;

	// Font with the given name in a font alias group
	virtual HFont GetFont( const char *pFontName, const char *pGroupName, bool bIsProportional ) = 0;

	virtual HFont LoadFont( const char *pFontName, const char *pWindowsFontName, int nTall, int nFlags, bool bIsProportional, int nWeight, int nBlur, int nScanlines, int nRangeMin, int nRangeMax, const char *pGroupName ) = 0;

	virtual HFont FindFont( const char *pFontName, bool bIsProportional, const char *pGroupName = NULL ) = 0;
	virtual HFont FindOrLoadFont( const char *pFontName, const char *pWindowsFontName, int nTall, int nFlags, bool bIsProportional, int nWeight, int nBlur, int nScanlines, int nRangeMin, int nRangeMax, const char *pGroupName ) = 0;

	// Name of a font entry returned by GetFontAliases
	virtual const char *GetTrueFontName( const FontAlias_t &fontAlias ) = 0;

	virtual bool GetFontRange( const char *pFontName, int &nRangeMin, int &nRangeMax, const char *pGroupName ) = 0;
	virtual void SetFontRange( const char *pFontName, int nRangeMin, int nRangeMax, const char *pGroupName ) = 0;

	virtual bool SetFontGlyphSet( HFont hFont, const char *pWindowsFontName, int nTall, int nWeight, int nBlur, int nScanlines, int nFlags, int nRangeMin, int nRangeMax ) = 0;

	// "<Unknown font>" for an invalid font
	virtual const char *GetFontName( HFont hFont ) = 0;
	virtual int GetFontTall( HFont hFont ) = 0;
	virtual int GetCharacterWidth( HFont hFont, uchar32 ch ) = 0;
	virtual void GetKernedCharWidth( HFont hFont, uchar32 ch, uchar32 chBefore, uchar32 chAfter, float &flWide, float &flAbcA, float &flAbcC ) = 0;
	virtual void GetCharABCwide( HFont hFont, uchar32 ch, int &a, int &b, int &c ) = 0;
	virtual void GetTextSize( HFont hFont, const uchar32 *pText, int &nWide, int &nTall ) = 0;

	virtual CWeakHandle< InfoForResourceTypeCTextureBase > GetTextureHandle( int nTextureId ) = 0;

	// Value read from the glyph of ch
	virtual int GetFontAscent( HFont hFont, uchar32 ch ) = 0;
	virtual bool GetFontUnderlined( HFont hFont ) = 0;

	// FONTFLAG_ADDITIVE is set
	virtual bool IsFontAdditive( HFont hFont ) = 0;

	virtual void PrecacheFontCharacters( HFont hFont, const uchar32 *pCharacters ) = 0;

	// Both run on the glyph cache under the font manager lock
	virtual void ClearFontTextureCache() = 0;
	virtual bool GetTextureForChar( HFont hFont, int nDrawType, uchar32 ch, int *pTextureId, float **ppTexCoords ) = 0;

	// Empty
	virtual void ClearTemporaryFontCache() = 0;

	// Prints every font of a font alias group
	virtual void DumpFontAliasGroup( const char *pGroupName ) = 0;

	virtual ~IFontManager() {}
};

#endif // IFONTMANAGER_H
