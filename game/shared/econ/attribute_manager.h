#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H

#pragma once

#include "tier0/platform.h"
#include "tier0/utlvector.h"
#include "tier0/utlstringtoken.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CBaseEntity;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Virtual table of CAttributeManager, which applies the attributes of an entity's items to
// its values, and of CAttributeContainer derived from it (the manager of an item entity,
// CEconEntity::m_AttributeManager; it overrides methods only). The classes carry a VTable
// suffix and declare virtual methods only: the game classes of the same names are schema
// classes, so a module declares its own class with the members it needs and derives it from
// these:
//
//   class CAttributeManager : public CAttributeManagerVTable {};
//   class CAttributeContainer : public CAttributeContainerVTable< CAttributeManager > {};
//
// Windows slots 0-8; on Linux the destructor takes two slots, so every slot after it is one
// further.
//-----------------------------------------------------------------------------
abstract_class CAttributeManagerVTable
{
public:
	// The network variable wrapper of an embedded manager forwards the first and the last to
	// the owning entity
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()

	virtual ~CAttributeManagerVTable() = 0;

	virtual void InitializeAttributes( CBaseEntity *pEntity ) = 0;
	// The parameters after the third are not confirmed
	virtual float ApplyAttributeFloat( float flValue, CBaseEntity *pInitiator, CUtlStringToken nAttribHookName, CUtlVector< CBaseEntity * > *pItemList ) = 0;
	virtual void OnAttributeValuesChanged() = 0;
};

template < class TBase = CAttributeManagerVTable >
abstract_class CAttributeContainerVTable : public TBase
{
};

#endif // ATTRIBUTE_MANAGER_H
