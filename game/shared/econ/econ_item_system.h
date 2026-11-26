//====== Copyright (c) 1996-2010, Valve Corporation, All rights reserved. =======
//
// Purpose: The item system: the owner of the item schema
//
//=============================================================================

#ifndef ECON_ITEM_SYSTEM_H
#define ECON_ITEM_SYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "iecon_item_system.h"

struct baseitemcriteria_t;
class CCStrike15ItemSchema;

//-----------------------------------------------------------------------------
// The item system (the game derives CCStrike15ItemSystem from it, which adds no virtual method
// and no member). 16 bytes. ItemSystem() creates the one instance on first use, together with the
// item schema it owns. Implements IEconItemSystem and adds two methods, Linux slots 4-6,
// Windows 4-5.
//-----------------------------------------------------------------------------
abstract_class CEconItemSystem : public IEconItemSystem
{
public:
	// Deletes m_pItemSchema
	virtual ~CEconItemSystem() = 0;

	// Picks an item definition that matches the criteria; returns -1 in CEconItemSystem itself
	virtual int GenerateBaseItem( baseitemcriteria_t *pCriteria ) = 0;

public:
	// The item schema (2384 bytes), created with the item system; GetItemSchemaInterface returns it
	CCStrike15ItemSchema *m_pItemSchema;
};

#endif // ECON_ITEM_SYSTEM_H
