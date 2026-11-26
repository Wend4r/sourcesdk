//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Spotting of entities.
//
//=============================================================================//

#ifndef CSTRIKE15_CS_ENTITY_SPOTTING_H
#define CSTRIKE15_CS_ENTITY_SPOTTING_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"
#include "schemasystem/schematypes.h"
#include "networksystem/networkvar_vtable.h"

class CKV3TransferLoadContext;
class CKV3TransferSaveContext;
class CNetworkSerializerClassInfo;
struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// The virtual table of the schema struct EntitySpottedState_t. Which players spotted the entity, networked.
// The game class has members this declaration does not have.
//-----------------------------------------------------------------------------
abstract_class EntitySpottedState_tVTable
{
public:
	DECLARE_EMBEDDED_NETWORKVAR_NOTFLATTENED_VTABLE()
};

#endif // CSTRIKE15_CS_ENTITY_SPOTTING_H
