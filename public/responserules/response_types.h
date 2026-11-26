//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Types of the response rules system
//
//=============================================================================//

#ifndef RESPONSE_TYPES_H
#define RESPONSE_TYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "schemasystem/schematypes.h"

class CRR_Response;
class CResponseCriteriaSet;

// The kind of a response (speak, sentence, scene, ...); the values are not described here
enum ResponseType_t : int;

//-----------------------------------------------------------------------------
// Decides whether a response may be used. One slot on both platforms, no virtual
// destructor. CAI_Expresser derives from it.
//-----------------------------------------------------------------------------
abstract_class IResponseFilter
{
public:
	virtual bool IsValidResponse( ResponseType_t type, const char *pszValue ) = 0;
};

//-----------------------------------------------------------------------------
// Virtual table of CResponseCriteriaSet, the criteria a response is chosen by. The class carries a
// VTable suffix and declares virtual methods only: the game class of the same name is a schema
// class whose members are not described here. One slot on both platforms, no virtual destructor.
//-----------------------------------------------------------------------------
abstract_class CResponseCriteriaSetVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
};

//-----------------------------------------------------------------------------
// The response rules system of the game (CResponseSystem, CGameResponseSystem). The Windows server
// library has no table of these abstract classes, so the Windows slots follow from the
// declaration order: the destructor takes one slot there and two on Linux.
//-----------------------------------------------------------------------------
abstract_class IResponseSystem
{
public:
	virtual ~IResponseSystem() {}
	virtual bool FindBestResponse( const CResponseCriteriaSet &set, CRR_Response &response, IResponseFilter *pFilter ) = 0;
	virtual void GetAllResponses( CUtlVector< CRR_Response > *pResponses ) = 0;
	// Looks a pointer up in a map member of the response system
	virtual void *Unk_3( void *pKey ) = 0;
	// The random stream of the response system; CGameResponseSystem asks its owner for it
	virtual void *Unk_4() = 0;
	// Returns an int member
	virtual int Unk_5() = 0;
};

namespace RR
{
//-----------------------------------------------------------------------------
// An operator of a response context ("++", "--", "!"); CIncrementOperator, CDecrementOperator
// and CToggleOperator override it; the base copies the value. One slot on both platforms, no
// virtual destructor. The game implements the virtual method and keeps one static object of each
// operator.
//-----------------------------------------------------------------------------
class CApplyContextOperator
{
public:
	virtual bool Apply( const char *pOldValue, const char *pOperator, char *pNewValue, int nBufSize );

public:
	// How many characters of the value the operator takes ("++" is 2).
	int m_nSkipChars;
};
}

#endif // RESPONSE_TYPES_H
