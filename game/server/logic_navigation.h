#ifndef LOGIC_NAVIGATION_H
#define LOGIC_NAVIGATION_H

#pragma once

#include "baseentity.h"
#include "entity2/ientitylistener.h"

//-----------------------------------------------------------------------------
// CLogicNavigation adds no method to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method). The Linux table has 1 more slot, 239: the override of a method of IEntityListener, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CLogicNavigationVTable : public TBase, public IEntityListener
{
public:
	// Overrides IEntityListener: a slot of this table on Linux only
	virtual void OnEntitySpawned( CEntityInstance *pEntity ) override = 0;
};

#endif // LOGIC_NAVIGATION_H
