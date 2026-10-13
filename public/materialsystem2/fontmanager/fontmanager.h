#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "materialsystem2/ifontmanager.h"
#include <tier0/threadtools.h>
#include <tier0/utlstring.h>
#include <tier0/utlsymbol.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlvector.h>
#include <tier2/tier2.h>

//-----------------------------------------------------------------------------
// Implementation of IFontManager. Containers whose element types are not
// declared in the SDK are kept as padding of their size.
//-----------------------------------------------------------------------------
class CFontManager : public CTier2AppSystem< IFontManager >
{
public:
	// CUtlVector of 48-byte font amalgams
	uint8 m_FontAmalgams[ 0x18 ];

	// CUtlMap of the platform fonts
	uint8 m_unk040[ 0x38 ];

	CUtlSymbolTable m_FontDataSymbols;

	// CUtlMap of the font data files and CUtlVector of the custom fonts
	uint8 m_unk0D8[ 0x40 ];

	char m_szLanguage[ 64 ];

	// Font texture cache, points back to the font manager
	uint8 m_unk158[ 0x1078 ];

	CUtlVector< FontAliasList_t * > m_pFontAliasLists;
	CAtomicMutex m_FontTextureCacheMutex;
	bool m_bUnk11F4;
};

#endif // FONTMANAGER_H
