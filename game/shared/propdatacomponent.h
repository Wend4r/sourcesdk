//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The prop data component of breakable props.
//
//=============================================================================//

#ifndef PROPDATACOMPONENT_H
#define PROPDATACOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlsymbollarge.h"
#include "entity2/entitycomponent.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// The damage modifiers and physics properties of a breakable prop, embedded in CBreakable,
// CBreakableProp and CRagdollProp (m_CPropDataComponent). 64 bytes on both platforms.
//-----------------------------------------------------------------------------
schema class CPropDataComponent : public CEntityComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CPropDataComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

public:
	noschema CEntityOwnerPtr m_pOwner;

	// 1.0 at construction
	float32 m_flDmgModBullet;
	float32 m_flDmgModClub;
	float32 m_flDmgModExplosive;
	float32 m_flDmgModFire;
	CUtlSymbolLarge m_iszPhysicsDamageTableName;
	CUtlSymbolLarge m_iszBasePropData;
	int32 m_nInteractions;
	bool m_bSpawnMotionDisabled;
	int32 m_nDisableTakePhysicsDamageSpawnFlag;
	int32 m_nMotionDisabledSpawnFlag;
};

#include "schemacompiler/schemaoff.h"

#endif // PROPDATACOMPONENT_H
