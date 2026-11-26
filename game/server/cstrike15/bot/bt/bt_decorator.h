#ifndef BT_DECORATOR_H
#define BT_DECORATOR_H

#ifdef _WIN32
#pragma once
#endif

#include "bt_node.h"
#include "tier0/utlvector.h"
#include "tier0/utlstring.h"
#include "kv3lib/keyvalues3.h"

class Vector;

// A record of an entity the bot has sensed, as the sensor decorator stores it in the
// blackboard (44 bytes). Its layout is not declared.
struct BtSensedEntity_t;

//-----------------------------------------------------------------------------
// CBtDecoratorPicker, the base of the decorator_picker_* nodes (160 bytes). Its Start resolves
// the input and output blackboard entries; its Eval copies the sensed-entity records of the
// input entry, calls Pick with the agent's position, fails when Pick returns false and writes
// one of the two record lists Pick leaves (m_bNegated chooses which) to the output entry, or
// back to the input entry when no output key is set. Adds one method to CBtNode: Linux slot
// 11, Windows slot 10. The decorator between the two adds none.
//
// A key with a domain names an entry of the global blackboard ("<domain>@@<key>"), a key
// without one an entry of the local blackboard.
//
// TBase stands for CBtNodeDecorator (88 bytes, schema class): the members below sit at the
// offsets of the game when TBase is the full class.
//-----------------------------------------------------------------------------
template < class TBase = CBtNodeVTable >
abstract_class CBtDecoratorPicker : public TBase
{
public:
	// Moves the chosen records from candidates to picked. Named by what it does.
	virtual bool Pick( const Vector &vecOrigin, CUtlVector< BtSensedEntity_t > &candidates, CUtlVector< BtSensedEntity_t > &picked ) = 0;

public:
	// KeyValues3 "input_domain". +88
	CUtlString m_szInputDomain;
	// The blackboard key of the sensed-entity records to pick from (KeyValues3 "input",
	// required). +96
	CUtlString m_szInputKey;
	// KeyValues3 "output_domain". +104
	CUtlString m_szOutputDomain;
	// The blackboard key the picked records go to (KeyValues3 "output"). +112
	CUtlString m_szOutputKey;
	// The input entry, found by Start. +120
	KeyValues3 *m_pInputValue;
	// The output entry, found by Start; null without an output key. +128
	KeyValues3 *m_pOutputValue;
	// Chooses which of the two record lists of Pick Eval writes (KeyValues3 "negated"). +136
	bool m_bNegated;
	// The full name of the input entry ("<domain>@@<key>" or the key alone), set by Start. Named
	// by what it holds. +144
	CUtlString m_szQualifiedInputKey;
	// The full name of the output entry, set by Start (the input name without an output key).
	// Named by what it holds. +152
	CUtlString m_szQualifiedOutputKey;
};

//-----------------------------------------------------------------------------
// CBtDecoratorRanker, the base of the ranking decorators (120 bytes; decorator_ranker_dist adds
// no data). Its Start finds the input entry in the local blackboard; its Eval calls Rank on the
// sensed-entity records of that entry with the agent's position and writes the scored records
// back. The distance ranker scores each record with one over its distance. Adds one method to
// CBtNode: Linux slot 11, Windows slot 10.
//
// TBase stands for CBtNodeDecorator (88 bytes, schema class): the members below sit at the
// offsets of the game when TBase is the full class.
//-----------------------------------------------------------------------------
template < class TBase = CBtNodeVTable >
abstract_class CBtDecoratorRanker : public TBase
{
public:
	// Writes the score of every record. Named by what it does.
	virtual bool Rank( const Vector &vecOrigin, CUtlVector< BtSensedEntity_t > &records ) = 0;

public:
	// The member name made from m_szInputKey by Start, used to look the entry up. Named by what
	// it holds. +88
	CKV3MemberName m_InputKeyName;
	// The blackboard key of the sensed-entity records (KeyValues3 "input", required). +104
	CUtlString m_szInputKey;
	// The input entry, found by Start. +112
	KeyValues3 *m_pInputValue;
};

#endif // BT_DECORATOR_H
