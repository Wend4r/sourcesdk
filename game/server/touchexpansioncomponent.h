//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The touch expansion component: a physics body welded to its owner that widens the
//          volume the owner touches triggers with.
//
//=============================================================================//

#ifndef TOUCHEXPANSIONCOMPONENT_H
#define TOUCHEXPANSIONCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "gametrace.h"
#include "entity2/entitycomponent.h"

#include "schemacompiler/schemaon.h"

class IPhysicsBody;
class IPhysicsJoint;

//-----------------------------------------------------------------------------
// Embedded in CCSPlayerPawnBase (m_CTouchExpansionComponent). The members are named by what the
// code does. 80 bytes on both platforms.
//-----------------------------------------------------------------------------
schema class CTouchExpansionComponent : public CEntityComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CTouchExpansionComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

public:
	noschema CEntityOwnerPtr m_pOwner;

	// The body the physics world creates at set-up, named "Touch Expansion: <name>(<index>)";
	// it carries m_CollisionAttribute and the owner's entity handle as user data
	noschema IPhysicsBody *m_pBody;

	// The joint that welds m_pBody to the owner's physics body; destroyed together with it
	noschema IPhysicsJoint *m_pJoint;

	// The constructor clears the four flag bytes as one int; no code reads or writes the first two
	noschema bool m_bUnreferencedFlag0;
	noschema bool m_bUnreferencedFlag1;

	// While it is false, set-up adds the interacts-with layers of m_pBody to the owner's
	// collision attribute
	noschema bool m_bKeepOwnerInteractsWith;

	// When set, set-up limits the collision functions of m_pBody to those of the owner
	noschema bool m_bInheritOwnerCollisionFunctions;

	// The collision attribute set-up gives m_pBody: the requested layers and collision group and
	// the owner's entity id. The collision function mask is 7 at construction and 4 after set-up
	noschema RnCollisionAttr_t m_CollisionAttribute;
};

#include "schemacompiler/schemaoff.h"

#endif // TOUCHEXPANSIONCOMPONENT_H
