#ifndef FONTAMALGAM_H
#define FONTAMALGAM_H

#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include <tier0/wchartypes.h>

class CFontBase;

//-----------------------------------------------------------------------------
// A font made of the fonts that cover ranges of code points
//-----------------------------------------------------------------------------
class CFontAmalgam
{
public:
	struct SFontRange
	{
		uchar32 lowRange;
		uchar32 highRange;

		// Created on first use from deferredFontName, which is purged then
		CFontBase *font;
		CUtlString deferredFontName;
	};

	CUtlVector< SFontRange > m_Fonts;

	// Passed to the creation of the deferred fonts
	int m_propTall;
	int m_propWeight;
	int m_propBlur;
	int m_propScanlines;
	int m_propFlags;
};

#endif // FONTAMALGAM_H
