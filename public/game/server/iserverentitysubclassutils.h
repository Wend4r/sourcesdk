//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Server-side queries about entity subclasses and their VData classes
//
//===========================================================================//

#ifndef ISERVERENTITYSUBCLASSUTILS_H
#define ISERVERENTITYSUBCLASSUTILS_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "appframework/iappsystem.h"
#include "tier0/utlstring.h"
#include "tier1/utlvector.h"

enum EntitySubclassScope_t : int
{
	SUBCLASS_SCOPE_NONE = -1,
	SUBCLASS_SCOPE_PRECIPITATION = 0,
	SUBCLASS_SCOPE_PLAYER_WEAPONS = 1,

	SUBCLASS_SCOPE_COUNT
};

struct EntitySubclassVDataClass_t
{
	const char *m_pszVDataClassName;
	EntitySubclassScope_t m_eScope;
};

abstract_class IServerEntitySubclassUtils : public IAppSystem
{
public:
	virtual bool GetSubclassVDataClass( const char *pszEntityClassName, EntitySubclassVDataClass_t *pOut ) = 0;

	virtual void GetSchemaBaseClassNames( const char *pszClassName, CUtlVector< CUtlString > *pBaseClassNames ) = 0;
};

#endif // ISERVERENTITYSUBCLASSUTILS_H
