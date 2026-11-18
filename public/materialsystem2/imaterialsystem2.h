#ifndef IMATERIALSYSTEM2_H
#define IMATERIALSYSTEM2_H

#ifdef _WIN32
	#pragma once
#endif

#include "imaterial2.h"

#include <appframework/iappsystem.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcesystem/resourcehandletypes.h>
#include <resourcefile/resourcetype.h>

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>

class KeyValues3;
class IRenderContext;
class CRenderAttributes;
class IMaterialMode;
class CUtlString;
struct MaterialRenderablePass_t;
struct MaterialPassConstantBufferInfo_t;
struct MaterialSystem2GlobalStats_t;
struct RsStencilStateOverride_t;
struct RsDepthBiasStateOverride_t;
struct RenderInputLayout_t__;
struct RenderShaderHandle_t__;

typedef RenderInputLayout_t__ *RenderInputLayout_t;
typedef RenderShaderHandle_t__ *RenderShaderHandle_t;
typedef void ( *FrameUpdateFunc_t )();

enum RenderShaderType_t : int;
enum ProceduralResourceType_t : int;

//-----------------------------------------------------------------------------
// Source 2 material system, exposed as MATERIAL_SYSTEM2_INTERFACE_VERSION
// ( see g_pMaterialSystem2 ).
//
// Names from the s&box DWARF of IMaterialSystem2 where the CS2 body matches;
// CS2 has no RsFillModeOverride_t argument in the render state setters. The
// slots s&box does not have are named by what they do.
//-----------------------------------------------------------------------------
abstract_class IMaterialSystem2 : public IAppSystem
{
public:
	virtual const char *GetModeString( const CUtlStringToken &shaderMode ) = 0;

	// Set bit 6 and bit 7 of the flag byte at +560
	virtual void EnableExtendedMaterialInfo() = 0;
	virtual void EnableHeadlessMode() = 0;

	virtual HMaterialWeak FindOrCreateMaterialFromResource( const char *pResourceName ) = 0;

	virtual bool SetRenderStateForRenderablePass( const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const MaterialRenderablePass_t &renderablePass, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) const = 0;
	virtual bool SetRenderStateForRenderablePass( const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const MaterialRenderablePass_t &renderablePass, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) const = 0;

	virtual RenderShaderHandle_t GetShaderForRenderablePass( RenderShaderType_t programType, const MaterialRenderablePass_t &renderablePass ) const = 0;
	virtual int GetConstantBufferInfoForRenderablePass( RenderShaderType_t programType, const MaterialRenderablePass_t &renderablePass, MaterialPassConstantBufferInfo_t *pBufferInfo, int nBufferInfoMax ) const = 0;

	// Named by what it does: validates the pass for the shader type (logs when the pass data
	// crossed a frame boundary) and writes the index of its per-shader data to pIndexOut
	virtual bool GetShaderDataIndexForRenderablePass( RenderShaderType_t programType, const MaterialRenderablePass_t &renderablePass, int *pIndexOut ) const = 0;

	virtual void FrameUpdate() = 0;

	virtual void EnableDynamicShaderCompile() = 0;
	virtual bool IsDynamicShaderCompileEnabled() = 0;

	virtual bool SetMaterialParam_MaterialEditor( HMaterialWeak hMaterial, const char *pParamName, const char *pParamValueAsString ) = 0;

	// Callbacks are run at the start of every FrameUpdate
	virtual void AddFrameUpdateFunc( FrameUpdateFunc_t pfnFunc ) = 0;
	virtual void RemoveFrameUpdateFunc( FrameUpdateFunc_t pfnFunc ) = 0;

	virtual void QueueMaterialReload() = 0;

	// Creates "<name>_copy<n>"
	virtual HMaterialWeak CreateProceduralMaterialCopy( HMaterialWeak hSourceMaterial, ProceduralResourceType_t nResourceType, bool bLoadShadersNow ) = 0;

	virtual HMaterialWeak CreateProceduralMaterial( const char *pMaterialName, const KeyValues3 &data, ProceduralResourceType_t nResourceType, bool bLoadShadersNow ) = 0;
	virtual HMaterialWeak CreateProceduralMaterial( const char *pMaterialName, HMaterialWeak hSourceMaterial, const KeyValues3 &overrides, ProceduralResourceType_t nResourceType, bool bLoadShadersNow ) = 0;

	// Named by what it does: like the KeyValues3 CreateProceduralMaterial, and keeps a counted
	// reference to hShaderRef in the new material (see IMaterial2::GetShaderExtRef)
	virtual HMaterialWeak CreateProceduralMaterialWithShaderRef( const char *pMaterialName, const KeyValues3 *pData, ResourceHandle_t hShaderRef, bool bLoadShadersNow ) = 0;

	// Value of setting.shaderquality
	virtual int GetShaderQuality() = 0;

	// CS2 zeroes the 36-byte block and returns
	virtual bool GetGlobalStats( MaterialSystem2GlobalStats_t *pStats, bool bClear ) = 0;

	virtual int GetMaterialGeneration() = 0;
	virtual void IncrementMaterialGeneration() = 0;

	virtual void GetModes( CUtlVector< CUtlStringToken > *pModeTokensOut, const CUtlVector< const char * > *pCategoryFiltersOptional ) const = 0;

	virtual bool GetKV3MaterialDescription( HMaterialWeak hMaterial, KeyValues3 *pKeyValuesOut ) const = 0;

	// Named by what it does: appends the texture of every texture parameter without a buffer
	virtual void GetMaterialTextureParams( HMaterialWeak hMaterial, CUtlVector< ResourceHandle_t > *pTexturesOut ) const = 0;

	virtual void AddShaderReloadListener( void *pListener ) = 0;
	virtual void RemoveShaderReloadListener( void *pListener ) = 0;

	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext ) const = 0;
	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) const = 0;

	// Named by what it does: when the render device supports it, passes the textures of
	// every material together with its 16-bit value to the render device
	virtual void SetMaterialTextureValues( int nCount, IMaterial2 *const *ppMaterials, const uint16 *pValues ) = 0;

	// Named by what it does: the vmat whose material has this m_nUniqueId (CMaterial2 +60)
	virtual HMaterialWeak FindMaterialByUniqueId( int nUniqueId ) = 0;

	// Named by what they do: "pc" or "vulkan", and 2 for a Vulkan render device (0 otherwise)
	virtual void GetVfxPlatformString( CUtlString &sPlatform ) = 0;
	virtual int GetVfxPlatformType() = 0;

	// Named by what it does: sets the flag at +704 and returns the previous value; the flag is
	// ORed into the "load shaders now" argument of the material mode setup
	virtual bool SetForceSynchronousShaderLoad( bool bForce ) = 0;

	virtual void AddMaterialParamChangedListener( void *pListener ) = 0;
	virtual void RemoveMaterialParamChangedListener( void *pListener ) = 0;

	virtual ~IMaterialSystem2() {}
};

#endif // IMATERIALSYSTEM2_H
