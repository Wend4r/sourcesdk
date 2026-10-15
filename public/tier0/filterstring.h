#ifndef FILTERSTRING_H
#define FILTERSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

// The comparison of CFilterString. Values 4 to 7 compare the input as a number with the filter
// (less, greater, less or equal, greater or equal), the lower values match text.
enum FilterStringOp_t : int;
enum FilterStringCase_t : int;

//-----------------------------------------------------------------------------
// Matches strings against a filter of "|" separated alternatives of " " separated
// terms, where a leading "-" excludes the term
//-----------------------------------------------------------------------------
class CFilterString
{
public:
	struct MatchString_t
	{
		bool m_bExcludeOnMatch;
		CUtlString m_String;
	};

	struct OrGroup_t
	{
		CUtlVector< MatchString_t > m_AndStrings;
	};

	DLL_CLASS_IMPORT CFilterString( const char *pString, FilterStringOp_t eOp, FilterStringCase_t eCase );

	DLL_CLASS_IMPORT void SetString( const char *pString, FilterStringOp_t eOp, FilterStringCase_t eCase );
	DLL_CLASS_IMPORT bool DoesMatchString( const char *pValue ) const;

private:
	FilterStringOp_t m_eOp;
	FilterStringCase_t m_eCase;
	CUtlString m_InputString;
	CUtlVector< OrGroup_t > m_OrGroups;
	bool m_bIsNumber;
	float m_flNumber;
};

#endif // FILTERSTRING_H
