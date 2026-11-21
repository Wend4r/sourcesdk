//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
//===========================================================================//

// light structure definitions.
#ifndef LIGHTDESC_H
#define LIGHTDESC_H

#include <mathlib/vector.h>

//-----------------------------------------------------------------------------
// Light structure
//-----------------------------------------------------------------------------
enum LightType_t
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
	MATERIAL_LIGHT_ORTHO,
	MATERIAL_LIGHT_ENVIRONMENT_PROBE,
};

// Bits of LightDesc_t::m_Flags that RecalculateDerivedValues sets
enum LightType_OptimizationFlags_t
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
	LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED = 8,
};

// Values of LightDesc_t::m_nShape
enum LightSourceShape_t
{
	LIGHT_SOURCE_SHAPE_SPHERE = 0,
	LIGHT_SOURCE_SHAPE_CAPSULE,
	LIGHT_SOURCE_SHAPE_RECTANGLE,
};

//-----------------------------------------------------------------------------
// The description of a light that the light components build and the scene
// system takes (288 bytes, 16 aligned, on both platforms)
//-----------------------------------------------------------------------------
struct LightDesc_t
{
	LightType_t m_Type;
	Vector m_Color;							// color * brightness
	Vector m_BounceColor;					// the secondary color of a capsule light
	float m_Range;							// 0 = infinite
	float m_Falloff;						// angular falloff exponent for spot lights
	float m_Attenuation0;					// constant distance falloff term
	float m_Attenuation1;					// linear term of falloff
	float m_Attenuation2;					// quadratic term of falloff

	// NOTE: theta and phi are *half angles*
	float m_Theta;							// inner cone angle, no angular falloff within this cone
	float m_Phi;							// outer cone angle

	int m_nLightCookieTextureIndex;			// render device index of the cookie texture, -1 without one
	uint8 m_nDirectLight;
	uint8 m_nCastShadows;
	uint8 m_nShadowCascadeCount;
	uint8 m_nCascadeRenderStaticObjects;
	int m_nShadowWidth;
	int m_nShadowHeight;
	float m_flShadowCascadeDistance[ 4 ];
	int m_nShadowCascadeResolution[ 4 ];
	float m_flShadowCascadeCrossFade;
	float m_flShadowCascadeDistanceFade;
	int m_nShadowPriority;
	uint8 m_nBakedShadowIndex;
	uint8 m_nBounceLight;
	bool m_bAllowSSTGeneration;
	uint8 m_nFogLightingMode;
	bool m_bRenderDiffuse;
	bool m_bRenderSpecular;
	bool m_bRenderTransmissive;
	uint8 m_nShape;							// LightSourceShape_t
	float m_flLightSourceDim0;
	float m_flLightSourceDim1;
	float m_flLightSourceSize0;				// width of an ortho light
	float m_flLightSourceSize1;				// height of an ortho light
	float m_flPrecomputedMaxRange;
	float m_flFogContributionStength;
	float m_flNearClipPlane;

	Vector m_Position;						// light source center position
	Vector m_Direction;						// for SPOT, direction it is pointing
	Vector m_vecUp;							// scaled by the capsule length for capsule lights

	// the values below are derived from the above settings by RecalculateDerivedValues

	// NOTE: These dots are cos( m_Theta ), cos( m_Phi )
	float m_ThetaDot;
	float m_PhiDot;
	float m_TanPhi;
	float m_OneOverThetaDotMinusPhiDot;
	float m_Truncation;						// from the brightness threshold passed to RecalculateDerivedValues
	uint32 m_Flags;							// LightType_OptimizationFlags_t and more

	Vector m_vMinBounds;
	Vector m_vMaxBounds;

	QuaternionAligned m_qPrecomputedOBBOrientation;
	Vector m_vPrecomputedOBBOrigin;
	Vector m_vPrecomputedOBBExtent;
};

#endif
