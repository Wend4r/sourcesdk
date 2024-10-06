#ifndef SPLITSTRING_H
#define SPLITSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier1/utlvector.h"

// <Sergiy> placing it here a few days before Cert to minimize disruption to the rest of codebase
class CSplitString: public CUtlVector< char* >
{
public:
	CSplitString(const char *pString, const char *pSeparator, bool bIncludeSeparators = false) : m_szBuffer(nullptr)
	{
		Split( pString, -1, &pSeparator, 1, bIncludeSeparators);
	}

	CSplitString(const char *pString, const char **pSeparators, int nSeparators, bool bIncludeSeparators = false) : m_szBuffer(nullptr)
	{
		Split(pString, -1, pSeparators, nSeparators, bIncludeSeparators);
	}

	void SplitString2( char const *pString, const char **pSeparators, int nSeparators )
	{
		V_SplitString2( pString, pSeparators, nSeparators, *this );
	}

	//
	// NOTE: If you want to make Construct() public and implement Purge() here, you'll have to free m_szBuffer there
	//
private:
	DLL_CLASS_IMPORT void Split(const char *pString, int stringSize, const char **pSeparators, int nSeparators, bool bIncludeSeparators);

	void PurgeAndDeleteElements()
	{
		Purge();
	}

private:
	char *m_szBuffer; // a copy of original string, with '\0' instead of separators
};

#endif //#ifndef SPLITSTRING_H
