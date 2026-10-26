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
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>
#include "materialsystem2/material_resource_data.h"

class CBufferString;
class CRenderAttributes;
class CVsInputSignatureVector;
class IMaterialMode;
class IRenderContext;
class InfoForResourceTypeCTextureBase;

// An entry IMaterial2::GetAllTextureAttributes() appends, one per texture attribute of the material
struct MaterialTextureAttribute_t
{
	CUtlStringToken m_Name;

	// -1 binds every mip
	int32 m_nSpecificMipLevelToBind;

	// HRenderTexture
	CWeakHandleBase m_hTexture;
};

// The state of a mode computed for a set of render attributes: the dynamic combos the shader types use
struct MaterialRenderablePass_t
{
	const IMaterialMode *pMode;
	uint32 dynamicComboArrayIdx[ 7 ];

	// The render state bits the selected dynamic combos set
	uint32 nAllStateSetBits;

	// Hash of the mode key and the dynamic combos
	uint64 nHash;

	// Frame count of the material system when the pass was computed; using it in a later frame is an error
	uint32 nFrameCount;
};

// A render mode of a material (CMaterialMode)
abstract_class IMaterialMode
{
public:
	// Selects the dynamic combos for the attributes, through a per-thread cache when mat_cache_renderablepasses is
	// set; returns 0 when the mode cannot be rendered. bForce is ORed with a material system flag
	virtual int ComputeRenderablePasses( const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2, MaterialRenderablePass_t *pRenderablePass, bool bForce ) = 0;

	// Same with the render attributes of the context
	virtual int ComputeRenderablePassesForContext( const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, MaterialRenderablePass_t *pRenderablePass, bool bForce ) = 0;

	// Computes the pass for the attributes, then returns the entry of shader type 6 for the combo through ppOut and
	// fills pData with that type's constant data
	virtual bool unk002( void **ppOut, CUtlVector< uint8 > *pData, uint32 *pOut, const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2 ) = 0;

	// Whether the mode's shader matches the current shader generation of the material system
	virtual bool IsShaderUpToDate() = 0;

	// The key hashed into MaterialRenderablePass_t::nHash
	virtual uint64 GetModeKey() = 0;
};

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

	// GetSimilarityKey() of a single mode: the key computed from the static combos of the mode, the address of this
	// material when it has no such mode
	virtual uint64 GetModeSimilarityKey( const CUtlStringToken &mode ) = 0;

	virtual const CRenderAttributes *GetAttributes() = 0;
	virtual bool HasAttribute( CUtlStringToken name ) = 0;
	virtual bool GetBoolAttribute( bool *pValue, CUtlStringToken name ) = 0;
	virtual bool GetIntAttribute( int *pValue, CUtlStringToken name ) = 0;
	virtual bool GetFloatAttribute( float *pValue, CUtlStringToken name ) = 0;
	virtual bool GetStringAttribute( CUtlString *pValue, CUtlStringToken name ) = 0;
	virtual bool GetVector2DAttribute( Vector2D *pValue, CUtlStringToken name ) = 0;
	virtual bool GetVectorAttribute( Vector *pValue, CUtlStringToken name ) = 0;
	virtual bool GetVector4DAttribute( Vector4D *pValue, CUtlStringToken name ) = 0;
	virtual bool GetTextureAttribute( CWeakHandle< InfoForResourceTypeCTextureBase > *pValue, CUtlStringToken name ) = 0;
	virtual CWeakHandle< InfoForResourceTypeCTextureBase > GetFirstTextureAttribute() = 0;
	virtual void GetAllTextureAttributes( CUtlVector< MaterialTextureAttribute_t > *pAttributes ) = 0;
	virtual const CVsInputSignatureVector *GetVertexShaderInputSignature() = 0;
	virtual bool IsBatchableInMode( const CUtlStringToken &mode, int nFlags ) = 0;
	virtual void GetRepresentativeTextureSize( int *pWidth, int *pHeight, int nDefWidth, int nDefHeight, bool bClamp ) = 0;
	// Reads "WorldMappingWidth" / "WorldMappingHeight", or "WorldMappingWidth2" / "WorldMappingHeight2" when nMapping is 1
	virtual void GetWorldMappingSize( int *pWidth, int *pHeight, int nMapping, int nDefWidth, int nDefHeight, bool bClamp ) = 0;
	// CMaterial2 returns true without reading any state; nothing calls it
	virtual bool unk023() = 0;

	// Only inside an EnterOverrideScope() / LeaveOverrideScope() pair
	virtual bool OverrideTextureParam( const char *pName, CWeakHandle< InfoForResourceTypeCTextureBase > hTexture ) = 0;

	virtual uint32 GetShaderStateOverriddenByMaterial() = 0;
	virtual const CUtlLeanVector< CUtlString > *GetRenderAttributesUsed() = 0;
	// Parameters of the mode's shader program of nShaderType (-1 to 6): a CUtlVector of 0x218 byte constant,
	// sampler and render state descriptions that the material binds; NULL when the mode or type is invalid
	virtual void *GetShaderParams( const CUtlStringToken &mode, int nShaderType ) = 0;
	// Resource referenced by the material, see IMaterialSystem2::CreateMaterialReferencingResource()
	virtual CWeakHandleBase GetReferencedResource() = 0;

	virtual void EnterOverrideScope() = 0;

	// Applies the overridden parameters when the last scope ends
	virtual void LeaveOverrideScope() = 0;

	virtual int GetIntParam( const char *pName, int nFallback ) = 0;
	virtual float GetFloatParam( const char *pName, float flFallback ) = 0;
	virtual const char *GetStringParam( const char *pName, const char *pFallback ) = 0;
	virtual ResourceHandle_t *GetTextureParam( ResourceHandle_t *pOut, const char *pName ) = 0;
	virtual Vector4D *GetVecParam( Vector4D *pOut, const char *pName, const Vector4D *pFallback ) = 0;
	virtual bool HasDynamicParams() = 0;

	// Updates the constants of every mode from pAttributes, called with the material's own attributes after a
	// parameter changes
	virtual void UpdateConstants( const void *p, const CRenderAttributes *pAttributes ) = 0;

	// Whether the static combo pName of the first loaded mode stays set once the combo rules are applied
	virtual bool IsStaticComboEnabled( const char *pName ) = 0;
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

	// Set when the resource data has dynamic or dynamic texture parameters
	bool m_bHasDynamicParams : 1;

private:
	uint8 m_pad00C[ 4 ];

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
