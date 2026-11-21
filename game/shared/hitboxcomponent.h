//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The hitbox component of a model entity.
//
//=============================================================================//

#ifndef HITBOXCOMPONENT_H
#define HITBOXCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "entity2/entitycomponent.h"

#include "schemacompiler/schemaon.h"

struct NetworkStateChanged_t;
struct NetworkStateChangedRemove_t;

//-----------------------------------------------------------------------------
// Embedded in CBaseModelEntity (m_CHitboxComponent). 24 bytes on both platforms.
//-----------------------------------------------------------------------------
schema class CHitboxComponent : public CEntityComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CHitboxComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

	// Forward a change of a networked member to the owner entity. NetworkStateChanged first
	// rebases the changed offsets from the component to the owner when the change has no path
	// index.
	virtual void NetworkStateChanged( const NetworkStateChanged_t &data );
	virtual void NetworkStateChangedRemove( const NetworkStateChangedRemove_t &data );

	// Releases m_nHitboxHandle
	virtual ~CHitboxComponent();

public:
	noschema CEntityOwnerPtr m_pOwner;

	// The slot of the component in the game's list of hitbox components; -1 until it is added.
	// The destructor frees the slot.
	noschema uint32 m_nHitboxHandle;

	float32 m_flBoundsExpandRadius;
};

#include "schemacompiler/schemaoff.h"

#endif // HITBOXCOMPONENT_H
