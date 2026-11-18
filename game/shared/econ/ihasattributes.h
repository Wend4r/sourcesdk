#ifndef IHASATTRIBUTES_H
#define IHASATTRIBUTES_H

#pragma once

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"

class CAttributeContainer;
class CAttributeList;
class CAttributeManager;
class CBaseEntity;
class CNetworkSerializerClassInfo;

//-----------------------------------------------------------------------------
// The interface of the entities that carry item attributes: CEconEntity (the
// weapons and the wearables), CChicken and CPlantedC4 inherit it.
//
// Virtual methods only, in the order of the game's table (7 slots on both
// platforms, no destructor). The names of the attribute getters and of
// ReapplyProvision are the ones of ihasattributes.h of Counter-Strike: Global
// Offensive; the first two methods are the schema pair every entity class has,
// as the table of Dota 2 2017 (DWARF) shows. A class that also derives from
// CEntityInstance overrides those two with its own, so they get no slot of their
// own in its primary table.
//-----------------------------------------------------------------------------
abstract_class IHasAttributes
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;
	// m_AttributeManager of the entity
	virtual CAttributeManager *GetAttributeManager() = 0;
	// m_AttributeManager of the entity as its container
	virtual CAttributeContainer *GetAttributeContainer() = 0;
	// The entity that owns the attributes
	virtual CBaseEntity *GetAttributeOwner() = 0;
	// The attribute list of the item
	virtual CAttributeList *GetAttributeList() = 0;
	// The attributes follow a change of the owner
	virtual void ReapplyProvision() = 0;
};

#endif // IHASATTRIBUTES_H
