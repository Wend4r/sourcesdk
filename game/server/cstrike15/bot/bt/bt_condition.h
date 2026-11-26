#ifndef BT_CONDITION_H
#define BT_CONDITION_H

#ifdef _WIN32
#pragma once
#endif

#include "bt_node.h"
#include "tier0/utlstring.h"

//-----------------------------------------------------------------------------
// Virtual table of CBtNodeCondition (schema class, a decorator; m_bNegated at 88), the base of
// the condition_* nodes. Its Eval fails when CheckCondition differs from the negation flag.
// Adds one method to CBtNode: Linux slot 11, Windows slot 10. The decorator between the two
// adds none.
//-----------------------------------------------------------------------------
template < class TBase = CBtNodeVTable >
abstract_class CBtNodeConditionVTable : public TBase
{
public:
	// The test of the condition node. Named by what it does.
	virtual bool CheckCondition() = 0;
};

//-----------------------------------------------------------------------------
// CBtNodeConditionComparison, the base of condition_is_equal, _is_greater, _is_greater_equal,
// _is_less and _is_less_equal (120 bytes; the five leaves add no data). Its CheckCondition
// reads the blackboard value named by m_szSourceInputKey and compares it with Compare: with
// the blackboard value named by m_szDestinationInputKey when that key is set (as strings when
// both are strings, else as integers), with m_nDestinationValue otherwise. Adds two methods:
// Linux slots 12 and 13, Windows 12 and 11 (the two overloads swap places on Windows).
//
// TBase stands for CBtNodeCondition (96 bytes, schema class): the members below sit at the
// offsets of the game when TBase is the full class.
//-----------------------------------------------------------------------------
template < class TBase = CBtNodeConditionVTable<> >
abstract_class CBtNodeConditionComparison : public TBase
{
public:
	// For string values. Named by what it does.
	virtual bool Compare( const CUtlString &a, const CUtlString &b ) = 0;
	// For integer values. Named by what it does.
	virtual bool Compare( int a, int b ) = 0;

public:
	// The blackboard key of the value compared (KeyValues3 "source", required). +96
	CUtlString m_szSourceInputKey;
	// The blackboard key of the value compared against (KeyValues3 "destination" given as a
	// string); empty when the destination is an integer. +104
	CUtlString m_szDestinationInputKey;
	// The integer compared against (KeyValues3 "destination" given as an integer). +112
	int m_nDestinationValue;
};

#endif // BT_CONDITION_H
