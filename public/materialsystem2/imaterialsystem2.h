#ifndef IMATERIALSYSTEM2_H
#define IMATERIALSYSTEM2_H

#ifdef _WIN32
	#pragma once
#endif

#include "imaterial2.h"

#include <appframework/iappsystem.h>
#include <resourcesystem/resourcehandle.h>
#include <resourcefile/resourcetype.h>

#include <tier0/platform.h>
#include <tier0/utlstringtoken.h>
#include <tier0/utlvector.h>

class KeyValues3;
class IRenderContext;
class CUtlString;
class IMaterialMode;
class CRenderAttributes;
struct MaterialRenderablePass_t;
struct MaterialPassConstantBufferInfo_t;
struct MaterialSystem2GlobalStats_t;
struct RsStencilStateOverride_t;
struct RsDepthBiasStateOverride_t;
FORWARD_DECLARE_HANDLE( RenderInputLayout_t );
FORWARD_DECLARE_HANDLE( RenderShaderHandle_t );

//-----------------------------------------------------------------------------
// Source 2 material system, exposed as MATERIAL_SYSTEM2_INTERFACE_VERSION
// ( see g_pMaterialSystem2 ).
//
//
//-----------------------------------------------------------------------------
abstract_class IMaterialSystem2 : public IAppSystem
{
public:
	virtual const char *GetRenderModeName( const CUtlStringToken &renderMode ) = 0;

	// Set two bits of an internal flag byte
	virtual void EnableExtendedMaterialInfo() = 0;
	virtual void EnableHeadlessMode() = 0;

	virtual HMaterialWeak FindOrCreateMaterialFromResource( const char *pMaterialName ) = 0;

	virtual bool SetRenderStateForRenderablePass( const CRenderAttributes *pAttributes, const CRenderAttributes *pAttributes2, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const MaterialRenderablePass_t &renderablePass, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;
	virtual bool SetRenderStateForRenderablePass( const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const MaterialRenderablePass_t &renderablePass, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	virtual RenderShaderHandle_t GetShaderForRenderablePass( int nShaderType, const MaterialRenderablePass_t &renderablePass ) = 0;
	virtual int GetConstantBufferInfoForRenderablePass( int nShaderType, const MaterialRenderablePass_t &renderablePass, MaterialPassConstantBufferInfo_t *pBufferInfo, int nBufferInfoMax ) = 0;
	virtual bool Unk_GetRenderStatesForRenderablePass( void *p ) = 0;

	virtual void FrameUpdate() = 0;

	virtual void EnableDynamicShaderCompile() = 0;
	virtual bool IsDynamicShaderCompileEnabled() = 0;

	virtual bool SetMaterialParamFromString( ResourceHandle_t hMaterial, const char *pParamName, const char *pValue ) = 0;

	// Callbacks are run at the start of every FrameUpdate
	virtual void AddFrameUpdateCallback( void ( *pfnCallback )() ) = 0;
	virtual void RemoveFrameUpdateCallback( void ( *pfnCallback )() ) = 0;

	virtual void QueueMaterialReload() = 0;

	// Names the copy "<source>_copy<n>"
	virtual HMaterialWeak CreateProceduralMaterialCopy( ResourceHandle_t hSrcMaterial, int nMode, bool bUnk ) = 0;

	// Creates a material from scratch out of a KeyValues3 description.
	virtual HMaterialWeak CreateMaterial( const char *pMaterialName, KeyValues3 *pData, int nFlags, bool bUnk ) = 0;

	// Creates a material derived from an already loaded one, applying overrides on top of it.
	virtual HMaterialWeak CreateMaterial( const char *pMaterialName, ResourceHandle_t hSrcMaterial, KeyValues3 *pOverrides, int nFlags, bool bUnk ) = 0;

	virtual HMaterialWeak unk030( const char *pMaterialName, KeyValues3 *pData, ResourceHandle_t hResource, bool bUnk ) = 0;

	// Value of setting.shaderquality
	virtual int GetShaderQuality() = 0;

	// Zeroes the 36-byte block at pStats and returns false
	virtual bool GetGlobalStats( MaterialSystem2GlobalStats_t *pStats, bool bClear ) = 0;

	virtual int GetMaterialGeneration() = 0;
	virtual void IncrementMaterialGeneration() = 0;

	virtual void GetRenderModes( CUtlVector< CUtlStringToken > *pRenderModes, const CUtlVector< const char * > *pModeTypes ) = 0;

	virtual bool GetKV3MaterialDescription( ResourceHandle_t hMaterial, KeyValues3 *pKV ) = 0;

	virtual void unk037( ResourceHandle_t hMaterial, CUtlVector< ResourceHandle_t > *pResources ) = 0;

	virtual void AddShaderReloadListener( void *pListener ) = 0;
	virtual void RemoveShaderReloadListener( void *pListener ) = 0;

	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext ) = 0;
	virtual bool SetRenderStateForMode( const IMaterialMode *pMode, const CRenderAttributes *pAttributes, IRenderContext *pRenderContext, RenderInputLayout_t hInputLayout, const RsStencilStateOverride_t *pOverrideStencilState, const RsDepthBiasStateOverride_t *pOverrideDepthBiasState, int nRequiredMipSize, uint32 nRenderStateSetNotAllowed ) = 0;

	virtual void unk042( int nCount, IMaterial2 *const *ppMaterials, const uint16 *pValues ) = 0;

	virtual HMaterialWeak unk043( int nMaterialId ) = 0;

	// "pc" or "vulkan" depending on the render device
	virtual void unk044( CUtlString &sName ) = 0;

	// 2 when rendering through Vulkan, 0 otherwise
	virtual int unk045() = 0;

	virtual bool unk046( bool bValue ) = 0;

	virtual void AddMaterialParamChangedListener( void *pListener ) = 0;
	virtual void RemoveMaterialParamChangedListener( void *pListener ) = 0;

	virtual ~IMaterialSystem2() {}
};

#endif // IMATERIALSYSTEM2_H
