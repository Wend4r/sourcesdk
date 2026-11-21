//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The render component of a model entity.
//
//=============================================================================//

#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "tier0/utlvector.h"
#include "entity2/entitycomponent.h"
#include "entity2/entitynetwork.h"
#include "skeletoninstance.h"

#include "schemacompiler/schemaon.h"

//-----------------------------------------------------------------------------
// The value that CRenderComponent::m_iv_enableRendering interpolates
//-----------------------------------------------------------------------------
struct EnableRenderingInfo_t
{
	uint16 m_nSplitscreenFlags;
	bool m_bEnableRendering;
};

//-----------------------------------------------------------------------------
// The render state of a model entity (CBaseModelEntity::m_CRenderComponent). 176 bytes on
// Windows, 192 on Linux (the delegates of the interpolated variable differ in size).
//-----------------------------------------------------------------------------
schema class CRenderComponent : public CEntityComponent
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CRenderComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

public:
	noschema CEntityOwnerPtr m_pOwner;

	CNetworkVarChainer __m_pChainEntity; META( MNotSaved );

	// The scene object updaters of the entity's scene objects; empty on the server
	noschema CUtlVector< SceneObjectUpdaterHandle_t > m_sceneObjectUpdaters;

	bool m_bIsRenderingWithViewModels; META( MNotSaved );
	uint32 m_nSplitscreenFlags; META( MNotSaved );
	bool m_bEnableRendering; META( MNotSaved );

	// Interpolates m_nSplitscreenFlags and m_bEnableRendering
	noschema CInterpolatedVarProcedural< EnableRenderingInfo_t > m_iv_enableRendering;

	// True at construction
	bool m_bInterpolationReadyToDraw; META( MNotSaved );
};

#include "schemacompiler/schemaoff.h"

#endif // RENDERCOMPONENT_H
