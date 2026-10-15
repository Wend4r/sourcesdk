#ifndef IMATERIAL2_H
#define IMATERIAL2_H

#ifdef _WIN32
	#pragma once
#endif

#include <resourcesystem/resourcehandle.h>
#include <resourcefile/resourcetype.h>

#include <mathlib/vector4d.h>
#include <tier0/platform.h>
#include <tier0/utlsymbollarge.h>
#include <tier0/utlleanvector.h>
#include <tier0/utlstring.h>
#include <tier0/utlvector.h>
#include "materialsystem2/material_resource_data.h"

class CBufferString;
class CRenderAttributes;
class IMaterialMode;
class InfoForResourceTypeCTextureBase;

//-----------------------------------------------------------------------------
// A single shader parameter of a material. Textures are stored as a resource
// handle in m_hTexture and flagged with MATERIAL_PARAM_TEXTURE.
//-----------------------------------------------------------------------------
enum MaterialParamFlags_t : uint32
{
	MATERIAL_PARAM_TEXTURE = 0x10000,
};

COMPILE_TIME_ASSERT( sizeof( MaterialParam_t ) == 64 );

//-----------------------------------------------------------------------------
// A loaded Source 2 material.
//
// Reconstructed layout. The vtable is declared flat: every slot is kept in its
// verified index, and slots whose purpose is unknown are placeholders named
// after their index. Do not reorder, remove or collapse them. There is no
// virtual destructor in slot 0 - GetName() owns it; the destructor is the
// last slot.
//
// The data members belong to the implementation (CMaterial2), their offsets
// are verified against it.
//-----------------------------------------------------------------------------
abstract_class IMaterial2
{
public:
	virtual const char *GetName() = 0;

	// Name with the "_c" suffix, relative to the mod path when the file is found under GAME
	virtual void GetNameWithMod( CBufferString &sName ) = 0;

	virtual const IMaterial2 *GetCopySource() = 0;
	virtual uint64 GetSimilarityKey() = 0;
	virtual bool IsLoaded() = 0;
	virtual IMaterialMode *GetMode( const CUtlStringToken &mode ) = 0;

	// Material of the mode, this material when it has no such mode
	virtual IMaterial2 *unk006( const CUtlStringToken &mode ) = 0;

	virtual const CRenderAttributes *GetAttributes() = 0;
	virtual void unk008() = 0;
	virtual bool GetBoolAttribute( bool *pValue, CUtlStringToken name ) = 0;
	virtual bool GetIntAttribute( int *pValue, CUtlStringToken name ) = 0;
	virtual bool GetFloatAttribute( float *pValue, CUtlStringToken name ) = 0;
	virtual bool GetStringAttribute( CUtlString *pValue, CUtlStringToken name ) = 0;
	virtual bool GetVector2DAttribute( Vector2D *pValue, CUtlStringToken name ) = 0;
	virtual bool GetVectorAttribute( Vector *pValue, CUtlStringToken name ) = 0;
	virtual bool GetVector4DAttribute( Vector4D *pValue, CUtlStringToken name ) = 0;
	virtual bool GetTextureAttribute( CWeakHandle< InfoForResourceTypeCTextureBase > *pValue, CUtlStringToken name ) = 0;
	virtual CWeakHandle< InfoForResourceTypeCTextureBase > GetFirstTextureAttribute() = 0;
	virtual void GetAllTextureAttributes( void *pAttributes ) = 0;
	virtual const void *GetVertexShaderInputSignature() = 0;
	virtual bool IsBatchableInMode( const CUtlStringToken &mode, int nFlags ) = 0;
	virtual void GetRepresentativeTextureSize( int *pWidth, int *pHeight, int nDefWidth, int nDefHeight, bool bClamp ) = 0;
	// Reads "WorldMappingWidth" / "WorldMappingHeight", or "WorldMappingWidth2" / "WorldMappingHeight2" when nMapping is 1
	virtual void GetWorldMappingSize( int *pWidth, int *pHeight, int nMapping, int nDefWidth, int nDefHeight, bool bClamp ) = 0;
	virtual void unk023() = 0;

	// Only inside an EnterOverrideScope() / LeaveOverrideScope() pair
	virtual bool OverrideTextureParam( const char *pName, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture ) = 0;

	virtual uint32 GetShaderStateOverriddenByMaterial() = 0;
	virtual const CUtlLeanVector< CUtlString > *unk026() = 0;
	virtual void *unk027( const CUtlStringToken &mode, int nShaderType ) = 0;
	virtual CWeakHandleBase unk028() = 0;

	virtual void EnterOverrideScope() = 0;

	// Applies the overridden parameters when the last scope ends
	virtual void LeaveOverrideScope() = 0;

	virtual int GetIntParam( const char *pName, int nFallback ) = 0;
	virtual float GetFloatParam( const char *pName, float flFallback ) = 0;
	virtual const char *GetStringParam( const char *pName, const char *pFallback ) = 0;
	virtual ResourceHandle_t *GetTextureParam( ResourceHandle_t *pOut, const char *pName ) = 0;
	virtual Vector4D *GetVecParam( Vector4D *pOut, const char *pName, const Vector4D *pFallback ) = 0;
	virtual bool unk036() = 0;

	// Updates the constants of every mode from pAttributes
	virtual void unk037( const void *p, const CRenderAttributes *pAttributes ) = 0;

	// Value of the static combo pName of the first loaded mode
	virtual bool unk038( const char *pName ) = 0;
	virtual ~IMaterial2() {}

public:
	// Incremented when the outstanding I/O requests of the material are dropped
	uint16 m_nReloadSerialNumber;
	uint8 m_nOutstandingIORequestCount;

	bool m_bIsLoaded : 1;
	bool m_bIsFallbackMaterial : 1;

	// Set by OverrideTextureParam(), applied and cleared when the last override scope ends
	bool m_bTextureParamsOverridden : 1;

	bool m_bIsFinalized : 1;

	// Returned by unk036()
	bool m_bUnk00B_4 : 1;

private:
	uint8 m_unk00C[ 4 ];

public:
	// Returned by GetName()
	CUtlString m_Name;
	CUtlVector< MaterialParam_t > m_Params;
};

class InfoForResourceTypeIMaterial2
{
public:
	using RuntimeClass_t = IMaterial2;
};

using HMaterialStrong = CStrongHandle< InfoForResourceTypeIMaterial2 >;
using HMaterialWeak = CWeakHandle< InfoForResourceTypeIMaterial2 >;

#endif // IMATERIAL2_H
