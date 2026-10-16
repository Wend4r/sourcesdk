#ifndef FILTERSTRING_H
#define FILTERSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlstring.h"
#include "tier0/utlvector.h"

// The comparison of CFilterString
enum FilterStringOp_t : int
{
	// The input contains the terms of one of the "|" separated alternatives and none of
	// the excluded ones
	FILTER_STRING_OP_CONTAINS = 0,

	// The input equals the whole filter
	FILTER_STRING_OP_EQUALS = 1,

	// The negations of the two above
	FILTER_STRING_OP_NOT_CONTAINS = 2,
	FILTER_STRING_OP_NOT_EQUALS = 3,

	// The input as a number compared with the filter as a number
	FILTER_STRING_OP_LESS = 4,
	FILTER_STRING_OP_GREATER = 5,
	FILTER_STRING_OP_LESS_EQUAL = 6,
	FILTER_STRING_OP_GREATER_EQUAL = 7,
};

// The case handling of the text comparisons of CFilterString
enum FilterStringCase_t : int
{
	FILTER_STRING_CASE_INSENSITIVE = 0,
	FILTER_STRING_CASE_SENSITIVE = 1,
};

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

	// An empty filter matches every string
	bool IsEmpty() const { return m_InputString.IsEmpty(); }
	const char *GetString() const { return m_InputString.Get(); }

private:
	FilterStringOp_t m_eOp;
	FilterStringCase_t m_eCase;
	CUtlString m_InputString;
	CUtlVector< OrGroup_t > m_OrGroups;
	bool m_bIsNumber;
	float m_flNumber;
};

#endif // FILTERSTRING_H
