#ifndef GENERIC_EXPR_H
#define GENERIC_EXPR_H

#pragma once

#include "tier0/platform.h"
#include "tier0/bufferstring.h"
#include "tier0/utlvector.h"

//-----------------------------------------------------------------------------
// A node of a parsed generic expression: the kind of the part, then the members of the derived
// part. The parts of the game derive from it: binary, unary and ternary operators, literals
// (bool, int, float, string), properties, variable references and function calls; only the
// function call adds methods. The method names are named by what the code does.
//-----------------------------------------------------------------------------
abstract_class CGenericExprPart
{
public:
	// Deletes the child parts
	virtual ~CGenericExprPart() {}

	// Appends the part to sOut ("Func( %s )" for a call, the operator of a binary part)
	virtual void ToString( CBufferString &sOut ) const = 0;
	// Compares the kind and the members, recursing through the children
	virtual bool IsEqual( const CGenericExprPart *pOther ) const = 0;
	// 2 for a binary part, 3 for the ternary one, 1 for unary parts and properties, the
	// argument count for a call, 0 for literals
	virtual int GetChildCount() const = 0;
	// The address of the child pointer nIndex
	virtual CGenericExprPart **GetChildPtr( int nIndex ) = 0;

public:
	// Named by what it holds: the kind of the part, set by the parser and compared first by
	// IsEqual: 0 unary, 1 binary, 2 ternary, 3 string, 4 float, 5 int, 6 bool, 7 variable
	// reference, 8 property, 9 function call.
	int32 m_nPartType;
};

//-----------------------------------------------------------------------------
// A unary operator applied to one child.
//-----------------------------------------------------------------------------
class CGenericExpr_Unary : public CGenericExprPart
{
public:
	// Named by what it holds: 1 "UnNot", 2 "UnNegate".
	int32 m_nOperator;
	CGenericExprPart *m_pChild;
};

//-----------------------------------------------------------------------------
// A binary operator applied to two children.
//-----------------------------------------------------------------------------
class CGenericExpr_Binary : public CGenericExprPart
{
public:
	// Named by what it holds: 1 "BinMul", 2 "BinDiv", 3 "BinMod", 4 "BinAdd", 5 "BinSub",
	// 6 "BinLt", 7 "BinGt", 8 "BinLtE", 9 "BinGtE", then "BinEq", "BinNe", "BinLogAnd" and
	// "BinLogOr".
	int32 m_nOperator;
	CGenericExprPart *m_pLeft;
	CGenericExprPart *m_pRight;
};

//-----------------------------------------------------------------------------
// "condition ? true : false".
//-----------------------------------------------------------------------------
class CGenericExpr_TernaryConditional : public CGenericExprPart
{
public:
	// Named by what they hold.
	CGenericExprPart *m_pCondition;
	CGenericExprPart *m_pTrueValue;
	CGenericExprPart *m_pFalseValue;
};

//-----------------------------------------------------------------------------
// The literals.
//-----------------------------------------------------------------------------
class CGenericExpr_BoolLiteral : public CGenericExprPart
{
public:
	bool m_bValue;
};

class CGenericExpr_IntLiteral : public CGenericExprPart
{
public:
	int32 m_nValue;
};

class CGenericExpr_FloatLiteral : public CGenericExprPart
{
public:
	float64 m_flValue;
};

class CGenericExpr_StringLiteral : public CGenericExprPart
{
public:
	CBufferString m_sValue;
	// Named by what the code does: 1 for one of the two string tokens of the parser, 0 for
	// the other; compared by IsEqual.
	int32 m_nStringKind;
};

//-----------------------------------------------------------------------------
// A variable referenced by name.
//-----------------------------------------------------------------------------
class CGenericExpr_VariableReference : public CGenericExprPart
{
public:
	CBufferString m_sName;
	// Named by what the code does: -1 when the parser creates the part; the parser does not
	// read it.
	int32 m_nBindingIndex;
};

//-----------------------------------------------------------------------------
// A property of the value of its child ("child.name").
//-----------------------------------------------------------------------------
class CGenericExpr_Property : public CGenericExprPart
{
public:
	CGenericExprPart *m_pChild;
	CBufferString m_sName;
};

//-----------------------------------------------------------------------------
// A function call of a generic expression; its children are the arguments.
//-----------------------------------------------------------------------------
abstract_class CGenericExpr_FunctionCall : public CGenericExprPart
{
public:
	// GetChildCount returns it
	virtual int GetArgumentCount() const = 0;
	virtual CGenericExprPart *GetArgument( int nIndex ) const = 0;

public:
	CBufferString m_sName;
	CUtlVector< CGenericExprPart * > m_Arguments;
	// Named by what the code does: -1 when the parser creates the part; the parser does not
	// read it.
	int32 m_nBindingIndex;
};

#endif // GENERIC_EXPR_H
