#ifndef IFONTMANAGER_H
#define IFONTMANAGER_H

#ifdef _WIN32
	#pragma once
#endif

#include <appframework/iappsystem.h>

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier0/wchartypes.h>

typedef uint32 HFont;

abstract_class IFontManager : public IAppSystem
{
public:
	virtual HFont CreateFont() = 0;

	// Clears the glyph data of every font alias group
	virtual void unk012() = 0;

	// Copied into a 64 char buffer
	virtual void SetLanguage( const char *pLanguage ) = 0;
	virtual const char *GetLanguage() = 0;

	virtual bool AddCustomFontFile( const char *pFontName, const char *pFontFileName, int nRangeMin, int nRangeMax, void *pUnk ) = 0;

	// Font alias group by name token, the first group for 0
	virtual void *FindFontAliasGroup( CUtlStringToken groupName ) = 0;

	// Looks up a font alias group by name
	virtual void unk017( const char *pGroupName ) = 0;

	// Font entries of a font alias group, 32 bytes each
	virtual void *unk018( const char *pGroupName ) = 0;

	// Creates a font and sets it up from the arguments
	virtual HFont unk019( int n1, int n2, bool b3 ) = 0;

	// Font with the given name in a font alias group
	virtual HFont unk020( const char *pFontName, const char *pGroupName, bool bUnk ) = 0;

	virtual HFont unk021( const char *pFontName, const char *pWindowsFontName, int nTall, int n4, bool b5, int nWeight, int n7, int n8, int n9, int n10, void *p11 ) = 0;

	virtual HFont unk022( const char *pFontName, bool bUnk, const char *pGroupName = NULL ) = 0;
	virtual void unk023( void *p ) = 0;

	// Name of a font entry returned by unk018
	virtual const char *unk024( void *pFontEntry ) = 0;

	virtual void unk025( void *p ) = 0;
	virtual void unk026( void *p ) = 0;

	virtual bool SetFontGlyphSet( HFont hFont, const char *pWindowsFontName, int nTall, int nWeight, int nBlur, int nScanlines, int nFlags, int nRangeMin, int nRangeMax ) = 0;

	// "<Unknown font>" for an invalid font
	virtual const char *GetFontName( HFont hFont ) = 0;
	virtual int GetFontTall( HFont hFont ) = 0;
	virtual int GetCharacterWidth( HFont hFont, uchar32 ch ) = 0;
	virtual void GetKernedCharWidth( HFont hFont, uchar32 ch, uchar32 chBefore, uchar32 chAfter, float &flWide, float &flAbcA, float &flAbcC ) = 0;
	virtual void GetCharABCwide( HFont hFont, uchar32 ch, int &a, int &b, int &c ) = 0;
	virtual void GetTextSize( HFont hFont, const uchar32 *pText, int &nWide, int &nTall ) = 0;

	virtual void unk034( void *p ) = 0;

	// Value read from the glyph of ch
	virtual int unk035( HFont hFont, uchar32 ch ) = 0;
	virtual bool unk036( HFont hFont ) = 0;

	// FONTFLAG_ADDITIVE is set
	virtual bool IsFontAdditive( HFont hFont ) = 0;

	virtual void PrecacheFontCharacters( HFont hFont, const uchar32 *pCharacters ) = 0;

	// Both run on the glyph cache under the font manager lock
	virtual void unk039() = 0;
	virtual void unk040( void *p ) = 0;

	// Empty
	virtual void unk041() = 0;

	// Prints every font of a font alias group
	virtual void DumpFontAliasGroup( const char *pGroupName ) = 0;

	virtual ~IFontManager() {}
};

#endif // IFONTMANAGER_H
