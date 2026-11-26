#ifndef POINT_TEMPLATE_H
#define POINT_TEMPLATE_H

#pragma once

#include "baseentity.h"

//-----------------------------------------------------------------------------
// CPointTemplate adds 2 methods to CLogicalEntity (through CLogicalEntity and CServerOnlyEntity, which add no method): Linux slots 239-240, Windows 238-239.
//-----------------------------------------------------------------------------
template < class TBase = CBaseEntityVTable >
abstract_class CPointTemplateVTable : public TBase
{
public:
	virtual int GetDesiredSpawnGroupType() = 0;
	virtual bool ForcePrecacheSpawnGroupLump() const = 0;
};

#endif // POINT_TEMPLATE_H
