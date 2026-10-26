#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/ifontmanager.h"
#include "materialsystem2/fontmanager/fontamalgam.h"
#include <tier0/threadtools.h>
#include <tier0/utlblockvector.h>
#include <tier0/utlmap.h>
#include <tier0/utlrbtree.h>
#include <tier0/utlstring.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>
#include <tier2/tier2.h>

struct SFontDescription;
class CFontManager;

// Packs the glyphs of a font texture page (a 0x30-byte object owned by the page)
class CTexturePacker;

// Passed to the CFontManager constructor; slot 1 releases a page texture by its id
class IFontTextureCacheCallbacks;

//-----------------------------------------------------------------------------
// The glyph textures of the font manager
//-----------------------------------------------------------------------------
class CFontTextureCache
{
public:
	// A glyph of the common chars: the texture page and the texture coordinates
	struct CommonCharEntry_t
	{
		int m_nPage;
		float m_texCoords[ 4 ];
	};

	struct CacheEntry_t
	{
		uint32 m_wch;
		int m_hFont;
		int m_nPage;
		float m_texCoords[ 4 ];
	};

	struct CacheEntryLess_t
	{
		bool operator()( const CacheEntry_t &lhs, const CacheEntry_t &rhs ) const;
	};

	struct Page_t
	{
		// The texture and the outline texture
		int m_nTextureID[ 2 ];
		CTexturePacker *m_pPacker;
	};

	CFontManager *m_pFontManager;
	IFontTextureCacheCallbacks *m_pCallbacks;

	// The glyphs of chars below 0x200, 256 fonts per allocated block
	CommonCharEntry_t *m_pCommonChars[ 512 ];

	// The other glyphs, keyed by char and font
	CUtlRBTree< CacheEntry_t, CacheEntryLess_t, unsigned short, CUtlBlockVector< UtlRBTreeNode_t< CacheEntry_t, unsigned short >, unsigned short > > m_CharCache;
	CUtlBlockVector< Page_t, int > m_PageList;

private:
	// Set to -1 when the cache is cleared, nothing reads it
	int m_unk1068;
	uint8 m_pad106C[ 4 ];

public:
	CUtlString m_unk1070;
};

//-----------------------------------------------------------------------------
// Implementation of IFontManager. Members whose types are not declared in the
// SDK are kept as padding of their size.
//-----------------------------------------------------------------------------
class CFontManager : public CTier2AppSystem< IFontManager >
{
public:
	class CFontAttributes
	{
	public:
		char m_szName[ 64 ];
		int m_tall;
		int m_weight;
		int m_blur;
		int m_scanlines;
		int m_flags;
		bool m_haveDescs;
	};

	class CFontInfo
	{
	public:
		CFontBase *m_font;
		CUtlVector< SFontDescription > m_descs;
	};

	class CFontDataFile
	{
	public:
		uint8 *m_pData;
		int m_nDataBytes;
	};

	struct CustomFontInfo_t
	{
		UtlSymId_t nameSym;
		UtlSymId_t fileSym;
		int nMinChar;
		int nMaxChar;
	};

	CUtlVector< CFontAmalgam > m_FontAmalgams;

	// The fonts created by the platform, released by ClearAllFonts()
	CUtlMap< CFontAttributes, CFontInfo, int > m_PlatformFonts;
	CAtomicMutex m_PlatformFontsMutex;

	CUtlSymbolTable m_FontDataSymbols;

	// Keyed by the file name symbol in m_FontDataSymbols
	CUtlMap< UtlSymId_t, CFontDataFile, int > m_FontDataFiles;

	// Registered by AddCustomFontFile()
	CUtlVector< CustomFontInfo_t > m_CustomFonts;

#ifdef PLATFORM_POSIX
	// FT_Library, created by Init() through FT_Init_FreeType()
	struct FT_LibraryRec_ *m_pFTLibrary;
#endif

	char m_szLanguage[ 64 ];

	CFontTextureCache m_FontTextureCache;

	CUtlVector< FontAliasList_t * > m_pFontAliasLists;
	CAtomicMutex m_FontTextureCacheMutex;
	bool m_bUnk11F4;
};

#endif // FONTMANAGER_H
