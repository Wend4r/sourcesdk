//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The light component of a light entity.
//
//=============================================================================//

#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#ifdef _WIN32
#pragma once
#endif

#include "color.h"
#include "tier0/utlstring.h"
#include "tier0/utlsignalslot.h"
#include "mathlib/vector.h"
#include "mathlib/lightdesc.h"
#include "entity2/entitycomponent.h"
#include "entity2/entitynetwork.h"
#include "resourcesystem/resourcehandle.h"
#include "resourcesystem/resourcehandletypes.h"
#include "skeletoninstance.h"

#include "schemacompiler/schemaon.h"

class CSceneLightObject;

//-----------------------------------------------------------------------------
// The light of a light entity (CLightEntity::m_CLightComponent); the light query game system
// builds the light descriptions of the lighting queries from it. 448 bytes on both platforms.
//-----------------------------------------------------------------------------
schema class CLightComponent : public CEntityComponent, public CUtlSlot
{
public:
	TYPEMETA( MGetKV3ClassDefaults );

	SchemaMetaInfoHandle_t< CSchemaClassInfo > Schema_DynamicBinding() override;
	datamap_t *GetDataDescMap() override;
	SchemaMetaInfoHandle_t< CSchemaClassInfo > GetSchemaBinding() override;
	CLASS_USES_KV3TRANSFER_VIRTUAL_OVERRIDE( CLightComponent );
	CEntityComponentHelper *GetComponentHelper() override;

	virtual const char *GetComponentName();

public:
	noschema CEntityOwnerPtr m_pOwner;

	CNetworkVarChainer __m_pChainEntity; META( MNotSaved );

	// Released through the scene system by the destructor; the server never creates one
	noschema CSceneLightObject *m_pLightSceneObject;

	// The server neither reads nor writes it and the constructor leaves it uninitialised
	noschema SceneObjectUpdaterHandle_t m_hUpdater;

	// Set from the class name of the owner at spawn: MATERIAL_LIGHT_POINT for light_omni and
	// light_capsule, MATERIAL_LIGHT_SPOT, MATERIAL_LIGHT_ORTHO, MATERIAL_LIGHT_DIRECTIONAL for
	// light_directional and light_environment, MATERIAL_LIGHT_DISABLE otherwise
	noschema LightType_t m_LightType;

	// Named by what it does: LIGHT_SOURCE_SHAPE_CAPSULE for light_capsule, set at spawn and copied
	// to LightDesc_t::m_nShape; a capsule light uses m_SecondaryColor and m_flCapsuleLength
	noschema uint8 m_nLightSourceShape;

	Color m_Color;
	Color m_SecondaryColor;
	float32 m_flBrightness;
	float32 m_flBrightnessScale;
	float32 m_flBrightnessMult;
	float32 m_flRange;
	float32 m_flFalloff;
	float32 m_flAttenuation0;
	float32 m_flAttenuation1;
	float32 m_flAttenuation2;
	float32 m_flTheta;
	float32 m_flPhi;
	CStrongHandle< InfoForResourceTypeCTextureBase > m_hLightCookie;
	int32 m_nCascades;
	int32 m_nCastShadows;
	int32 m_nShadowWidth;
	int32 m_nShadowHeight;
	bool m_bRenderDiffuse;
	int32 m_nRenderSpecular;
	bool m_bRenderTransmissive;
	float32 m_flOrthoLightWidth;
	float32 m_flOrthoLightHeight;
	int32 m_nStyle;
	CUtlString m_Pattern;
	int32 m_nCascadeRenderStaticObjects;
	float32 m_flShadowCascadeCrossFade;
	float32 m_flShadowCascadeDistanceFade;
	float32 m_flShadowCascadeDistance0;
	float32 m_flShadowCascadeDistance1;
	float32 m_flShadowCascadeDistance2;
	float32 m_flShadowCascadeDistance3;
	int32 m_nShadowCascadeResolution0;
	int32 m_nShadowCascadeResolution1;
	int32 m_nShadowCascadeResolution2;
	int32 m_nShadowCascadeResolution3;
	bool m_bUsesBakedShadowing;
	int32 m_nShadowPriority;
	int32 m_nBakedShadowIndex;
	int32 m_nLightPathUniqueId;
	int32 m_nLightMapUniqueId;
	bool m_bRenderToCubemaps;
	bool m_bAllowSSTGeneration;
	int32 m_nDirectLight;
	int32 m_nBounceLight;
	float32 m_flBounceScale;
	float32 m_flFadeMinDist;
	float32 m_flFadeMaxDist;
	float32 m_flShadowFadeMinDist;
	float32 m_flShadowFadeMaxDist;
	bool m_bEnabled;
	bool m_bFlicker;
	bool m_bPrecomputedFieldsValid;
	Vector m_vPrecomputedBoundsMins;
	Vector m_vPrecomputedBoundsMaxs;
	Vector m_vPrecomputedOBBOrigin;
	QAngle m_vPrecomputedOBBAngles;
	Vector m_vPrecomputedOBBExtent;
	float32 m_flPrecomputedMaxRange;
	int32 m_nFogLightingMode;
	float32 m_flFogContributionStength;
	float32 m_flNearClipPlane;
	Color m_SkyColor;
	float32 m_flSkyIntensity;
	Color m_SkyAmbientBounce;
	bool m_bUseSecondaryColor;
	bool m_bMixedShadows; META( MNotSaved );
	GameTime_t m_flLightStyleStartTime;
	float32 m_flCapsuleLength;
	float32 m_flMinRoughness;

	// Named by its initial value: the constructor sets it to -FLT_MAX, the usual "never" time; no
	// server code reads it
	noschema float32 m_flLastUpdateTime;

	// Named by what it does: when set, the light description takes its cookie texture from it
	// instead of m_hLightCookie; the scene system releases it with the scene object
	noschema CStrongHandle< InfoForResourceTypeCTextureBase > m_hLightCookieOverride;

	bool m_bPvsModifyEntity;
};

#include "schemacompiler/schemaoff.h"

#endif // LIGHTCOMPONENT_H
