#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/ifontmanager.h"
#include "materialsystem2/fontmanager/fontamalgam.h"
#include <tier0/threadtools.h>
#include <tier0/utlmap.h>
#include <tier0/utlstring.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>
#include <tier2/tier2.h>

struct SFontDescription;

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
		void *m_pData;
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

	char m_szLanguage[ 64 ];

	// CFontTextureCache, points back to the font manager
	uint8 m_FontTextureCache[ 0x1078 ];

	CUtlVector< FontAliasList_t * > m_pFontAliasLists;
	CAtomicMutex m_FontTextureCacheMutex;
	bool m_bUnk11F4;
};

#endif // FONTMANAGER_H
