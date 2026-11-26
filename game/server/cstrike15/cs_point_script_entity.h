#ifndef CSTRIKE15_CS_POINT_SCRIPT_ENTITY_H
#define CSTRIKE15_CS_POINT_SCRIPT_ENTITY_H

#pragma once

#include "../baseentity.h"
#include "resourcesystem/itoolsresourcelistener.h"

//-----------------------------------------------------------------------------
// CCSPointScriptEntity adds no method to CBaseEntity. The Linux table has 1 more slot, 239: the override of a method of IToolsResourceListener, which the
// Itanium ABI also enters into the primary table and MSVC does not.
// The second bases sit at offsets 8, 16, ... here, not where the game has them: do not convert a pointer to them.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CCSPointScriptEntityVTable : public TBase, public IToolsResourceListener
{
public:
	// Overrides IToolsResourceListener: a slot of this table on Linux only
	virtual void NotifyResourceStatusChange( const ResourceStatusChange_t *pChanges, int64 nCount ) override = 0;
};

#endif // CSTRIKE15_CS_POINT_SCRIPT_ENTITY_H
