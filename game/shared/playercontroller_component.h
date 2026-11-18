#ifndef PLAYERCONTROLLER_COMPONENT_H
#define PLAYERCONTROLLER_COMPONENT_H

#pragma once

#include "tier0/platform.h"
#include "entitytypes.h"
#include "schemasystem/schematypes.h"

class CNetworkSerializerClassInfo;

//-----------------------------------------------------------------------------
// Virtual table of CPlayerControllerComponent, the base of the services of a
// player controller, as laid out by the server module.
//
// The class follows the rules of CPlayerPawnComponentVTable: virtual methods only,
// a VTable suffix, and the classes derived from it are templates over their base
// class. See playerpawn_component.h.
//
// A controller keeps its components in a list and runs the methods from Activate
// to PostThink for each of them from its own method of the same name.
//-----------------------------------------------------------------------------
abstract_class CPlayerControllerComponentVTable
{
public:
	virtual SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() = 0;
	virtual const CNetworkSerializerClassInfo *GetSerializerClassInfo() = 0;
	virtual ~CPlayerControllerComponentVTable() = 0;

	// The second call after the component is linked to its controller, after PreInit
	virtual void Init() = 0;
	virtual void Activate( ActivateType_t activateType ) = 0;
	virtual void UpdateOnRemove() = 0;
	virtual void OnRestore() = 0;
	virtual void PreThink() = 0;
	virtual void PostThink() = 0;

	// The first call after the component is linked to its controller, every class chains to its base
	virtual void PreInit() = 0;
};

#endif // PLAYERCONTROLLER_COMPONENT_H
