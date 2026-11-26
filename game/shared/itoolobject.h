//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The tool side of entities the tools bake.
//
//=============================================================================//

#ifndef ITOOLOBJECT_H
#define ITOOLOBJECT_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"

class CEntityKeyValues;
class CRenderAttributes;
class CSceneObject;
class ISceneWorld;
struct matrix3x4_t;
struct SceneFogParams_t;

//-----------------------------------------------------------------------------
// The tool side of an entity that the tools bake (CBaseCubemap, CBaseLightProbeVolume).
// The server has no scene system: every method that would create or describe a scene
// object returns null or false there.
//-----------------------------------------------------------------------------
abstract_class IToolObject
{
public:
	virtual ~IToolObject() {}

	// Reads the tool key values (cubemap: "customcubemaptexture", "influenceradius", "box_mins", ...).
	virtual void UpdateStateFromKeyValues( const CEntityKeyValues *pKeyValues ) = 0;

	// Empty.
	virtual void GetStateAsKeyValues( CEntityKeyValues *pOutKeyValues ) const = 0;

	// Null; the cubemap refreshes its bound resource first.
	virtual CSceneObject *CreateSceneObjectForRenderInstance( const matrix3x4_t &mTransform, ISceneWorld *pSceneWorld ) const = 0;

	// Empty.
	virtual void WriteSceneViewRenderAttributes( CRenderAttributes &renderAttributes ) = 0;

	// Returns false.
	virtual bool GenerateFogState( SceneFogParams_t *pOutFogState ) const = 0;

	// Returns null.
	virtual CSceneObject *GenerateDirectionalLight( ISceneWorld *pSceneWorld ) const = 0;

	// Returns null.
	virtual CSceneObject *GenerateSkybox( ISceneWorld *pSceneWorld ) const = 0;
};

#endif // ITOOLOBJECT_H
